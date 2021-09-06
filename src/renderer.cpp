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

#include "renderer.h"

#include <grantlee/templateloader.h>
#include <QCoreApplication>
#include <QDebug>
#include <QDirIterator>
#include <QXmlStreamReader>

/// Shorten the QStringLiteral macro for readability.
#define QSL(str) QStringLiteral(str)

/// Shorten QCoreApplication::translate calls for readability.
#define QTR(str) QCoreApplication::translate("Renderer", str)

Renderer::Renderer(const QString &inputDir, const QString &outputDir, const ClobberMode clobber)
    : inputDir(inputDir), outputDir(outputDir), clobber(clobber)
{
    // Configure the Grantlee Templates rendering engine.
    engine.setSmartTrimEnabled(true);
}

#include <QRegularExpression>

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
    engine.addTemplateLoader(loader);

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
            //staticFileNames.append(dir.filePath());
            staticFileNames.append({dir.filePath(), relativePathName});
            continue;
        }

        // Extract the 'kind' string, if any. Equivalent to ^([^-.]+) on the file name only.
        const QString kind =
            dir.fileName().section(QLatin1Char('-'),0,0).section(QLatin1Char('.'),0,0);
        qDebug().noquote() << QTR("Inspecting template: %1 (%2,%3)")
            .arg(dir.filePath(), relativePathName, kind);

        // If the 'kind' is static, just record it for copying to the output later.
        if (kind == QSL("static")) {
            staticFileNames.append({dir.filePath(), relativePathName});
            continue;
        }

        // Load the template, and store against the relevant 'kind'.
        if ((kind == QSL("index")) || (kinds.first.contains(kind)) || (kinds.second.contains(kind))) {
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

bool Renderer::render()
{
    Grantlee::Context context;
    context.insert(QSL("doxleeVersion"), QStringLiteral(CMAKE_PROJECT_VERSION));

    // Parse the XML index.
    if (!parseIndex(inputDir.absoluteFilePath(QSL("index.xml")), context)) return false;

    // Supplement the compounds index data (from parseIndex) with additional views of the same data.
    if (!supplementIndexes(context)) return false;

    // Render all templates to output files.
    return renderAll(context);
}

int Renderer::outputFileCount() const
{
    return 0; ///< \todo
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
    qDebug() << "Schema specifies" << compoundKinds.size() << "compound kind(s), and"
                                   << memberKinds.size() << "member kind(s)";
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
                    qWarning().noquote() << QTR("xSkipping unknown <%1> element at %2:%3:%4")
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

QVariantMap toVariantMap(const QHash<QString,QVariantMap> &hash)
{
    QVariantMap map;
    for (auto iter = hash.begin(); iter != hash.end(); ++iter) {
        map.insert(iter.key(), iter.value());
    }
    return map;
}

bool Renderer::supplementIndexes(Grantlee::Context &context)
{
    const QVariantList compounds = context.lookup(QSL("compoundsList")).toList();
    QHash<QString,QVariantMap> compoundsByKind, membersByKind;
    QVariantMap compoundsByRefId, membersByRefId;
    for (const QVariant &compound: compounds) {
        const QVariantMap compoundMap = compound.toMap();
        {
            const QString kind = compoundMap.value(QSL("kind")).toString();
            const QString name = compoundMap.value(QSL("name")).toString();
            const QString refid = compoundMap.value(QSL("refid")).toString();
            //qDebug() << "compound" << kind << name << refid;
            compoundsByKind[kind].insert(name, compound);
            compoundsByRefId.insert(refid, compound);
        }

        const QVariantList members = compoundMap.value(QSL("members")).toList();
        for (const QVariant &member: members) {
            const QVariantMap memberMap = member.toMap();
            const QString kind = memberMap.value(QSL("kind")).toString();
            const QString name = memberMap.value(QSL("name")).toString();
            const QString refid = memberMap.value(QSL("refid")).toString();
            //qDebug() << "member" << kind << name << refid;
            membersByKind[kind].insert(name, member);
            membersByRefId.insert(refid, member);
        }
    }
    context.insert(QSL("compoundsByKind" ), toVariantMap(compoundsByKind));
    context.insert(QSL("compoundsByRefId"), compoundsByRefId);
    context.insert(QSL("membersByKind"   ), toVariantMap(membersByKind));
    context.insert(QSL("membersByRefId"  ), membersByRefId);
    return true;
}

bool Renderer::renderAll(Grantlee::Context &context)
{
    // Render all index templates.

    // Render all "lind" templates:
    // For each <kind>, if we have templates, then
    //   for each input XML file with matching kind
    //     render each XML file for each <kind> template.

    // Copy all static files.

    Q_UNUSED(clobber);
    Q_UNUSED(context);
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
