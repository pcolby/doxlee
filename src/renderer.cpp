/*
    Copyright 2021 Paul Colby

    This file is part of doxlee.

    Doxlee is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Doxlee is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with doxlee.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "variant.h"

#include "renderer.h"

#include <grantlee/cachingloaderdecorator.h>
#include <grantlee/templateloader.h>
#include <QCoreApplication>
#include <QDebug>
#include <QDirIterator>
#include <QRegularExpression>
#include <QXmlStreamReader>

/// Shorten the QStringLiteral macro for readability.
#define QSL(str) QStringLiteral(str)

/// Shorten QCoreApplication::translate calls for readability.
#define QTR(str) QCoreApplication::translate("Renderer", str)

namespace doxlee {

Renderer::Renderer(const QString &inputDir) : inputDir(inputDir)
{
    // Default Grantlee context values.
    context.insert(QSL("doxleeVersion"), QStringLiteral(CMAKE_PROJECT_VERSION));

    // Configure the Grantlee Templates rendering engine.
    engine.setSmartTrimEnabled(true);

    // Parse the XML index
    /// \todo the caller should be able to detect failure here.
    parseIndex(this->inputDir.absoluteFilePath(QSL("index.xml")), context)

    // Supplement the compounds index data (from parseIndex) with additional views.
    && supplementIndexes(context);
}

bool Renderer::loadTemplates(const QString &templatesDir)
{
    // Fetch the list of compound and member kinds supported by the Doxgen version.
    auto kinds = getKinds(inputDir.absoluteFilePath(QSL("index.xsd")));
    if ((kinds.first.isEmpty()) && (kinds.second.isEmpty())) {
        return false; // getKinds failed; and reported an appropriate error.
    }

    // Setup the template loader.
    auto loader = QSharedPointer<Grantlee::FileSystemTemplateLoader>::create();
    // Note, {% include "<filename>" %} will look for files relative to templateDirs.
    loader->setTemplateDirs(QStringList() << templatesDir);
    auto cachedLoader = QSharedPointer<Grantlee::CachingLoaderDecorator>::create(loader);
    engine.addTemplateLoader(cachedLoader);

    // Load the templates.
    QDirIterator dir(templatesDir, QDir::Files|QDir::Readable, QDirIterator::Subdirectories);
    int otherFilesCount=0;
    while (dir.hasNext()) {
        // Fetch the next entry.
        const QString relativePathName = dir.next().mid(dir.path().size()+1);
        qDebug().noquote() << QTR("Inspecting template: %1 (%2)")
            .arg(dir.filePath(), relativePathName);

        // Check for 'static' directory names in the local path.
        if (relativePathName.split(QLatin1Char('/')).contains(QSL("static"))) {
            staticFileNames.append({dir.filePath(), relativePathName});
            continue;
        }

        // Extract the 'kind' string, if any (everything up to the first non-alphanemeric char).
        QString kind = getKindFromFileName(dir.fileName());
        qDebug().noquote() << QTR("Inspecting template: %1 (%2,%3)")
            .arg(dir.filePath(), relativePathName, kind);

        // If the 'kind' is static, just record it for copying to the output later.
        if (kind == QSL("static")) {
            staticFileNames.append({dir.filePath(), relativePathName});
            continue;
        }

        // Load the template, and store against the relevant compound kind.
        if ((kind == QSL("index")) || (kinds.first.contains(kind))) {
            qDebug().noquote() << QTR("Loading template: %1 (%2,%3)")
                .arg(dir.filePath(), relativePathName, kind);
            const Grantlee::Template tmplate = engine.loadByName(relativePathName);
            if (tmplate->error()) {
                qWarning().noquote() << QTR("Error loading template: %1 - %2")
                    .arg(relativePathName, tmplate->errorString());
                return false;
            }
            if (kind == QSL("index")) {
                indexTemplateNames.append(relativePathName);
            } else {
                templateNamesByKind.insert(kind, relativePathName);
            }
            continue;
        }
        otherFilesCount++;
    }
    qInfo().noquote() << QTR("Loaded %1 template(s), alongside %2 static file(s)")
        .arg(indexTemplateNames.size() + templateNamesByKind.size() + otherFilesCount)
        .arg(staticFileNames.size());
    return true;
}

// Estimate number of files that will be generated.
// Also report if any compounds have no templates.
int Renderer::expectedFileCount() const
{
    int count = indexTemplateNames.count() + staticFileNames.count();
    const QVariantMap compoundsByKind = context.lookup(QSL("compoundsByKind")).toMap();
    for (auto iter = compoundsByKind.constBegin(); iter != compoundsByKind.constEnd(); ++iter) {
        const int templatesCount = templateNamesByKind.count(iter.key());
        const int itemsCounts = iter.value().toList().size();
        if (templatesCount == 0) {
            qWarning().noquote() << QTR("Found documentation for %1 %2 compound(s), "
                                        "but no specialised templates for %2 compounds")
                                        .arg(itemsCounts).arg(iter.key());
        }
        count += templatesCount * itemsCounts;
    }
    return count;
}

bool Renderer::render(const QDir &outputDir, ClobberMode clobberMode)
{
    // Render all compounds (and members) we have templates for.
    const QVariantMap compoundsByKind = context.lookup(QSL("compoundsByKind")).toMap();
    for (auto iter = compoundsByKind.constBegin(); iter != compoundsByKind.constEnd(); ++iter) {
        const QStringList templateNames = templateNamesByKind.values(iter.key());
        if (templateNames.empty()) continue;
        if (!render(iter.value().toList(), templateNames, outputDir, context, clobberMode))
            return false;
    }

    // Render all index templates.
    for (const QString &templateName: qAsConst(indexTemplateNames)) {
        Q_ASSERT(templateName.startsWith(QSL("index")));
        const QString outputPath = outputDir.absoluteFilePath(
            (templateName.lastIndexOf(QLatin1Char('.')) == 5) ? templateName : templateName.mid(6));
        if (!render(templateName, outputPath, context, clobberMode))
            return false;
    }

    // Copy all static files.
    for (auto iter = staticFileNames.begin(); iter != staticFileNames.end(); ++iter) {
        const QString destination = outputDir.absoluteFilePath(iter->second);
        if (!copy(iter->first, destination, clobberMode)) {
            return false;
        }
    }
    return true;
}

int Renderer::outputFileCount() const
{
    return filesWritten.size();
}

QString Renderer::compoundPathName(const QVariantMap &compound, const QString &templateName)
{
    // Start by breaking the path into the fileName and dirName (if any).
    const int pos = templateName.lastIndexOf(QLatin1Char('/'));
    QString fileName = (pos<0) ? templateName : templateName.mid(pos+1);
    QString dirName = (pos<0) ? QString() : templateName.left(pos);

    // Strip the "<kind>[char]" prefix from fileName, but remember which separator (if any) was used.
    const QString kind = compound.value(QSL("kind")).toString();
    Q_ASSERT(fileName.startsWith(kind));
    QChar separator;
    fileName = fileName.mid(kind.length());
    if ((!fileName.isEmpty()) && (fileName.at(0) != QLatin1Char('.'))) {
        separator = fileName.at(0);
        fileName.remove(0,1);
    }
    if ((fileName.isEmpty()) || (fileName.at(0) == QLatin1Char('.'))) {
        fileName.prepend(QSL("refid"));
    }

    // Replace the supported compound tokens.
    const QStringList tokens { QSL("kind"), QSL("name"), QSL("refid") };
    for (const QString &token: tokens) {
        const QRegularExpression before(QSL("(^|\\W|_)%1(\\W|_|$)").arg(token));
        const QString after = QSL("\\1%1\\2").arg(compound.value(token).toString());
        fileName.replace(before, after);
        dirName.replace(before, after);
    }
    if (!separator.isNull()) {
        fileName.remove(separator);
        dirName.remove(separator);
    }
    if (!dirName.isEmpty()) dirName.append(QLatin1Char('/'));
    return dirName + fileName;
}

QString Renderer::getKindFromFileName(const QString &fileName)
{
    QString kind = fileName.split(QLatin1Char('/')).last();
    const QString::ConstIterator pos = std::find_if_not(kind.constBegin(), kind.constEnd(),
        [](const QChar &c){ return c.isLetterOrNumber();});
    if (pos != kind.constEnd()) {
        kind.truncate(pos - kind.constBegin());
    }
    return kind;
}

QPair<QStringList,QStringList> Renderer::getKinds(const QString &indexXsdPath)
{
    // Open the file for reading.
    QFile file(indexXsdPath);
    if (!file.open(QIODevice::ReadOnly|QIODevice::Text)) {
        qWarning().noquote() << QTR("Error opening file for reading: %1").arg(indexXsdPath);
        return { };
    }

    QStringList compoundKinds, memberKinds;

    // Parse the opening <schema> element.
    QXmlStreamReader xml(&file);
    if (!xml.readNextStartElement()) {
        qWarning().noquote() << QTR("Invalid XML file: %1 - %2").arg(indexXsdPath, xml.errorString());
        return { };
    }
    if (xml.name() != QSL("schema")) {
        qWarning().noquote() << QTR("File is not a Doxygen XML index schema: %1 - %2")
                                .arg(indexXsdPath, xml.name().toString());
        return { };
    }

    // Parse the contained 'kind' elements.
    while ((!xml.atEnd()) && (xml.readNextStartElement())) {
        if (xml.name() == QSL("simpleType")) {
            const QString nameAttribute = xml.attributes().value(QSL("name")).toString();
            if ((nameAttribute == QSL("CompoundKind")) || (nameAttribute == QSL("MemberKind"))) {
                while ((!xml.atEnd()) && (xml.readNextStartElement())) {
                    if (xml.name() == QSL("restriction")) {
                        QStringList &kinds = (nameAttribute == QLatin1String("CompoundKind"))
                            ? compoundKinds : memberKinds;
                        while ((!xml.atEnd()) && (xml.readNextStartElement())) {
                            if (xml.name() == QSL("enumeration")) {
                                kinds.append(xml.attributes().value(QSL("value")).toString());
                            }
                            xml.skipCurrentElement();
                        }
                    } else xml.skipCurrentElement();
                }
            } else xml.skipCurrentElement();
        } else xml.skipCurrentElement();
    }
    qInfo().noquote() << QTR("Parsed %1 compound kind(s), and %2 member kind(s) from %3")
        .arg(compoundKinds.size()).arg(memberKinds.size()).arg(indexXsdPath);
    return { compoundKinds, memberKinds };
}

bool Renderer::parseIndex(const QString &fileName, Grantlee::Context &context)
{
    // Open the file for reading.
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly|QIODevice::Text)) {
        qWarning().noquote() << QTR("Error opening file for reading: %1").arg(fileName);
        return false;
    }

    // Parse the opening <doxygenindex> element.
    QXmlStreamReader xml(&file);
    if (!xml.readNextStartElement()) {
        qWarning().noquote() << QTR("Invalid XML file: %1 - %2").arg(fileName, xml.errorString());
        return false;
    }
    qDebug() << xml.name() << "version" << xml.attributes().value(QSL("version"))
             << xml.attributes().value(QSL("xml:lang")).toString();
    if (xml.name() != QSL("doxygenindex")) {
        qWarning().noquote() << QTR("File is not a Doxygen XML index: %1 - %2")
                                .arg(fileName, xml.name().toString());
        return false;
    }
    context.insert(QSL("doxygenVersion"), xml.attributes().value(QSL("version")).toString());
    context.insert(QSL("doxygenLanguage"), xml.attributes().value(QSL("xml:lang")).toString());

    // Parse the contained <compound> elements.
    QVariantList compounds;
    while ((!xml.atEnd()) && (xml.readNextStartElement())) {
        if (xml.name() == QSL("compound")) {
            QVariantMap compound;
            compound.insert(QSL("refid"), xml.attributes().value(QSL("refid")).toString());
            compound.insert(QSL("kind"), xml.attributes().value(QSL("kind")).toString());
            if ((!xml.readNextStartElement()) || (xml.name() != QSL("name"))) {
                qWarning().noquote() << QTR(" %1:%2:%3 <compound> does not begin with <name>")
                    .arg(fileName).arg(xml.lineNumber()).arg(xml.columnNumber());
                return false;
            }
            compound.insert(QSL("name"), xml.readElementText());
            //qDebug() << __func__ << "compound" << compound;
            QVariantList members;
            while ((!xml.atEnd()) && (xml.readNextStartElement())) {
                if (xml.name() == QSL("member")) {
                    QVariantMap member;
                    member.insert(QSL("refid"), xml.attributes().value(QSL("refid")).toString());
                    member.insert(QSL("kind"), xml.attributes().value(QSL("kind")).toString());
                    if ((!xml.readNextStartElement()) || (xml.name() != QSL("name"))) {
                        qWarning().noquote() << QTR("%1:%2:%3 <member> does not begin with <name>")
                            .arg(fileName).arg(xml.lineNumber()).arg(xml.columnNumber());
                        return false;
                    }
                    member.insert(QSL("name"), xml.readElementText());
                    //qDebug() << __func__ << "member" << member;
                    members.append(member);
                    xml.skipCurrentElement();
                } else {
                    qWarning().noquote() << QTR("Skipping unknown <%1> element at %2:%3:%4")
                        .arg(xml.name().toString(), fileName).arg(xml.lineNumber()).arg(xml.columnNumber());
                    xml.skipCurrentElement();
                }
            }
            compound.insert(QSL("members"), members);
            compounds.append(compound);
        } else {
            qWarning().noquote() << QTR("Skipping unknown <%1> element at %2:%3:%4")
                .arg(xml.name().toString(), fileName).arg(xml.lineNumber()).arg(xml.columnNumber());
            xml.skipCurrentElement();
        }
    }
    qInfo().noquote() << QTR("Parsed %1 compound(s) from %2").arg(compounds.size()).arg(fileName);
    context.insert(QSL("compoundsList"), compounds);
    return true;
}

bool Renderer::promptToOverwrite(const QString &pathName, ClobberMode &clobberMode)
{
    Q_ASSERT(clobberMode == Prompt);
    while (true) {
        qWarning().noquote() << QTR("Overwrite %1 [y,n,a,s,q,?]? ").arg(pathName);
        QTextStream stream(stdin);
        const QString response = stream.readLine();
        if (response == QSL("y")) {
            return true;
        } else if (response == QSL("n")) {
            return false;
        } else if (response == QSL("a")) {
            clobberMode = Overwrite;
            return true;
        } else if (response == QSL("s")) {
            clobberMode = Skip;
            return false;
        } else if (response == QSL("q")) {
            exit(255);
        } else {
            qInfo().noquote() << QTR("y - overwrite this file");
            qInfo().noquote() << QTR("n - do not overwrite this file");
            qInfo().noquote() << QTR("a - overwrite this, and all remaining files");
            qInfo().noquote() << QTR("s - do not overwrite this, or any remaining files");
            qInfo().noquote() << QTR("q - quit now, without writing any further files");
            qInfo().noquote() << QTR("? - print help");
        }
   }
}

inline void sortBy(QVariantList &list, const QString &name)
{
    std::sort(list.begin(), list.end(),
        [&name](const QVariant &a, const QVariant &b) {
            return a.toMap().value(name).toString() < b.toMap().value(name).toString();
        });
}

bool Renderer::supplementIndexes(Grantlee::Context &context)
{
    const QVariantList compounds = context.lookup(QSL("compoundsList")).toList();
    QHash<QString,QVariantList> compoundsByKind, membersByKind;
    QVariantMap compoundsByRefId, membersByRefId;
    for (const QVariant &compound: compounds) {
        const QVariantMap compoundMap = compound.toMap();
        {
            const QString kind = compoundMap.value(QSL("kind")).toString();
            const QString refid = compoundMap.value(QSL("refid")).toString();
            compoundsByKind[kind].append(compound);
            compoundsByRefId.insert(refid, compound);
        }

        const QVariantList members = compoundMap.value(QSL("members")).toList();
        for (const QVariant &member: members) {
            const QVariantMap memberMap = member.toMap();
            const QString kind = memberMap.value(QSL("kind")).toString();
            const QString refid = memberMap.value(QSL("refid")).toString();
            membersByKind[kind].append(member);
            membersByRefId.insert(refid, member);
        }
    }
    for (QVariantList &compoundsList: compoundsByKind) sortBy(compoundsList, QSL("name"));
    for (QVariantList &membersList: membersByKind)     sortBy(membersList,   QSL("name"));
    context.insert(QSL("compoundsByKind" ), toVariant(compoundsByKind));
    context.insert(QSL("compoundsByRefId"), compoundsByRefId);
    context.insert(QSL("membersByKind"   ), toVariant(membersByKind));
    context.insert(QSL("membersByRefId"  ), membersByRefId);
    return true;
}

bool Renderer::copy(const QString &fromPath, const QString &toPath, ClobberMode &clobberMode)
{
    qDebug() << __func__ << fromPath << toPath << clobberMode;

    QFileInfo toFileInfo(toPath);
    if (toFileInfo.exists()) {
        switch (clobberMode) {
        case Prompt:
            if (!promptToOverwrite(toPath, clobberMode)) {
                qDebug().noquote() << QTR("Skipping existing output file: %1").arg(toPath);
                return true;
            }
            __attribute__((fallthrough)); // Fall-through to Overwrite behaviour.
        case Overwrite:
            if (!QFile::remove(toPath)) {
                qWarning().noquote() << QTR("Failed to copy over existing file: %1").arg(toPath);
            }
            break;
        case Skip:
            qDebug().noquote() << QTR("Skipping existing output file: %1").arg(toPath);
            return true;
        }
    }

    if (!toFileInfo.dir().exists()) {
        toFileInfo.dir().mkpath(QSL("./"));
    }

    if (!QFile::copy(fromPath, toPath)) {
        qWarning() << QTR("Failed to copy %1 to %2").arg(fromPath, toPath);
        return false;
    }
    filesWritten.append(toPath);
    return true;
}

bool Renderer::render(const QVariantList &compounds, const QStringList &templateNames,
                      const QDir &outputDir, Grantlee::Context &context, ClobberMode &clobberMode)
{
    // Note, we're effectively doing a product of compounds * templates here, which could be quite a
    // lot of processing. We choose to iterate items in the outer loop, so we only parse each item
    // once. Whereas repeatedly loading templates in the inner loop is fairly cheap, since we
    // allocated a caching template loader earlier. We could of course, invert the loops for the
    // same ouput, just an order of magnitude slower and/or using more RAM to cache parsed XML.
    for (const QVariant &compound: compounds) {
        // Parse the item's Doxygen XML data.
        const QString refId = compound.toMap().value(QSL("refid")).toString();
        const QString xmlFilePath = inputDir.absoluteFilePath(refId + QSL(".xml"));
        QFile file(xmlFilePath);
        if (!file.open(QIODevice::ReadOnly|QIODevice::Text)) {
            qWarning().noquote() << QTR("Error opening file for reading: %1").arg(xmlFilePath);
            return false;
        }
        QXmlStreamReader xml(&file);
        const QVariantMap compoundDefinition = toVariant(xml).value(QSL("doxygen")).toMap()
            .value(QSL("compounddef")).toMap();
        if (compoundDefinition.isEmpty()) {
            qWarning().noquote() << QTR("Error reading compond defintion: %1").arg(xmlFilePath);
            return false;
        }

        // Render the output for each template.
        context.push();
        context.insert(QSL("compound"), compoundDefinition);
        for (const QString &templateName: templateNames) {
            const QString outputPath = outputDir.absoluteFilePath(
                compoundPathName(compound.toMap(), templateName));
            if (!render(templateName, outputPath, context, clobberMode)) {
                context.pop();
                return false;
            }
        }
        context.pop();
    }
    return true;
}

bool Renderer::render(const QString &templateName, const QString &outputPath,
                      Grantlee::Context &context, ClobberMode &clobberMode)
{
    qDebug() << __func__ << templateName << outputPath << clobberMode;

    QFileInfo toFileInfo(outputPath);
    if (toFileInfo.exists()) {
        switch (clobberMode) {
        case Overwrite:
            // QFile::open below will happily overwrite (if we have write permission).
            break;
        case Prompt:
            if (promptToOverwrite(outputPath, clobberMode))
                break; // QFile::open below will happily overwrite (if we have write permission).
            __attribute__((fallthrough)); // else fall-through to Skip behaviour.
        case Skip:
            qDebug() << QTR("Skipping existing output file: %1").arg(outputPath);
            return true;
        }
    }

    const Grantlee::Template tmplate = engine.loadByName(templateName);
    if (tmplate->error()) {
        qWarning().noquote() << QTR("Error loading template: %1 - %2")
            .arg(templateName, tmplate->errorString());
        return false;
    }

    if (!toFileInfo.dir().exists()) {
        toFileInfo.dir().mkpath(QSL("./"));
    }

    QFile file(outputPath);
    if (!file.open(QFile::WriteOnly)) {
        qWarning().noquote() << QTR("Failed to open file for writing: %1").arg(outputPath);
        return false;
    }

    QTextStream textStream(&file);
    //NoEscapeStream noEscapeStream(&textStream); ///< \todo Do we need this?
    Grantlee::OutputStream outputStream(&textStream);
    tmplate->render(&outputStream, &context);
    if (tmplate->error()) {
        qWarning() << QTR("Failed to render: %1 - %2").arg(outputPath, tmplate->errorString());
        return false;
    }
    filesWritten.append(outputPath);
    return true;
}

/// Grantlee output stream that does *no* content escaping.
//class NoEscapeStream : public Grantlee::OutputStream {
//public:
//    explicit NoEscapeStream(QTextStream * stream) : Grantlee::OutputStream(stream) { }

//    virtual QString escape(const QString &input) const { return input; }

//    virtual QSharedPointer<OutputStream> clone( QTextStream *stream ) const {
//        return QSharedPointer<OutputStream>(new NoEscapeStream(stream));
//    }
//};

} // namespace doxlee
