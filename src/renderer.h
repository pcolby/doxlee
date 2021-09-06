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

#include <grantlee/engine.h>
#include <QDir>
#include <QMultiHash>
#include <QVariantList>

class Renderer {

public:
    enum ClobberMode {
        Prompt,
        Overwrite,
        Skip,
    };

    Renderer(const QString &inputDir, const QString &outputDir, const ClobberMode clobber);

    bool loadTemplates(const QString &templatesDir); // Need's inputDir.
    bool render();
    int outputFileCount() const;

protected:
    static bool supplementIndexes(Grantlee::Context &context);
    static bool parseIndex(const QString &fileName, Grantlee::Context &context);

    bool renderAll(Grantlee::Context &context);

private:
    const QDir inputDir, outputDir;
    const ClobberMode clobber;
    Grantlee::Engine engine;
    QStringList indexTemplateNames;
    QList<QPair<QString, QString>> staticFileNames;
    QMultiHash<QString, QString> templateNamesByKind;

};
