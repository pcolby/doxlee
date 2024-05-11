// SPDX-FileCopyrightText: 2021-2024 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "renderer.h"

#if defined USE_CUTELEE
#include <cutelee/cutelee_version.h>
#elif defined USE_GRANTLEE
#include <grantlee/grantlee_version.h>
#endif

#include <QCommandLineParser>
#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QLoggingCategory>

#if defined(Q_OS_UNIX)
#include <unistd.h>
#elif defined(Q_OS_WIN)
#include <Windows.h>
#endif

static Q_LOGGING_CATEGORY(lc, "doxlee.main", QtInfoMsg);

inline bool haveConsole()
{
    #if defined(Q_OS_UNIX)
    return isatty(STDERR_FILENO);
    #elif defined(Q_OS_WIN)
    return GetConsoleWindow();
    #else
    return false;
    #endif
}

void configureLogging(const QCommandLineParser &parser)
{
    // Start with the Qt default message pattern (see qtbase:::qlogging.cpp:defaultPattern)
    QString messagePattern = QStringLiteral("%{if-category}%{category}: %{endif}%{message}");

    if (parser.isSet(QStringLiteral("debug"))) {
        #ifdef QT_MESSAGELOGCONTEXT
        // %{file}, %{line} and %{function} are only available when QT_MESSAGELOGCONTEXT is set.
        messagePattern.prepend(QStringLiteral("%{function} "));
        #endif
        messagePattern.prepend(QStringLiteral("%{time process} %{threadid} %{type} "));
        QLoggingCategory::setFilterRules(QStringLiteral("doxlee.*.debug=true"));
    }

    const QString color = parser.value(QStringLiteral("color"));
    if ((color == QStringLiteral("yes")) || (color == QStringLiteral("auto") && haveConsole())) {
        messagePattern.prepend(QStringLiteral(
        "%{if-debug}\x1b[37m%{endif}"      // White
        "%{if-info}\x1b[32m%{endif}"       // Green
        "%{if-warning}\x1b[35m%{endif}"    // Magenta
        "%{if-critical}\x1b[31m%{endif}"   // Red
        "%{if-fatal}\x1b[31;1m%{endif}")); // Red and bold
        messagePattern.append(QStringLiteral("\x1b[0m")); // Reset.
    }

    qSetMessagePattern(messagePattern);
}

int main(int argc, char *argv[])
{
    // Setup the core application.
    QCoreApplication app(argc, argv);
    QCoreApplication::setApplicationName(QStringLiteral(PROJECT_NAME));
    QCoreApplication::setApplicationVersion(QString::fromLatin1(PROJECT_VERSION
        #ifdef PROJECT_PRE_RELEASE
        "-" PROJECT_PRE_RELEASE
        #endif
        #ifdef PROJECT_BUILD_ID
        "+" PROJECT_BUILD_ID
        #endif
    ));

    // \todo Install localised translator, if we have translations for the current locale.

    // Parse the command line options.
    QCommandLineParser parser;
    parser.setApplicationDescription(QStringLiteral("Render Doxygen XML via text templates"));
    parser.addOptions({
        {{QStringLiteral("i"), QStringLiteral("input-dir")},
          QCoreApplication::translate("main", "Read Doyxgen XML files from dir"),
          QStringLiteral("dir")},
        {{QStringLiteral("t"), QStringLiteral("templates-dir")},
          QCoreApplication::translate("main", "Read text templates from dir"),
          QStringLiteral("dir")},
        {{QStringLiteral("o"), QStringLiteral("output-dir")},
          QCoreApplication::translate("main", "Write output files to dir"), QStringLiteral("dir")},
        {{QStringLiteral("d"), QStringLiteral("debug")}, QStringLiteral("Enable debug output")},
        { QStringLiteral("color"),
          QCoreApplication::translate("main","Color the console output (default auto)"),
          QStringLiteral("yes|no|auto"), QStringLiteral("auto")},
        { QStringLiteral("overwrite"),
          QCoreApplication::translate("main","Overwrite existing files (default prompt)"),
          QStringLiteral("yes|no|prompt"), QStringLiteral("prompt")},
        {{QStringLiteral("f"), QStringLiteral("force")},
          QCoreApplication::translate("main", "Same as --overwrite=yes but also skip initial prompt")},
    });
    parser.addHelpOption();
    parser.addVersionOption();
    parser.process(app);
    configureLogging(parser);
    qCDebug(lc).noquote() << QCoreApplication::applicationName() << QCoreApplication::applicationVersion();
    qCDebug(lc).noquote() << "Qt " QT_VERSION_STR " compile-time";
    qCDebug(lc).noquote() << "Qt" << qVersion() << "runtime";
    #ifdef CUTELEE_VERSION_STRING
    qCDebug(lc).noquote() << "Cutelee " CUTELEE_VERSION_STRING " runtime";
    #endif
    #ifdef GRANTLEE_VERSION_STRING
    qCDebug(lc).noquote() << "Grantlee " GRANTLEE_VERSION_STRING " runtime";
    #endif

    // Check for any missing (but required) command line options.
    QStringList missingOptions {
        QLatin1String("input-dir"),
        QLatin1String("output-dir"),
        QLatin1String("templates-dir"),
    };
    #if QT_VERSION >= QT_VERSION_CHECK(6, 1, 0) // QList::removeIf() added in Qt 6.1.
    missingOptions.removeIf([&parser](const QString &option){ return parser.isSet(option); });
    #else
    for (auto iter = missingOptions.begin(); iter != missingOptions.end();) {
        if (parser.isSet(*iter)) iter=missingOptions.erase(iter); else ++iter;
    }
    #endif
    if (!missingOptions.empty()) {
        qCWarning(lc).noquote() << QCoreApplication::translate("main", "Missing required option(s): %1")
            .arg(missingOptions.join(QLatin1Char(' ')));
        return 2;
    }

    // Translate the --overwrite option's value (if any) to a Renderer::ClobberMode value.
    doxlee::Renderer::ClobberMode clobberMode = doxlee::Renderer::Prompt;
    if (parser.isSet(QStringLiteral("force"))) {
        clobberMode = doxlee::Renderer::Overwrite;
    } else {
        const QString overwrite = parser.value(QStringLiteral("overwrite"));
        if      (overwrite == QStringLiteral("yes"))    clobberMode = doxlee::Renderer::Overwrite;
        else if (overwrite == QStringLiteral("no"))     clobberMode = doxlee::Renderer::Skip;
        else if (overwrite == QStringLiteral("prompt")) clobberMode = doxlee::Renderer::Prompt;
        else {
            qCWarning(lc).noquote() << QCoreApplication::translate("main",
                "Invalid argument to option --overwrite: %1").arg(overwrite);
            return 2;
        }
    }

    // Verify that the directories exist.
    const QFileInfo inputDir (QDir::cleanPath(parser.value(QStringLiteral("input-dir"))));
    const QFileInfo templatesDir (QDir::cleanPath(parser.value(QStringLiteral("templates-dir"))));
    const QFileInfo outputDir(QDir::cleanPath(parser.value(QStringLiteral("output-dir"))));

    if ((!inputDir.exists()) || (!inputDir.isDir()) || (!inputDir.isReadable())) {
        qCWarning(lc).noquote() << QCoreApplication::translate("main",
            "Input directory does not exist, is not a directory, or is not readable: %1")
            .arg(inputDir.absoluteFilePath());
        return 2;
    }
    if ((!templatesDir.exists()) || (!templatesDir.isDir()) || (!templatesDir.isReadable())) {
        qCWarning(lc).noquote() << QCoreApplication::translate("main",
            "Templates directory does not exist, is not a directory, or is not readable: %1")
            .arg(templatesDir.absoluteFilePath());
        return 2;
    }
    if ((!outputDir.exists()) || (!outputDir.isDir()) || (!outputDir.isWritable())) {
        qCWarning(lc).noquote() << QCoreApplication::translate("main",
            "Output directory does not exist, is not a directory, or is not writable: %1")
            .arg(outputDir.absoluteFilePath());
        return 2;
    }

    // Setup the renderer.
    doxlee::Renderer renderer(inputDir.absoluteFilePath());
    if (!renderer.loadTemplates(templatesDir.absoluteFilePath())) {
        return 3;
    }

    // Let the user know we're about to generate a lot of files, then do it!
    qCWarning(lc).noquote() << QCoreApplication::translate("main",
        "About to generate approximately %1 file(s) in: %2")
        .arg(renderer.expectedFileCount()).arg(outputDir.absoluteFilePath());
    if (!parser.isSet(QStringLiteral("force"))) {
        qCInfo(lc).noquote() << QCoreApplication::translate("main", "Press Enter to contine");
        QTextStream stream(stdin);
        stream.readLine();
    }
    if (!renderer.render(outputDir.absoluteFilePath(), clobberMode)) {
        return 4;
    }
    qCInfo(lc).noquote() << QCoreApplication::translate("main",
        "Rendered %1 file(s) in %2").arg(renderer.outputFileCount()).arg(outputDir.absoluteFilePath());
    return 0;
}
