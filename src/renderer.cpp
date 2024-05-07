// SPDX-FileCopyrightText: 2021-2024 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "renderer.h"

#include "doxml.h"

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
    const QVariantMap map = doxml::parseIndex(this->inputDir);
    for (auto iter = map.constBegin(); iter != map.constEnd(); ++iter)
        context.insert(iter.key(), iter.value());
}

bool Renderer::loadTemplates(const QString &templatesDir)
{
    // Fetch the list of compound and member kinds supported by the Doxgen version.
    auto kinds = doxml::kinds(inputDir);
    if ((kinds.first.isEmpty()) && (kinds.second.isEmpty())) {
        return false; // doxml::kinds failed; and reported an appropriate error.
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
        const QVariantMap compoundDefinition = doxml::parseCompound(inputDir, refId);

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
