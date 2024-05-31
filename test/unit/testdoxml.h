// SPDX-FileCopyrightText: 2021-2024 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include <QtTest>

class TestDoxml: public QObject {
    Q_OBJECT

private slots:
    void location_data();
    void location();

    void logError();

    void logWarning();

    void parseNumericCharacterReference_data();
    void parseNumericCharacterReference();

    void parseCompound_data();
    void parseCompound();

    void parseCompound_DoxygenType_data();
    void parseCompound_DoxygenType();

    void parseCompound_compounddefType_data();
    void parseCompound_compounddefType();

    void parseCompound_listofallmembersType_data();
    void parseCompound_listofallmembersType();

    void parseCompound_memberRefType_data();
    void parseCompound_memberRefType();

    void parseCompound_docHtmlOnlyType_data();
    void parseCompound_docHtmlOnlyType();

    void parseCompound_compoundRefType_data();
    void parseCompound_compoundRefType();

    void parseCompound_reimplementType_data();
    void parseCompound_reimplementType();

    void parseCompound_incType_data();
    void parseCompound_incType();

    void parseCompound_exportsType_data();
    void parseCompound_exportsType();

    void parseCompound_exportType_data();
    void parseCompound_exportType();

    void parseCompound_refType_data();
    void parseCompound_refType();

    void parseCompound_refTextType_data();
    void parseCompound_refTextType();

    void parseCompound_MemberType_data();
    void parseCompound_MemberType();

    void parseCompound_sectiondefType_data();
    void parseCompound_sectiondefType();

    void parseCompound_memberdefType_data();
    void parseCompound_memberdefType();

    void parseCompound_descriptionType_data();
    void parseCompound_descriptionType();

    void parseCompound_enumvalueType_data();
    void parseCompound_enumvalueType();

    void parseCompound_templateparamlistType_data();
    void parseCompound_templateparamlistType();

    void parseCompound_paramType_data();
    void parseCompound_paramType();

    void parseCompound_linkedTextType_data();
    void parseCompound_linkedTextType();

    void parseCompound_graphType_data();
    void parseCompound_graphType();

    void parseCompound_nodeType_data();
    void parseCompound_nodeType();

    void parseCompound_childnodeType_data();
    void parseCompound_childnodeType();

    void parseCompound_linkType_data();
    void parseCompound_linkType();

    void parseCompound_listingType_data();
    void parseCompound_listingType();

    void parseCompound_codelineType_data();
    void parseCompound_codelineType();

    void parseCompound_highlightType_data();
    void parseCompound_highlightType();

    void parseCompound_spType_data();
    void parseCompound_spType();

    void parseCompound_referenceType_data();
    void parseCompound_referenceType();

    void parseCompound_locationType_data();
    void parseCompound_locationType();

    void parseCompound_docSect1Type_data();
    void parseCompound_docSect1Type();

    void parseCompound_docSect2Type_data();
    void parseCompound_docSect2Type();

    void parseCompound_docSect3Type_data();
    void parseCompound_docSect3Type();

    void parseCompound_docSect4Type_data();
    void parseCompound_docSect4Type();

    void parseCompound_docInternalType_data();
    void parseCompound_docInternalType();

    void parseCompound_docInternalS1Type_data();
    void parseCompound_docInternalS1Type();

    void parseCompound_docInternalS2Type_data();
    void parseCompound_docInternalS2Type();

    void parseCompound_docInternalS3Type_data();
    void parseCompound_docInternalS3Type();

    void parseCompound_docInternalS4Type_data();
    void parseCompound_docInternalS4Type();

    void parseCompound_docTitleType_data();
    void parseCompound_docTitleType();

    void parseCompound_docSummaryType_data();
    void parseCompound_docSummaryType();

    void parseCompound_docParaType_data();
    void parseCompound_docParaType();

    void parseCompound_docMarkupType_data();
    void parseCompound_docMarkupType();

    void parseCompound_docURLLink_data();
    void parseCompound_docURLLink();

    void parseCompound_docAnchorType_data();
    void parseCompound_docAnchorType();

    void parseCompound_docFormulaType_data();
    void parseCompound_docFormulaType();

    void parseCompound_docIndexEntryType_data();
    void parseCompound_docIndexEntryType();

    void parseCompound_docListType_data();
    void parseCompound_docListType();

    void parseCompound_docListItemType_data();
    void parseCompound_docListItemType();

    void parseCompound_docSimpleSectType_data();
    void parseCompound_docSimpleSectType();

    void parseCompound_docVarListEntryType_data();
    void parseCompound_docVarListEntryType();

    void parseCompound_docVariableListType_data();
    void parseCompound_docVariableListType();

    void parseCompound_docRefTextType_data();
    void parseCompound_docRefTextType();

    void parseCompound_docTableType_data();
    void parseCompound_docTableType();

    void parseCompound_docRowType_data();
    void parseCompound_docRowType();

    void parseCompound_docEntryType_data();
    void parseCompound_docEntryType();

    void parseCompound_docCaptionType_data();
    void parseCompound_docCaptionType();

    void parseCompound_docHeadingType_data();
    void parseCompound_docHeadingType();

    void parseCompound_docImageType_data();
    void parseCompound_docImageType();

    void parseCompound_docDotMscType_data();
    void parseCompound_docDotMscType();

    void parseCompound_docImageFileType_data();
    void parseCompound_docImageFileType();

    void parseCompound_docPlantumlType_data();
    void parseCompound_docPlantumlType();

    void parseCompound_docTocItemType_data();
    void parseCompound_docTocItemType();

    void parseCompound_docTocListType_data();
    void parseCompound_docTocListType();

    void parseCompound_docLanguageType_data();
    void parseCompound_docLanguageType();

    void parseCompound_docParamListType_data();
    void parseCompound_docParamListType();

    void parseCompound_docParamListItem_data();
    void parseCompound_docParamListItem();

    void parseCompound_docParamNameList_data();
    void parseCompound_docParamNameList();

    void parseCompound_docParamType_data();
    void parseCompound_docParamType();

    void parseCompound_docParamName_data();
    void parseCompound_docParamName();

    void parseCompound_docXRefSectType_data();
    void parseCompound_docXRefSectType();

    void parseCompound_docCopyType_data();
    void parseCompound_docCopyType();

    void parseCompound_docDetailsType_data();
    void parseCompound_docDetailsType();

    void parseCompound_docBlockQuoteType_data();
    void parseCompound_docBlockQuoteType();

    void parseCompound_docParBlockType_data();
    void parseCompound_docParBlockType();

    void parseCompound_docEmptyType_data();
    void parseCompound_docEmptyType();

    void parseCompound_tableofcontentsType_data();
    void parseCompound_tableofcontentsType();

    void parseCompound_tableofcontentsKindType_data();
    void parseCompound_tableofcontentsKindType();

    void parseCompound_docEmojiType_data();
    void parseCompound_docEmojiType();

    void parseDoxyfile_data();
    void parseDoxyfile();

    void parseDoxyfile_DoxygenFileType_data();
    void parseDoxyfile_DoxygenFileType();

    void parseDoxyfile_OptionType_data();
    void parseDoxyfile_OptionType();

    void parseIndex_data();
    void parseIndex();

    void parseIndex_DoxygenType_data();
    void parseIndex_DoxygenType();

    void parseIndex_CompoundType_data();
    void parseIndex_CompoundType();

    void parseIndex_MemberType_data();
    void parseIndex_MemberType();
};
