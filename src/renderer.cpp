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

#include <QCoreApplication>
#include <QDebug>
#include <QDirIterator>

#include <grantlee/templateloader.h>

#define QSL(str) QStringLiteral(str) // Shorten the QStringLiteral macro for readability.

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

    templatesDir.setFilter(QDir::Files|QDir::Readable);
    QDirIterator dir(templatesDir, QDirIterator::Subdirectories);
    while (dir.hasNext()) {
        const QString name = dir.next().mid(dir.path().size()+1);
        qInfo().noquote() << QCoreApplication::translate("Renderer", "Loading template: %1").arg(name);
        const auto tmplate = engine.loadByName(name);
        if (tmplate->error()) {
            qWarning().noquote() << QCoreApplication::translate("Renderer",
                "Error loading template: %1 - %2").arg(name, tmplate->errorString());
            continue;
        }
        //templates.insert(name, tmplate);
    }
}

bool Renderer::render()
{
    Grantlee::Context context;
    context.insert(QSL("doxleeVersion"), QStringLiteral(CMAKE_PROJECT_VERSION));

    /// \todo

    return true;
}

int Renderer::outputFileCount() const
{
    return 0; ///< \todo
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
