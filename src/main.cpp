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
    QCoreApplication app(argc, argv);
    app.setApplicationName(QStringLiteral(CMAKE_PROJECT_NAME));
    app.setApplicationVersion(QStringLiteral(CMAKE_PROJECT_VERSION));

    // Parse the command line options.
    QCommandLineParser parser;
    parser.setApplicationDescription(QStringLiteral("Render Doxygen XML via Grantlee Templates"));
    parser.addOptions({
        {{QStringLiteral("i"), QStringLiteral("input-dir")},
          QCoreApplication::translate("main", "Read Doyxgen XML files from dir"),
          QStringLiteral("dir")},
        {{QStringLiteral("t"), QStringLiteral("templates-dir")},
          QCoreApplication::translate("main", "Read Grantlee templates from dir"),
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
        QLatin1String("templates-dir"),
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
    Renderer::ClobberMode clobberMode = Renderer::Prompt;
    if (parser.isSet(QStringLiteral("force"))) {
        clobberMode = Renderer::Overwrite;
    } else {
        const QString overwrite = parser.value(QStringLiteral("overwrite"));
        if      (overwrite == QStringLiteral("yes"))    clobberMode = Renderer::Overwrite;
        else if (overwrite == QStringLiteral("no"))     clobberMode = Renderer::Skip;
        else if (overwrite == QStringLiteral("prompt")) clobberMode = Renderer::Prompt;
        else {
            qWarning().noquote() << QCoreApplication::translate("main",
                "Invalid argument to option --overwrite: %1").arg(overwrite);
            return 2;
        }
    }

    // Verify that the directories exist.
    const QFileInfo inputDir    (QDir::cleanPath(parser.value(QStringLiteral("input-dir"))));
    const QFileInfo templatesDir(QDir::cleanPath(parser.value(QStringLiteral("templates-dir"))));
    const QFileInfo outputDir   (QDir::cleanPath(parser.value(QStringLiteral("output-dir"))));

    if ((!inputDir.exists()) || (!inputDir.isDir()) || (!inputDir.isReadable())) {
        qWarning().noquote() << QCoreApplication::translate("main",
            "Input directory does not exist, is not a directory, or is not readable: %1")
            .arg(inputDir.absoluteFilePath());
        return 2;
    }
    if ((!templatesDir.exists()) || (!templatesDir.isDir()) || (!templatesDir.isReadable())) {
        qWarning().noquote() << QCoreApplication::translate("main",
            "Templates directory does not exist, is not a directory, or is not readable: %1")
            .arg(templatesDir.absoluteFilePath());
        return 2;
    }
    if ((!outputDir.exists()) || (!outputDir.isDir()) || (!outputDir.isWritable())) {
        qWarning().noquote() << QCoreApplication::translate("main",
            "Output directory does not exist, is not a directory, or is not writable: %1")
            .arg(outputDir.absoluteFilePath());
        return 2;
    }

    // Setup the renderer.
    Renderer renderer(inputDir.absoluteFilePath());
    if (!renderer.loadTemplates(templatesDir.absoluteFilePath())) {
        return 3;
    }

    // Let the user know we're about to generate a lot of files, then do it!
    if (!parser.isSet(QStringLiteral("force"))) {
        qWarning().noquote() << QCoreApplication::translate("main",
            "About to generate approximately %1 file(s) in: %2")
            .arg(renderer.expectedFileCount()).arg(outputDir.absoluteFilePath());
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
