# doxlee

Doxlee renders source code documentation by applying [Grantlee] templates to [Doxygen]'s XML output.

    ┌───┐            ┌───┐           ┌────┐
    │src├──Doxygen──►│XML├──doxlee──►│HTML│
    └───┘            └───┘    ▲      └────┘
                              │
               ┌─────────┐    │
               │templates├────┘
               └─────────┘

Note, the output doesn't have to be HTML... just about any text format should be possible.

Doxlee was written specifically to support generating documentation from [Doxygen] in the same (or very
similar) format as [QDoc]'s _online_ format (ie the format used by the official [Qt documentation]). But
anyone can provide their own templates to generate completely different site layouts of their own.

Note, [Doxygen] does support using your own custom headers, footers, and style sheets, which can achieve
a fair amount of flexibility already ([Doxygen Awesome] is a great example). But those options do not
allow fundamental restructuring things like the navigation menus, sidebar and breadcrumbs. So if
[Doxygen]'s built-in options are sufficient for your desired output, then use them. But if you want more
control (as I did), then feel free to explore this project :)

## Installation

\todo

For now, see **Building from Source**.

## Use

\todo

Something like:

Set the following options in your [Doxygen] configuration:

```
GENERATE_XML           = YES
XML_OUTPUT             = <path-to-xml>
XML_PROGRAMLISTING     = YES # \todo Do we recomment/require this?
XML_NS_MEMB_FILE_SCOPE = NO  # \todo Do we recommend/require/forbid this?
```

Generate the documentation:

```sh
doxygen [configname]
doxlee --input-dir <path-to-XML-> --output-dir <path-to-html> --templates-dir <path-to-templates>
```

## [Building from Source](build-ing-fromsosurce)

\todo

[QDoc]:             https://doc.qt.io/qt-6/01-qdoc-manual.html "Introduction to QDoc"
[Doxygen]:          https://www.doxygen.nl/ "Doxygen"
[Doxygen Awesome]:  https://jothepro.github.io/doxygen-awesome-css/ "Doxygen Awesome"
[Grantlee]:         https://github.com/steveire/grantlee "The Grantlee Libraries"
[Qt documentation]: https://doc.qt.io/qt-6/index.html "Qt Documentation"
