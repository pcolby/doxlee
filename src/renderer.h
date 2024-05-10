// SPDX-FileCopyrightText: 2021-2024 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

#if defined USE_CUTELEE
#include <cutelee/engine.h>
#include <cutelee/context.h>
#elif defined USE_GRANTLEE
#include <grantlee/engine.h>
#include <grantlee/context.h>
#endif

#include "textlee.h"

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
                const QDir &outputDir, Textlee::Context &context, ClobberMode &clobberMode);

    bool render(const QString &templateName, const QString &outputPath,
                Textlee::Context &context, ClobberMode &clobberMode);

private:
    const QDir inputDir;
    Textlee::Context context;
    Textlee::Engine engine;
    QStringList indexTemplateNames;
    QList<QPair<QString, QString>> staticFileNames;
    QMultiHash<QString, QString> templateNamesByKind;
    QStringList filesWritten;

};

} // namespace doxlee
