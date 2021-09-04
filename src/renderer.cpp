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

Renderer::Renderer(QDir inputDir, QDir templatesDir, const QDir &outputDir, const ClobberMode clobber)
    : inputDir(inputDir), templatesDir(templatesDir), outputDir(outputDir), clobber(clobber)
{
    Q_ASSERT(inputDir.exists() && inputDir.isReadable());
    Q_ASSERT(templatesDir.exists() && templatesDir.isReadable());
    Q_ASSERT(outputDir.exists());

    auto loader = QSharedPointer<Grantlee::FileSystemTemplateLoader>::create();
    loader->setTemplateDirs(QStringList() << QSL(":/templates"));
    engine.addTemplateLoader(loader);
    engine.setSmartTrimEnabled(true);

    /// \todo We probably don't need to iteratively load these up-front.
//    templatesDir.setFilter(QDir::Files|QDir::Readable);
//    QDirIterator dir(templatesDir, QDirIterator::Subdirectories);
//    while (dir.hasNext()) {
//        const QString name = dir.next().mid(dir.path().size()+1);
//        qInfo().noquote() << QTR("Loading template: %1").arg(name);
//        const auto tmplate = engine.loadByName(name);
//        if (tmplate->error()) {
//            qWarning().noquote() << QTR("Error loading template: %1 - %2").arg(name, tmplate->errorString());
//            continue;
//        }
//        //templates.insert(name, tmplate);
//    }
}

bool Renderer::render()
{
    Grantlee::Context context;
    context.insert(QSL("doxleeVersion"), QStringLiteral(CMAKE_PROJECT_VERSION));

    if (!parseIndexXml(inputDir.absoluteFilePath(QSL("index.xml")), context)) {
            return false;
    }


    // Parse index.xml

    /// \todo
    Q_UNUSED(clobber);

    return true;
}

int Renderer::outputFileCount() const
{
    return 0; ///< \todo
}

bool Renderer::parseIndexXml(const QString &fileName, Grantlee::Context &context)
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
        qWarning().noquote() << QTR("File is not a Doxygen XML index: %1 - %2").arg(fileName, xml.name());
        return false;
    }
    context.insert(QSL("doxygenVersion"), xml.attributes().value(QSL("version")).toString());
    context.insert(QSL("doxygenLanguage"), xml.attributes().value(QSL("xml:lang")).toString());

    // Parse the contained <compound> elements.
    while ((!xml.atEnd()) && (xml.readNextStartElement())) {
        if (xml.name() == QSL("compound")) {
            // parse compound.
            qDebug() << xml.name();
        } else {
            qWarning().noquote() << QTR("Skipping unknown element: <%1>").arg(xml.name());
            xml.skipCurrentElement();
        }
    }
    return false; ///< \todo
}


/// Grantlee output stream that does *no* content escaping.
class NoEscapeStream : public Grantlee::OutputStream {
public:
    explicit NoEscapeStream(QTextStream * stream) : Grantlee::OutputStream(stream) { }

    virtual QString escape(const QString &input) const { return input; }

    virtual QSharedPointer<OutputStream> clone( QTextStream *stream ) const {
        return QSharedPointer<OutputStream>(new NoEscapeStream(stream));
    }
};
