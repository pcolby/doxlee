// SPDX-FileCopyrightText: 2021-2024 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "testrenderer.h"

#define protected public
#define private public

#include "renderer.h"

/// Shorten the QStringLiteral macro for readability.
#define QSL(str) QStringLiteral(str)

#define DATA_TAG QString::fromUtf8(QTest::currentDataTag())

void TestRenderer::compoundPathName_data()
{
    QTest::addColumn<QVariantMap>("compound");
    QTest::addColumn<QString>("pathName");

    QVariantMap compound;
    compound.insert(QSL("kind"),  QSL("class"));
    compound.insert(QSL("name"),  QSL("Foo"));
    compound.insert(QSL("refid"), QSL("barBaz"));

    // Filenames of <kind>[.ext] only, are treated the same as <kind>-refid[.txt]
    QTest::addRow("class.txt")       << compound << QSL("barBaz.txt");

    // Basic substitutions.
    QTest::addRow("class-kind.txt")            << compound << QSL("class.txt");
    QTest::addRow("class-name.txt")            << compound << QSL("Foo.txt");
    QTest::addRow("class-refid.txt")           << compound << QSL("barBaz.txt");

    // Separator removal is dependant on the <kind><sep> separator character.
    QTest::addRow("class+refid+ABC.txt")           << compound << QSL("barBazABC.txt");
    QTest::addRow("class+refid-ABC.txt")           << compound << QSL("barBaz-ABC.txt");
    QTest::addRow("class+refid_ABC.txt")           << compound << QSL("barBaz_ABC.txt");
    QTest::addRow("class-refid+ABC.txt")           << compound << QSL("barBaz+ABC.txt");
    QTest::addRow("class-refid-ABC.txt")           << compound << QSL("barBazABC.txt");
    QTest::addRow("class-refid_ABC.txt")           << compound << QSL("barBaz_ABC.txt");
    QTest::addRow("class_refid+ABC.txt")           << compound << QSL("barBaz+ABC.txt");
    QTest::addRow("class_refid-ABC.txt")           << compound << QSL("barBaz-ABC.txt");
    QTest::addRow("class_refid_ABC.txt")           << compound << QSL("barBazABC.txt");
    QTest::addRow("class:refid+ABC.txt")           << compound << QSL("barBaz+ABC.txt");
    QTest::addRow("class:refid-ABC.txt")           << compound << QSL("barBaz-ABC.txt");
    QTest::addRow("class:refid_ABC.txt")           << compound << QSL("barBaz_ABC.txt");

    // Can use multiple replacement tokens, and in any order.
    QTest::addRow("class+kind-name-refid.txt") << compound << QSL("class-Foo-barBaz.txt");
    QTest::addRow("class-name-refid-kind.txt") << compound << QSL("FoobarBazclass.txt");
    QTest::addRow("class_refid_kind_name.txt") << compound << QSL("barBazclassFoo.txt");

    // Path <kind> prefix applies to filenames, not ancestors directory names.
    QTest::addRow("kind/name/refid/class.txt") << compound << QSL("class/Foo/barBaz/barBaz.txt");
    QTest::addRow("name/refid/kind/class.txt") << compound << QSL("Foo/barBaz/class/barBaz.txt");
    QTest::addRow("refid/kind/name/class.txt") << compound << QSL("barBaz/class/Foo/barBaz.txt");
    QTest::addRow("abc-refid/class-.html")     << compound << QSL("abcbarBaz/barBaz.html");
    QTest::addRow("abc_refid/class-.html")     << compound << QSL("abc_barBaz/barBaz.html");
    QTest::addRow("refid-abc/class+A.html")    << compound << QSL("barBaz-abc/A.html");
    QTest::addRow("abc-refid-def/class.html")  << compound << QSL("abc-barBaz-def/barBaz.html");

    // Support file files named that same as their kind.
    QTest::addRow("refid/class-class")         << compound << QSL("barBaz/class");
    QTest::addRow("refid/class-class.html")    << compound << QSL("barBaz/class.html");
}

void TestRenderer::compoundPathName()
{
    QFETCH(QVariantMap, compound);
    QTEST(doxlee::Renderer::compoundPathName(compound, DATA_TAG), "pathName");
}

void TestRenderer::getKindFromFileName_data()
{
    QTest::addColumn<QString>("kind");
    QTest::addRow(" ")                    << QString();
    QTest::addRow("/")                    << QString();
    QTest::addRow(".abc")                 << QString();
    QTest::addRow("kind1")                << QSL("kind1");
    QTest::addRow("kind2-foo")            << QSL("kind2");
    QTest::addRow("kind3.html")           << QSL("kind3");
    QTest::addRow("kind4-foo.html")       << QSL("kind4");
    QTest::addRow("some/path/kind5")      << QSL("kind5");
    QTest::addRow("some/path/kind6.html") << QSL("kind6");
}

void TestRenderer::getKindFromFileName()
{
    QTEST(doxlee::Renderer::getKindFromFileName(DATA_TAG), "kind");
}


QTEST_MAIN(TestRenderer)
