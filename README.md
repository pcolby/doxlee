# doxlee

[![Build Status](https://github.com/pcolby/doxlee/actions/workflows/build.yaml/badge.svg)](https://github.com/pcolby/doxlee/actions/workflows/build.yaml)
[![Static Analysis](https://github.com/pcolby/doxlee/actions/workflows/static.yaml/badge.svg)](https://github.com/pcolby/doxlee/actions/workflows/static.yaml)
[![Coverage Status](https://img.shields.io/coveralls/github/pcolby/doxlee?label=Coverage)](https://coveralls.io/github/pcolby/doxlee)
[![GitHub Release](https://img.shields.io/github/v/release/pcolby/doxlee?include_prereleases&label=Release)](https://github.com/pcolby/doxlee/releases/latest)
[![GPLv3 License](https://img.shields.io/badge/License-GPLv3-informational.svg)](https://www.gnu.org/licenses/gpl-3.0.html)

Doxlee renders source code documentation by applying [Grantlee] templates to [Doxygen]'s XML output.

    ┌───┐            ┌───┐           ┌────┐
    │src├──Doxygen──►│XML├──doxlee──►│HTML│
    └───┘            └───┘    ▲      └────┘
                              │
               ┌─────────┐    │
               │templates├────┘
               └─────────┘

Of course, the output doesn't have to be HTML... just about any text format should be possible.

Doxlee was written specifically to support generating documentation from [Doxygen] in the same (or very
similar) format as [QDoc]'s _online_ format (ie the format used by the official [Qt documentation]). But
anyone can provide their own templates to generate completely different site layouts of their own.

[Doxygen] does support using your own custom headers, footers, and style sheets, which can achieve a
fair amount of flexibility already ([Doxygen Awesome] is a great example). But those options do not
allow fundamental restructuring of things like the navigation menus, sidebar and breadcrumbs. So if
[Doxygen]'s built-in options are sufficient for your desired output, then use them. But if you need more
control, then feel free to explore this project :)

## Installation

\todo

For now, see [Building from Source](#building-from-source) below.

## Usage

Update your [Doxygen] configuration file to enable XML output:

```
GENERATE_XML           = YES
XML_OUTPUT             = <path-to-xml> # Optional; defaults to xml.
XML_PROGRAMLISTING     = YES # Optional; set to YES to include source code in the documentation.
XML_NS_MEMB_FILE_SCOPE = YES # Optional; set to YES to include namespace documentation, if any.
```

Generate the documentation:

```sh
doxygen [configname]
doxlee -i <path-to-xml-> -o <path-to-write-output> -t <path-to-theme>
```

There are some other basic options too. Use `--help` to see them.

```
Usage: ./src/doxlee [options]
Render Doxygen XML via Grantlee Templates

Options:
  -i, --input-dir <dir>        Read Doyxgen XML files from dir
  -t, --theme-dir <dir>        Read Grantlee theme from dir
  -o, --output-dir <dir>       Write output files to dir
  -d, --debug                  Enable debug output
  --color <yes|no|auto>        Color the console output (default auto)
  --overwrite <yes|no|prompt>  Overwrite existing files (default prompt)
  -f, --force                  Same as --overwrite=yes but also skip initial
                               prompt
  -h, --help                   Displays help on commandline options.
  --help-all                   Displays help including Qt specific options.
  -v, --version                Displays version information.

```

## Building from Source

Just follow a typical [CMake]-based out-of-source build process:

~~~.sh
cmake -E make_directory <tmp-build-dir>
cmake -D CMAKE_BUILD_TYPE=Release -S <path-to-cloned-repo> -B <tmp-build-dir>
cmake --build <tmp-build-dir>
ctest --test-dir <tmp-build-dir> --verbose
~~~

[CMake]:            https://cmake.org/ "CMake"
[Doxygen]:          https://www.doxygen.nl/ "Doxygen"
[Doxygen Awesome]:  https://jothepro.github.io/doxygen-awesome-css/ "Doxygen Awesome"
[Grantlee]:         https://github.com/steveire/grantlee "The Grantlee Libraries"
[QDoc]:             https://doc.qt.io/qt-6/01-qdoc-manual.html "Introduction to QDoc"
[Qt documentation]: https://doc.qt.io/qt-6/index.html "Qt Documentation"
