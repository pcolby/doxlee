// SPDX-FileCopyrightText: 2021-2024 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "renderer.h"

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
        messagePattern.prepend(QStringLiteral("%{time process} %{type} %{function} "));
        QLoggingCategory::defaultCategory()->setEnabled(QtDebugMsg, true);
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
    parser.setApplicationDescription(QStringLiteral("Render Doxygen XML via Grantlee Templates"));
    parser.addOptions({
        {{QStringLiteral("i"), QStringLiteral("input-dir")},
          QCoreApplication::translate("main", "Read Doyxgen XML files from dir"),
          QStringLiteral("dir")},
        {{QStringLiteral("t"), QStringLiteral("theme-dir")},
          QCoreApplication::translate("main", "Read Grantlee theme from dir"),
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

    // Check for any missing (but required) command line options.
    QStringList missingOptions {
        QLatin1String("input-dir"),
        QLatin1String("output-dir"),
        QLatin1String("theme-dir"),
    };
    for (auto iter = missingOptions.begin(); iter != missingOptions.end();) {
        if (parser.isSet(*iter)) iter=missingOptions.erase(iter); else ++iter;
    }
    if (!missingOptions.empty()) {
        qWarning().noquote() << QCoreApplication::translate("main", "Missing required option(s): %1")
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
            qWarning().noquote() << QCoreApplication::translate("main",
                "Invalid argument to option --overwrite: %1").arg(overwrite);
            return 2;
        }
    }

    // Verify that the directories exist.
    const QFileInfo inputDir (QDir::cleanPath(parser.value(QStringLiteral("input-dir"))));
    const QFileInfo themeDir (QDir::cleanPath(parser.value(QStringLiteral("theme-dir"))));
    const QFileInfo outputDir(QDir::cleanPath(parser.value(QStringLiteral("output-dir"))));

    if ((!inputDir.exists()) || (!inputDir.isDir()) || (!inputDir.isReadable())) {
        qWarning().noquote() << QCoreApplication::translate("main",
            "Input directory does not exist, is not a directory, or is not readable: %1")
            .arg(inputDir.absoluteFilePath());
        return 2;
    }
    if ((!themeDir.exists()) || (!themeDir.isDir()) || (!themeDir.isReadable())) {
        qWarning().noquote() << QCoreApplication::translate("main",
            "Theme directory does not exist, is not a directory, or is not readable: %1")
            .arg(themeDir.absoluteFilePath());
        return 2;
    }
    if ((!outputDir.exists()) || (!outputDir.isDir()) || (!outputDir.isWritable())) {
        qWarning().noquote() << QCoreApplication::translate("main",
            "Output directory does not exist, is not a directory, or is not writable: %1")
            .arg(outputDir.absoluteFilePath());
        return 2;
    }

    // Setup the renderer.
    doxlee::Renderer renderer(inputDir.absoluteFilePath());
    if (!renderer.loadTemplates(themeDir.absoluteFilePath())) {
        return 3;
    }

    // Let the user know we're about to generate a lot of files, then do it!
    qWarning().noquote() << QCoreApplication::translate("main",
        "About to generate approximately %1 file(s) in: %2")
        .arg(renderer.expectedFileCount()).arg(outputDir.absoluteFilePath());
    if (!parser.isSet(QStringLiteral("force"))) {
        qInfo().noquote() << QCoreApplication::translate("main", "Press Enter to contine");
        QTextStream stream(stdin);
        stream.readLine();
    }
    if (!renderer.render(outputDir.absoluteFilePath(), clobberMode)) {
        return 4;
    }
    qInfo().noquote() << QCoreApplication::translate("main",
        "Rendered %1 file(s) in %2").arg(renderer.outputFileCount()).arg(outputDir.absoluteFilePath());
    return 0;
}
