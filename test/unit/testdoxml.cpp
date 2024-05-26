// SPDX-FileCopyrightText: 2021-2024 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "testdoxml.h"

#define protected public
#define private public

#include "doxml.h"

/// Shorten the QStringLiteral macro for readability.
#define QSL(str) QStringLiteral(str)

void TestDoxml::location_data()
{
    QTest::addColumn<QString>("xmlString");
    QTest::addColumn<QString>("expected");

    QTest::addRow("empty") << QString{}                           << QStringLiteral("empty:1:0");
    QTest::addRow("foo")   << QStringLiteral("<foo/>")            << QStringLiteral("foo:1:6");
    QTest::addRow("bar")   << QStringLiteral("<foo><bar/></foo>") << QStringLiteral("bar:1:5");
}

void TestDoxml::location()
{
    QFETCH(QString, xmlString);
    QXmlStreamReader xml(xmlString);
    xml.readNextStartElement();

    doxlee::Doxml doxml(QString{});
    doxml.currentXmlFilePath=QString::fromUtf8(QTest::currentDataTag());
    QTEST(doxml.location(xml), "expected");
}

void TestDoxml::parseCompound_data()
{
}

void TestDoxml::parseCompound()
{
    /// \todo Implement TestDoxml::parseCompound().
    QXmlStreamReader xml;
    doxlee::Doxml doxml(QString{});
    QCOMPARE(doxml.parseCompound(xml), QVariantMap{});
}

void TestDoxml::parseCompound_DoxygenType_data()
{
}

void TestDoxml::parseCompound_DoxygenType()
{
    /// \todo Implement TestDoxml::parseCompound_DoxygenType().
    QXmlStreamReader xml;
    doxlee::Doxml doxml(QString{});
    QCOMPARE(doxml.parseCompound_DoxygenType(xml), QVariantMap{});
}

void TestDoxml::parseCompound_compounddefType_data()
{
}

void TestDoxml::parseCompound_compounddefType()
{
    /// \todo Implement TestDoxml::parseCompound_compounddefType().
    QXmlStreamReader xml;
    doxlee::Doxml doxml(QString{});
    QCOMPARE(doxml.parseCompound_compounddefType(xml), QVariantMap{});
}

void TestDoxml::parseCompound_listofallmembersType_data()
{
}

void TestDoxml::parseCompound_listofallmembersType()
{
    /// \todo Implement TestDoxml::parseCompound_listofallmembersType().
    QXmlStreamReader xml;
    doxlee::Doxml doxml(QString{});
    QCOMPARE(doxml.parseCompound_listofallmembersType(xml), QVariantMap{});
}

void TestDoxml::parseCompound_memberRefType_data()
{
}

void TestDoxml::parseCompound_memberRefType()
{
    /// \todo Implement TestDoxml::parseCompound_memberRefType().
    QXmlStreamReader xml;
    doxlee::Doxml doxml(QString{});
    QCOMPARE(doxml.parseCompound_memberRefType(xml), QVariantMap{});
}

void TestDoxml::parseCompound_docHtmlOnlyType_data()
{
}

void TestDoxml::parseCompound_docHtmlOnlyType()
{
    /// \todo Implement TestDoxml::parseCompound_docHtmlOnlyType().
    QXmlStreamReader xml;
    doxlee::Doxml doxml(QString{});
    QCOMPARE(doxml.parseCompound_docHtmlOnlyType(xml), QVariantMap{});
}

void TestDoxml::parseCompound_compoundRefType_data()
{
}

void TestDoxml::parseCompound_compoundRefType()
{
    /// \todo Implement TestDoxml::parseCompound_compoundRefType().
    QXmlStreamReader xml;
    doxlee::Doxml doxml(QString{});
    QCOMPARE(doxml.parseCompound_compoundRefType(xml), QVariantMap{});
}

void TestDoxml::parseCompound_reimplementType_data()
{
}

void TestDoxml::parseCompound_reimplementType()
{
    /// \todo Implement TestDoxml::parseCompound_reimplementType().
    QXmlStreamReader xml;
    doxlee::Doxml doxml(QString{});
    QCOMPARE(doxml.parseCompound_reimplementType(xml), QVariantMap{});
}

void TestDoxml::parseCompound_incType_data()
{
}

void TestDoxml::parseCompound_incType()
{
    /// \todo Implement TestDoxml::parseCompound_incType().
    QXmlStreamReader xml;
    doxlee::Doxml doxml(QString{});
    QCOMPARE(doxml.parseCompound_incType(xml), QVariantMap{});
}

void TestDoxml::parseCompound_exportsType_data()
{
}

void TestDoxml::parseCompound_exportsType()
{
    /// \todo Implement TestDoxml::parseCompound_exportsType().
    QXmlStreamReader xml;
    doxlee::Doxml doxml(QString{});
    QCOMPARE(doxml.parseCompound_exportsType(xml), QVariantMap{});
}

void TestDoxml::parseCompound_exportType_data()
{
}

void TestDoxml::parseCompound_exportType()
{
    /// \todo Implement TestDoxml::parseCompound_exportType().
    QXmlStreamReader xml;
    doxlee::Doxml doxml(QString{});
    QCOMPARE(doxml.parseCompound_exportType(xml), QVariantMap{});
}

void TestDoxml::parseCompound_refType_data()
{
}

void TestDoxml::parseCompound_refType()
{
    /// \todo Implement TestDoxml::parseCompound_refType().
    QXmlStreamReader xml;
    doxlee::Doxml doxml(QString{});
    QCOMPARE(doxml.parseCompound_refType(xml), QVariantMap{});
}

void TestDoxml::parseCompound_refTextType_data()
{
}

void TestDoxml::parseCompound_refTextType()
{
    /// \todo Implement TestDoxml::parseCompound_refTextType().
    QXmlStreamReader xml;
    doxlee::Doxml doxml(QString{});
    QCOMPARE(doxml.parseCompound_refTextType(xml), QVariantMap{});
}

void TestDoxml::parseCompound_MemberType_data()
{
}

void TestDoxml::parseCompound_MemberType()
{
    /// \todo Implement TestDoxml::parseCompound_MemberType().
    QXmlStreamReader xml;
    doxlee::Doxml doxml(QString{});
    QCOMPARE(doxml.parseCompound_MemberType(xml), QVariantMap{});
}

void TestDoxml::parseCompound_sectiondefType_data()
{
}

void TestDoxml::parseCompound_sectiondefType()
{
    /// \todo Implement TestDoxml::parseCompound_sectiondefType().
    QXmlStreamReader xml;
    doxlee::Doxml doxml(QString{});
    QCOMPARE(doxml.parseCompound_sectiondefType(xml), QVariantMap{});
}

void TestDoxml::parseCompound_memberdefType_data()
{
}

void TestDoxml::parseCompound_memberdefType()
{
    /// \todo Implement TestDoxml::parseCompound_memberdefType().
    QXmlStreamReader xml;
    doxlee::Doxml doxml(QString{});
    QCOMPARE(doxml.parseCompound_memberdefType(xml), QVariantMap{});
}

void TestDoxml::parseCompound_descriptionType_data()
{
}

void TestDoxml::parseCompound_descriptionType()
{
    /// \todo Implement TestDoxml::parseCompound_descriptionType().
    QXmlStreamReader xml;
    doxlee::Doxml doxml(QString{});
    QCOMPARE(doxml.parseCompound_descriptionType(xml), QVariantMap{});
}

void TestDoxml::parseCompound_enumvalueType_data()
{
}

void TestDoxml::parseCompound_enumvalueType()
{
    /// \todo Implement TestDoxml::parseCompound_enumvalueType().
    QXmlStreamReader xml;
    doxlee::Doxml doxml(QString{});
    QCOMPARE(doxml.parseCompound_enumvalueType(xml), QVariantMap{});
}

void TestDoxml::parseCompound_templateparamlistType_data()
{
}

void TestDoxml::parseCompound_templateparamlistType()
{
    /// \todo Implement TestDoxml::parseCompound_templateparamlistType().
    QXmlStreamReader xml;
    doxlee::Doxml doxml(QString{});
    QCOMPARE(doxml.parseCompound_templateparamlistType(xml), QVariantMap{});
}

void TestDoxml::parseCompound_paramType_data()
{
}

void TestDoxml::parseCompound_paramType()
{
    /// \todo Implement TestDoxml::parseCompound_paramType().
    QXmlStreamReader xml;
    doxlee::Doxml doxml(QString{});
    QCOMPARE(doxml.parseCompound_paramType(xml), QVariantMap{});
}

void TestDoxml::parseCompound_linkedTextType_data()
{
}

void TestDoxml::parseCompound_linkedTextType()
{
    /// \todo Implement TestDoxml::parseCompound_linkedTextType().
    QXmlStreamReader xml;
    doxlee::Doxml doxml(QString{});
    QCOMPARE(doxml.parseCompound_linkedTextType(xml), QVariantMap{});
}

void TestDoxml::parseCompound_graphType_data()
{
}

void TestDoxml::parseCompound_graphType()
{
    /// \todo Implement TestDoxml::parseCompound_graphType().
    QXmlStreamReader xml;
    doxlee::Doxml doxml(QString{});
    QCOMPARE(doxml.parseCompound_graphType(xml), QVariantMap{});
}

void TestDoxml::parseCompound_nodeType_data()
{
}

void TestDoxml::parseCompound_nodeType()
{
    /// \todo Implement TestDoxml::parseCompound_nodeType().
    QXmlStreamReader xml;
    doxlee::Doxml doxml(QString{});
    QCOMPARE(doxml.parseCompound_nodeType(xml), QVariantMap{});
}

void TestDoxml::parseCompound_childnodeType_data()
{
}

void TestDoxml::parseCompound_childnodeType()
{
    /// \todo Implement TestDoxml::parseCompound_childnodeType().
    QXmlStreamReader xml;
    doxlee::Doxml doxml(QString{});
    QCOMPARE(doxml.parseCompound_childnodeType(xml), QVariantMap{});
}

void TestDoxml::parseCompound_linkType_data()
{
}

void TestDoxml::parseCompound_linkType()
{
    /// \todo Implement TestDoxml::parseCompound_linkType().
    QXmlStreamReader xml;
    doxlee::Doxml doxml(QString{});
    QCOMPARE(doxml.parseCompound_linkType(xml), QVariantMap{});
}

void TestDoxml::parseCompound_listingType_data()
{
}

void TestDoxml::parseCompound_listingType()
{
    /// \todo Implement TestDoxml::parseCompound_listingType().
    QXmlStreamReader xml;
    doxlee::Doxml doxml(QString{});
    QCOMPARE(doxml.parseCompound_listingType(xml), QVariantMap{});
}

void TestDoxml::parseCompound_codelineType_data()
{
}

void TestDoxml::parseCompound_codelineType()
{
    /// \todo Implement TestDoxml::parseCompound_codelineType().
    QXmlStreamReader xml;
    doxlee::Doxml doxml(QString{});
    QCOMPARE(doxml.parseCompound_codelineType(xml), QVariantMap{});
}

void TestDoxml::parseCompound_highlightType_data()
{
}

void TestDoxml::parseCompound_highlightType()
{
    /// \todo Implement TestDoxml::parseCompound_highlightType().
    QXmlStreamReader xml;
    doxlee::Doxml doxml(QString{});
    QCOMPARE(doxml.parseCompound_highlightType(xml), QVariantMap{});
}

void TestDoxml::parseCompound_spType_data()
{
}

void TestDoxml::parseCompound_spType()
{
    /// \todo Implement TestDoxml::parseCompound_spType().
    QXmlStreamReader xml;
    doxlee::Doxml doxml(QString{});
    QCOMPARE(doxml.parseCompound_spType(xml), QVariantMap{});
}

void TestDoxml::parseCompound_referenceType_data()
{
}

void TestDoxml::parseCompound_referenceType()
{
    /// \todo Implement TestDoxml::parseCompound_referenceType().
    QXmlStreamReader xml;
    doxlee::Doxml doxml(QString{});
    QCOMPARE(doxml.parseCompound_referenceType(xml), QVariantMap{});
}

void TestDoxml::parseCompound_locationType_data()
{
}

void TestDoxml::parseCompound_locationType()
{
    /// \todo Implement TestDoxml::parseCompound_locationType().
    QXmlStreamReader xml;
    doxlee::Doxml doxml(QString{});
    QCOMPARE(doxml.parseCompound_locationType(xml), QVariantMap{});
}

void TestDoxml::parseCompound_docSect1Type_data()
{
}

void TestDoxml::parseCompound_docSect1Type()
{
    /// \todo Implement TestDoxml::parseCompound_docSect1Type().
    QXmlStreamReader xml;
    doxlee::Doxml doxml(QString{});
    QCOMPARE(doxml.parseCompound_docSect1Type(xml), QVariantMap{});
}

void TestDoxml::parseCompound_docSect2Type_data()
{
}

void TestDoxml::parseCompound_docSect2Type()
{
    /// \todo Implement TestDoxml::parseCompound_docSect2Type().
    QXmlStreamReader xml;
    doxlee::Doxml doxml(QString{});
    QCOMPARE(doxml.parseCompound_docSect2Type(xml), QVariantMap{});
}

void TestDoxml::parseCompound_docSect3Type_data()
{
}

void TestDoxml::parseCompound_docSect3Type()
{
    /// \todo Implement TestDoxml::parseCompound_docSect3Type().
    QXmlStreamReader xml;
    doxlee::Doxml doxml(QString{});
    QCOMPARE(doxml.parseCompound_docSect3Type(xml), QVariantMap{});
}

void TestDoxml::parseCompound_docSect4Type_data()
{
}

void TestDoxml::parseCompound_docSect4Type()
{
    /// \todo Implement TestDoxml::parseCompound_docSect4Type().
    QXmlStreamReader xml;
    doxlee::Doxml doxml(QString{});
    QCOMPARE(doxml.parseCompound_docSect4Type(xml), QVariantMap{});
}

void TestDoxml::parseCompound_docInternalType_data()
{
}

void TestDoxml::parseCompound_docInternalType()
{
    /// \todo Implement TestDoxml::parseCompound_docInternalType().
    QXmlStreamReader xml;
    doxlee::Doxml doxml(QString{});
    QCOMPARE(doxml.parseCompound_docInternalType(xml), QVariantMap{});
}

void TestDoxml::parseCompound_docInternalS1Type_data()
{
}

void TestDoxml::parseCompound_docInternalS1Type()
{
    /// \todo Implement TestDoxml::parseCompound_docInternalS1Type().
    QXmlStreamReader xml;
    doxlee::Doxml doxml(QString{});
    QCOMPARE(doxml.parseCompound_docInternalS1Type(xml), QVariantMap{});
}

void TestDoxml::parseCompound_docInternalS2Type_data()
{
}

void TestDoxml::parseCompound_docInternalS2Type()
{
    /// \todo Implement TestDoxml::parseCompound_docInternalS2Type().
    QXmlStreamReader xml;
    doxlee::Doxml doxml(QString{});
    QCOMPARE(doxml.parseCompound_docInternalS2Type(xml), QVariantMap{});
}

void TestDoxml::parseCompound_docInternalS3Type_data()
{
}

void TestDoxml::parseCompound_docInternalS3Type()
{
    /// \todo Implement TestDoxml::parseCompound_docInternalS3Type().
    QXmlStreamReader xml;
    doxlee::Doxml doxml(QString{});
    QCOMPARE(doxml.parseCompound_docInternalS3Type(xml), QVariantMap{});
}

void TestDoxml::parseCompound_docInternalS4Type_data()
{
}

void TestDoxml::parseCompound_docInternalS4Type()
{
    /// \todo Implement TestDoxml::parseCompound_docInternalS4Type().
    QXmlStreamReader xml;
    doxlee::Doxml doxml(QString{});
    QCOMPARE(doxml.parseCompound_docInternalS4Type(xml), QVariantMap{});
}

void TestDoxml::parseCompound_docTitleType_data()
{
}

void TestDoxml::parseCompound_docTitleType()
{
    /// \todo Implement TestDoxml::parseCompound_docTitleType().
    QXmlStreamReader xml;
    doxlee::Doxml doxml(QString{});
    QCOMPARE(doxml.parseCompound_docTitleType(xml), QVariantMap{});
}

void TestDoxml::parseCompound_docSummaryType_data()
{
}

void TestDoxml::parseCompound_docSummaryType()
{
    /// \todo Implement TestDoxml::parseCompound_docSummaryType().
    QXmlStreamReader xml;
    doxlee::Doxml doxml(QString{});
    QCOMPARE(doxml.parseCompound_docSummaryType(xml), QVariantMap{});
}

void TestDoxml::parseCompound_docParaType_data()
{
}

void TestDoxml::parseCompound_docParaType()
{
    /// \todo Implement TestDoxml::parseCompound_docParaType().
    QXmlStreamReader xml;
    doxlee::Doxml doxml(QString{});
    QCOMPARE(doxml.parseCompound_docParaType(xml), QVariantMap{});
}

void TestDoxml::parseCompound_docMarkupType_data()
{
}

void TestDoxml::parseCompound_docMarkupType()
{
    /// \todo Implement TestDoxml::parseCompound_docMarkupType().
    QXmlStreamReader xml;
    doxlee::Doxml doxml(QString{});
    QCOMPARE(doxml.parseCompound_docMarkupType(xml), QVariantMap{});
}

void TestDoxml::parseCompound_docURLLink_data()
{
}

void TestDoxml::parseCompound_docURLLink()
{
    /// \todo Implement TestDoxml::parseCompound_docURLLink().
    QXmlStreamReader xml;
    doxlee::Doxml doxml(QString{});
    QCOMPARE(doxml.parseCompound_docURLLink(xml), QVariantMap{});
}

void TestDoxml::parseCompound_docAnchorType_data()
{
}

void TestDoxml::parseCompound_docAnchorType()
{
    /// \todo Implement TestDoxml::parseCompound_docAnchorType().
    QXmlStreamReader xml;
    doxlee::Doxml doxml(QString{});
    QCOMPARE(doxml.parseCompound_docAnchorType(xml), QVariantMap{});
}

void TestDoxml::parseCompound_docFormulaType_data()
{
}

void TestDoxml::parseCompound_docFormulaType()
{
    /// \todo Implement TestDoxml::parseCompound_docFormulaType().
    QXmlStreamReader xml;
    doxlee::Doxml doxml(QString{});
    QCOMPARE(doxml.parseCompound_docFormulaType(xml), QVariantMap{});
}

void TestDoxml::parseCompound_docIndexEntryType_data()
{
}

void TestDoxml::parseCompound_docIndexEntryType()
{
    /// \todo Implement TestDoxml::parseCompound_docIndexEntryType().
    QXmlStreamReader xml;
    doxlee::Doxml doxml(QString{});
    QCOMPARE(doxml.parseCompound_docIndexEntryType(xml), QVariantMap{});
}

void TestDoxml::parseCompound_docListType_data()
{
}

void TestDoxml::parseCompound_docListType()
{
    /// \todo Implement TestDoxml::parseCompound_docListType().
    QXmlStreamReader xml;
    doxlee::Doxml doxml(QString{});
    QCOMPARE(doxml.parseCompound_docListType(xml), QVariantMap{});
}

void TestDoxml::parseCompound_docListItemType_data()
{
}

void TestDoxml::parseCompound_docListItemType()
{
    /// \todo Implement TestDoxml::parseCompound_docListItemType().
    QXmlStreamReader xml;
    doxlee::Doxml doxml(QString{});
    QCOMPARE(doxml.parseCompound_docListItemType(xml), QVariantMap{});
}

void TestDoxml::parseCompound_docSimpleSectType_data()
{
}

void TestDoxml::parseCompound_docSimpleSectType()
{
    /// \todo Implement TestDoxml::parseCompound_docSimpleSectType().
    QXmlStreamReader xml;
    doxlee::Doxml doxml(QString{});
    QCOMPARE(doxml.parseCompound_docSimpleSectType(xml), QVariantMap{});
}

void TestDoxml::parseCompound_docVarListEntryType_data()
{
}

void TestDoxml::parseCompound_docVarListEntryType()
{
    /// \todo Implement TestDoxml::parseCompound_docVarListEntryType().
    QXmlStreamReader xml;
    doxlee::Doxml doxml(QString{});
    QCOMPARE(doxml.parseCompound_docVarListEntryType(xml), QVariantMap{});
}

void TestDoxml::parseCompound_docVariableListType_data()
{
}

void TestDoxml::parseCompound_docVariableListType()
{
    /// \todo Implement TestDoxml::parseCompound_docVariableListType().
    QXmlStreamReader xml;
    doxlee::Doxml doxml(QString{});
    QCOMPARE(doxml.parseCompound_docVariableListType(xml), QVariantMap{});
}

void TestDoxml::parseCompound_docRefTextType_data()
{
}

void TestDoxml::parseCompound_docRefTextType()
{
    /// \todo Implement TestDoxml::parseCompound_docRefTextType().
    QXmlStreamReader xml;
    doxlee::Doxml doxml(QString{});
    QCOMPARE(doxml.parseCompound_docRefTextType(xml), QVariantMap{});
}

void TestDoxml::parseCompound_docTableType_data()
{
}

void TestDoxml::parseCompound_docTableType()
{
    /// \todo Implement TestDoxml::parseCompound_docTableType().
    QXmlStreamReader xml;
    doxlee::Doxml doxml(QString{});
    QCOMPARE(doxml.parseCompound_docTableType(xml), QVariantMap{});
}

void TestDoxml::parseCompound_docRowType_data()
{
}

void TestDoxml::parseCompound_docRowType()
{
    /// \todo Implement TestDoxml::parseCompound_docRowType().
    QXmlStreamReader xml;
    doxlee::Doxml doxml(QString{});
    QCOMPARE(doxml.parseCompound_docRowType(xml), QVariantMap{});
}

void TestDoxml::parseCompound_docEntryType_data()
{
}

void TestDoxml::parseCompound_docEntryType()
{
    /// \todo Implement TestDoxml::parseCompound_docEntryType().
    QXmlStreamReader xml;
    doxlee::Doxml doxml(QString{});
    QCOMPARE(doxml.parseCompound_docEntryType(xml), QVariantMap{});
}

void TestDoxml::parseCompound_docCaptionType_data()
{
}

void TestDoxml::parseCompound_docCaptionType()
{
    /// \todo Implement TestDoxml::parseCompound_docCaptionType().
    QXmlStreamReader xml;
    doxlee::Doxml doxml(QString{});
    QCOMPARE(doxml.parseCompound_docCaptionType(xml), QVariantMap{});
}

void TestDoxml::parseCompound_docHeadingType_data()
{
}

void TestDoxml::parseCompound_docHeadingType()
{
    /// \todo Implement TestDoxml::parseCompound_docHeadingType().
    QXmlStreamReader xml;
    doxlee::Doxml doxml(QString{});
    QCOMPARE(doxml.parseCompound_docHeadingType(xml), QVariantMap{});
}

void TestDoxml::parseCompound_docImageType_data()
{
}

void TestDoxml::parseCompound_docImageType()
{
    /// \todo Implement TestDoxml::parseCompound_docImageType().
    QXmlStreamReader xml;
    doxlee::Doxml doxml(QString{});
    QCOMPARE(doxml.parseCompound_docImageType(xml), QVariantMap{});
}

void TestDoxml::parseCompound_docDotMscType_data()
{
}

void TestDoxml::parseCompound_docDotMscType()
{
    /// \todo Implement TestDoxml::parseCompound_docDotMscType().
    QXmlStreamReader xml;
    doxlee::Doxml doxml(QString{});
    QCOMPARE(doxml.parseCompound_docDotMscType(xml), QVariantMap{});
}

void TestDoxml::parseCompound_docImageFileType_data()
{
}

void TestDoxml::parseCompound_docImageFileType()
{
    /// \todo Implement TestDoxml::parseCompound_docImageFileType().
    QXmlStreamReader xml;
    doxlee::Doxml doxml(QString{});
    QCOMPARE(doxml.parseCompound_docImageFileType(xml), QVariantMap{});
}

void TestDoxml::parseCompound_docPlantumlType_data()
{
}

void TestDoxml::parseCompound_docPlantumlType()
{
    /// \todo Implement TestDoxml::parseCompound_docPlantumlType().
    QXmlStreamReader xml;
    doxlee::Doxml doxml(QString{});
    QCOMPARE(doxml.parseCompound_docPlantumlType(xml), QVariantMap{});
}

void TestDoxml::parseCompound_docTocItemType_data()
{
}

void TestDoxml::parseCompound_docTocItemType()
{
    /// \todo Implement TestDoxml::parseCompound_docTocItemType().
    QXmlStreamReader xml;
    doxlee::Doxml doxml(QString{});
    QCOMPARE(doxml.parseCompound_docTocItemType(xml), QVariantMap{});
}

void TestDoxml::parseCompound_docTocListType_data()
{
}

void TestDoxml::parseCompound_docTocListType()
{
    /// \todo Implement TestDoxml::parseCompound_docTocListType().
    QXmlStreamReader xml;
    doxlee::Doxml doxml(QString{});
    QCOMPARE(doxml.parseCompound_docTocListType(xml), QVariantMap{});
}

void TestDoxml::parseCompound_docLanguageType_data()
{
}

void TestDoxml::parseCompound_docLanguageType()
{
    /// \todo Implement TestDoxml::parseCompound_docLanguageType().
    QXmlStreamReader xml;
    doxlee::Doxml doxml(QString{});
    QCOMPARE(doxml.parseCompound_docLanguageType(xml), QVariantMap{});
}

void TestDoxml::parseCompound_docParamListType_data()
{
}

void TestDoxml::parseCompound_docParamListType()
{
    /// \todo Implement TestDoxml::parseCompound_docParamListType().
    QXmlStreamReader xml;
    doxlee::Doxml doxml(QString{});
    QCOMPARE(doxml.parseCompound_docParamListType(xml), QVariantMap{});
}

void TestDoxml::parseCompound_docParamListItem_data()
{
}

void TestDoxml::parseCompound_docParamListItem()
{
    /// \todo Implement TestDoxml::parseCompound_docParamListItem().
    QXmlStreamReader xml;
    doxlee::Doxml doxml(QString{});
    QCOMPARE(doxml.parseCompound_docParamListItem(xml), QVariantMap{});
}

void TestDoxml::parseCompound_docParamNameList_data()
{
}

void TestDoxml::parseCompound_docParamNameList()
{
    /// \todo Implement TestDoxml::parseCompound_docParamNameList().
    QXmlStreamReader xml;
    doxlee::Doxml doxml(QString{});
    QCOMPARE(doxml.parseCompound_docParamNameList(xml), QVariantMap{});
}

void TestDoxml::parseCompound_docParamType_data()
{
}

void TestDoxml::parseCompound_docParamType()
{
    /// \todo Implement TestDoxml::parseCompound_docParamType().
    QXmlStreamReader xml;
    doxlee::Doxml doxml(QString{});
    QCOMPARE(doxml.parseCompound_docParamType(xml), QVariantMap{});
}

void TestDoxml::parseCompound_docParamName_data()
{
}

void TestDoxml::parseCompound_docParamName()
{
    /// \todo Implement TestDoxml::parseCompound_docParamName().
    QXmlStreamReader xml;
    doxlee::Doxml doxml(QString{});
    QCOMPARE(doxml.parseCompound_docParamName(xml), QVariantMap{});
}

void TestDoxml::parseCompound_docXRefSectType_data()
{
}

void TestDoxml::parseCompound_docXRefSectType()
{
    /// \todo Implement TestDoxml::parseCompound_docXRefSectType().
    QXmlStreamReader xml;
    doxlee::Doxml doxml(QString{});
    QCOMPARE(doxml.parseCompound_docXRefSectType(xml), QVariantMap{});
}

void TestDoxml::parseCompound_docCopyType_data()
{
}

void TestDoxml::parseCompound_docCopyType()
{
    /// \todo Implement TestDoxml::parseCompound_docCopyType().
    QXmlStreamReader xml;
    doxlee::Doxml doxml(QString{});
    QCOMPARE(doxml.parseCompound_docCopyType(xml), QVariantMap{});
}

void TestDoxml::parseCompound_docDetailsType_data()
{
}

void TestDoxml::parseCompound_docDetailsType()
{
    /// \todo Implement TestDoxml::parseCompound_docDetailsType().
    QXmlStreamReader xml;
    doxlee::Doxml doxml(QString{});
    QCOMPARE(doxml.parseCompound_docDetailsType(xml), QVariantMap{});
}

void TestDoxml::parseCompound_docBlockQuoteType_data()
{
}

void TestDoxml::parseCompound_docBlockQuoteType()
{
    /// \todo Implement TestDoxml::parseCompound_docBlockQuoteType().
    QXmlStreamReader xml;
    doxlee::Doxml doxml(QString{});
    QCOMPARE(doxml.parseCompound_docBlockQuoteType(xml), QVariantMap{});
}

void TestDoxml::parseCompound_docParBlockType_data()
{
}

void TestDoxml::parseCompound_docParBlockType()
{
    /// \todo Implement TestDoxml::parseCompound_docParBlockType().
    QXmlStreamReader xml;
    doxlee::Doxml doxml(QString{});
    QCOMPARE(doxml.parseCompound_docParBlockType(xml), QVariantMap{});
}

void TestDoxml::parseCompound_docEmptyType_data()
{
}

void TestDoxml::parseCompound_docEmptyType()
{
    /// \todo Implement TestDoxml::parseCompound_docEmptyType().
    QXmlStreamReader xml;
    doxlee::Doxml doxml(QString{});
    QCOMPARE(doxml.parseCompound_docEmptyType(xml), QVariantMap{});
}

void TestDoxml::parseCompound_tableofcontentsType_data()
{
}

void TestDoxml::parseCompound_tableofcontentsType()
{
    /// \todo Implement TestDoxml::parseCompound_tableofcontentsType().
    QXmlStreamReader xml;
    doxlee::Doxml doxml(QString{});
    QCOMPARE(doxml.parseCompound_tableofcontentsType(xml), QVariantMap{});
}

void TestDoxml::parseCompound_tableofcontentsKindType_data()
{
}

void TestDoxml::parseCompound_tableofcontentsKindType()
{
    /// \todo Implement TestDoxml::parseCompound_tableofcontentsKindType().
    QXmlStreamReader xml;
    doxlee::Doxml doxml(QString{});
    QCOMPARE(doxml.parseCompound_tableofcontentsKindType(xml), QVariantMap{});
}

void TestDoxml::parseCompound_docEmojiType_data()
{
}

void TestDoxml::parseCompound_docEmojiType()
{
    /// \todo Implement TestDoxml::parseCompound_docEmojiType().
    QXmlStreamReader xml;
    doxlee::Doxml doxml(QString{});
    QCOMPARE(doxml.parseCompound_docEmojiType(xml), QVariantMap{});
}

void TestDoxml::parseDoxyfile_data()
{
    parseDoxyfile_DoxygenFileType_data();
}

void TestDoxml::parseDoxyfile()
{
    QFETCH(QString, xmlString);
    QXmlStreamReader xml(xmlString);
    doxlee::Doxml doxml(QString{});
    QTEST(doxml.parseDoxyfile(xml), "expected");
}

void TestDoxml::parseDoxyfile_DoxygenFileType_data()
{
    QTest::addColumn<QString>("xmlString");
    QTest::addColumn<QVariantMap>("expected");

    QTest::addRow("string") << QStringLiteral(R"(<?xml version='1.0' encoding='UTF-8' standalone='no'?>
        <doxyfile xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:noNamespaceSchemaLocation="doxyfile.xsd" version="1.10.0" xml:lang="en-US">
            <option id='QUIET' default='yes' type='bool'><value>NO</value></option>
            <option id='WARNINGS' default='yes' type='bool'><value>YES</value></option>
            <option id='TAB_SIZE' default='yes' type='int'><value>4</value></option>
            <option id='PAPER_TYPE' default='yes' type='string'><value>a4</value></option>
            <option id='STRIP_FROM_PATH' default='yes' type='stringlist'>
            </option>
            <option id='INPUT' default='no' type='stringlist'>
                <value><![CDATA[./src1]]></value>
                <value><![CDATA[./src2]]></value>
            </option>
        </doxyfile>)")
        << QVariantMap{
            { QSL("version"),  QSL("1.10.0") },
            { QSL("language"), QSL("en-US") },
            { QSL("options"),  QVariantMap{
                { QSL("QUIET"), false },
                { QSL("WARNINGS"), true },
                { QSL("TAB_SIZE"), 4 },
                { QSL("PAPER_TYPE"), QSL("a4") },
                { QSL("STRIP_FROM_PATH"), QStringList{ } },
                { QSL("INPUT"), QStringList{ QSL("./src1"), QSL("./src2") } },
            } },
        };
}

void TestDoxml::parseDoxyfile_DoxygenFileType()
{
    QFETCH(QString, xmlString);
    QXmlStreamReader xml(xmlString);
    xml.readNextStartElement();
    doxlee::Doxml doxml(QString{});
    QTEST(doxml.parseDoxyfile_DoxygenFileType(xml), "expected");
}

void TestDoxml::parseDoxyfile_OptionType_data()
{
    QTest::addColumn<QString>("xmlString");
    QTest::addColumn<QVariantMap>("expected");

    QTest::addRow("bool:no")
        << QSL("<option id='QUIET' default='yes' type='bool'><value>NO</value></option>")
        << QVariantMap{ { QSL("QUIET"), QVariant(false) } };

    QTest::addRow("bool:yes")
        << QSL("<option id='WARNINGS' default='yes' type='bool'><value>YES</value></option>")
        << QVariantMap{ { QSL("WARNINGS"), QVariant(true) } };

    QTest::addRow("int")
        << QSL("<option id='TAB_SIZE' default='yes' type='int'><value>4</value></option>")
        << QVariantMap{ { QSL("TAB_SIZE"), QVariant(4) } };

    QTest::addRow("string")
        << QSL("<option id='PAPER_TYPE' default='yes' type='string'><value>a4</value></option>")
        << QVariantMap{ { QSL("PAPER_TYPE"), QVariant(QSL("a4")) } };

    QTest::addRow("stinglist:empty")
        << QSL("<option id='STRIP_FROM_PATH' default='yes' type='stringlist'>\n</option>")
        << QVariantMap{ { QSL("STRIP_FROM_PATH"), QStringList{} } };

    QTest::addRow("stinglist")
        << QSL("<option id='INPUT' default='no' type='stringlist'>\
                   <value><![CDATA[./src1]]></value>\
                   <value><![CDATA[./src2]]></value>\
                   </option>")
        << QVariantMap{ { QSL("INPUT"), QStringList{ QSL("./src1"), QSL("./src2") } } };
}

void TestDoxml::parseDoxyfile_OptionType()
{
    QFETCH(QString, xmlString);
    QXmlStreamReader xml(xmlString);
    xml.readNextStartElement();
    doxlee::Doxml doxml(QString{});
    QTEST(doxml.parseDoxyfile_OptionType(xml), "expected");
}

void TestDoxml::parseIndex_data()
{
    parseIndex_DoxygenType_data();
}

void TestDoxml::parseIndex()
{
    QFETCH(QString, xmlString);
    QXmlStreamReader xml(xmlString);
    doxlee::Doxml doxml(QString{});
    QTEST(doxml.parseIndex(xml), "expected");
}

void TestDoxml::parseIndex_DoxygenType_data()
{
    QTest::addColumn<QString>("xmlString");
    QTest::addColumn<QVariantMap>("expected");

    QTest::addRow("index")
        << QSL(R"(<?xml version='1.0' encoding='UTF-8' standalone='no'?>
            <doxygenindex xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:noNamespaceSchemaLocation="index.xsd" version="1.10.0" xml:lang="en-US">
                <compound refid="foo" kind="namespace"><name>bar</name></copmound>
            </doxygenindex>)")
        << QVariantMap {
            { QSL("version"), QSL("1.10.0") },
            { QSL("language"), QSL("en-US") },
            { QSL("compounds"), QVariantList {
                QVariantMap {
                    { QSL("refid"), QSL("foo") },
                    { QSL("kind"), QSL("namespace") },
                    { QSL("name"), QSL("bar") },
                    { QSL("members"), QVariantList{ } }, // Yes, we expect an empty 'members' variant list.
                }
            } }
        };
}

void TestDoxml::parseIndex_DoxygenType()
{
    QFETCH(QString, xmlString);
    QXmlStreamReader xml(xmlString);
    xml.readNextStartElement();
    doxlee::Doxml doxml(QString{});
    QTEST(doxml.parseIndex_DoxygenType(xml), "expected");
}

void TestDoxml::parseIndex_CompoundType_data()
{
    QTest::addColumn<QString>("xmlString");
    QTest::addColumn<QVariantMap>("expected");

    QTest::addRow("without-members")
        << QSL(R"(<compound refid="foo" kind="namespace"><name>bar</name></copmound>)")
        << QVariantMap {
            { QSL("refid"), QSL("foo") },
            { QSL("kind"), QSL("namespace") },
            { QSL("name"), QSL("bar") },
            { QSL("members"), QVariantList{ } }, // Yes, we expect an empty 'members' variant list.
        };

    QTest::addRow("with-members")
        << QSL(R"(<compound refid="foo" kind="namespace"><name>bar</name>
                    <member refid="baz" kind="enum"><name>qux</name></member>
                    <member refid="abc" kind="define"><name>def</name></member>
                  </copmound>)")
        << QVariantMap {
            { QSL("refid"), QSL("foo") },
            { QSL("kind"), QSL("namespace") },
            { QSL("name"), QSL("bar") },
            { QSL("members"), QVariantList{
                QVariantMap {
                    { QSL("refid"), QSL("baz") },
                    { QSL("kind"), QSL("enum") },
                    { QSL("name"), QSL("qux") },
                },
                QVariantMap {
                    { QSL("refid"), QSL("abc") },
                    { QSL("kind"), QSL("define") },
                    { QSL("name"), QSL("def") },
                },
            } }
        };
}

void TestDoxml::parseIndex_CompoundType()
{
    QFETCH(QString, xmlString);
    QXmlStreamReader xml(xmlString);
    xml.readNextStartElement();
    doxlee::Doxml doxml(QString{});
    QTEST(doxml.parseIndex_CompoundType(xml), "expected");
}

void TestDoxml::parseIndex_MemberType_data()
{
    QTest::addColumn<QString>("xmlString");
    QTest::addColumn<QVariantMap>("expected");

    QTest::addRow("enum")
        << QSL(R"(<member refid="foo" kind="enum"><name>bar</name></member>)")
        << QVariantMap{
            { QSL("refid"), QSL("foo") },
            { QSL("kind"), QSL("enum") },
            { QSL("name"), QSL("bar") },
        };
}

void TestDoxml::parseIndex_MemberType()
{
    QFETCH(QString, xmlString);
    QXmlStreamReader xml(xmlString);
    xml.readNextStartElement();
    doxlee::Doxml doxml(QString{});
    QTEST(doxml.parseIndex_MemberType(xml), "expected");
}

QTEST_MAIN(TestDoxml)