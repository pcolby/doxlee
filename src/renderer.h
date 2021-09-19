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
#include <grantlee/context.h>
#include <QDir>
#include <QMultiHash>
#include <QVariantList>
#include <QVariantMap>
#include <QXmlStreamReader>

namespace doxlee {

class Renderer {

public:
    enum ClobberMode {
        Prompt,
        Overwrite,
        Skip,
    };

    explicit Renderer(const QString &inputDir);

    bool loadTemplates(const QString &templatesDir);
    int expectedFileCount() const;
    bool render(const QDir &outputDir, ClobberMode clobberMode);
    int outputFileCount() const;

protected:
    static QString compoundPathName(const QVariantMap &compound, const QString &templateName);
    static QString getKindFromFileName(const QString &fileName);
    static bool promptToOverwrite(const QString &pathName, ClobberMode &clobberMode);

    bool copy(const QString &fromPath, const QString &toPath, ClobberMode &clobberMode);

    bool render(const QVariantList &compounds, const QStringList &templateNames,
                const QDir &outputDir, Grantlee::Context &context, ClobberMode &clobberMode);

    bool render(const QString &templateName, const QString &outputPath,
                Grantlee::Context &context, ClobberMode &clobberMode);

private:
    const QDir inputDir;
    Grantlee::Context context;
    Grantlee::Engine engine;
    QStringList indexTemplateNames;
    QList<QPair<QString, QString>> staticFileNames;
    QMultiHash<QString, QString> templateNamesByKind;
    QStringList filesWritten;

};

} // namespace doxlee
