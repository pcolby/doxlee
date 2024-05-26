// SPDX-FileCopyrightText: 2021-2024 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include <QDir>
#include <QLoggingCategory>
#include <QPair>
#include <QVariantMap>
#include <QXmlStreamReader>

#ifndef DOXLEE_DOXML_H
#define DOXLEE_DOXML_H

namespace doxlee {

namespace doxml {

/// \todo Remove these stanalone functions as the Doxml class supersedes them.

QPair<QStringList,QStringList> kinds(const QDir &doxmlDir);
QPair<QStringList,QStringList> kinds(const QString &indexXsdPath);

QVariantMap parseIndex(const QDir &doxmlDir, const bool extraIndexes = true);
QVariantMap parseIndex(const QString &indexXmlPath, const bool extraIndexes = true);

QVariantMap extraIndexes(const QVariantList &compounds);

QVariantMap parseCompound(const QDir &doxmlDir, const QString &refId);
QVariantMap parseCompound(const QString &compoundXmlPath);

} // namespace doxml

class Doxml {

public:
    explicit Doxml(const QString &doxmlDir);
    bool isValid() const;

    QVariantMap compound(const QString &refId) const;
    QVariantMap doxyfile() const;
    QVariantMap index() const;

protected:
    QString location(const QXmlStreamReader &xml) const;

private:
    static Q_LOGGING_CATEGORY(lc, "doxlee.doxml", QtInfoMsg);

    const QDir doxmlDir;
    // mutable QVariantMap indexCache;
    QString currentXmlFilePath;

    QVariantMap parseCompound                         (QXmlStreamReader &xml) const;
    QVariantMap parseCompound_DoxygenType             (QXmlStreamReader &xml) const;
    QVariantMap parseCompound_compounddefType         (QXmlStreamReader &xml) const;
    QVariantMap parseCompound_listofallmembersType    (QXmlStreamReader &xml) const;
    QVariantMap parseCompound_memberRefType           (QXmlStreamReader &xml) const;
    QVariantMap parseCompound_docHtmlOnlyType         (QXmlStreamReader &xml) const;
    QVariantMap parseCompound_compoundRefType         (QXmlStreamReader &xml) const;
    QVariantMap parseCompound_reimplementType         (QXmlStreamReader &xml) const;
    QVariantMap parseCompound_incType                 (QXmlStreamReader &xml) const;
    QVariantMap parseCompound_exportsType             (QXmlStreamReader &xml) const;
    QVariantMap parseCompound_exportType              (QXmlStreamReader &xml) const;
    QVariantMap parseCompound_refType                 (QXmlStreamReader &xml) const;
    QVariantMap parseCompound_refTextType             (QXmlStreamReader &xml) const;
    QVariantMap parseCompound_MemberType              (QXmlStreamReader &xml) const;
    QVariantMap parseCompound_sectiondefType          (QXmlStreamReader &xml) const;
    QVariantMap parseCompound_memberdefType           (QXmlStreamReader &xml) const;
    QVariantMap parseCompound_descriptionType         (QXmlStreamReader &xml) const;
    QVariantMap parseCompound_enumvalueType           (QXmlStreamReader &xml) const;
    QVariantMap parseCompound_templateparamlistType   (QXmlStreamReader &xml) const;
    QVariantMap parseCompound_paramType               (QXmlStreamReader &xml) const;
    QVariantMap parseCompound_linkedTextType          (QXmlStreamReader &xml) const;
    QVariantMap parseCompound_graphType               (QXmlStreamReader &xml) const;
    QVariantMap parseCompound_nodeType                (QXmlStreamReader &xml) const;
    QVariantMap parseCompound_childnodeType           (QXmlStreamReader &xml) const;
    QVariantMap parseCompound_linkType                (QXmlStreamReader &xml) const;
    QVariantMap parseCompound_listingType             (QXmlStreamReader &xml) const;
    QVariantMap parseCompound_codelineType            (QXmlStreamReader &xml) const;
    QVariantMap parseCompound_highlightType           (QXmlStreamReader &xml) const;
    QVariantMap parseCompound_spType                  (QXmlStreamReader &xml) const;
    QVariantMap parseCompound_referenceType           (QXmlStreamReader &xml) const;
    QVariantMap parseCompound_locationType            (QXmlStreamReader &xml) const;
    QVariantMap parseCompound_docSect1Type            (QXmlStreamReader &xml) const;
    QVariantMap parseCompound_docSect2Type            (QXmlStreamReader &xml) const;
    QVariantMap parseCompound_docSect3Type            (QXmlStreamReader &xml) const;
    QVariantMap parseCompound_docSect4Type            (QXmlStreamReader &xml) const;
    QVariantMap parseCompound_docInternalType         (QXmlStreamReader &xml) const;
    QVariantMap parseCompound_docInternalS1Type       (QXmlStreamReader &xml) const;
    QVariantMap parseCompound_docInternalS2Type       (QXmlStreamReader &xml) const;
    QVariantMap parseCompound_docInternalS3Type       (QXmlStreamReader &xml) const;
    QVariantMap parseCompound_docInternalS4Type       (QXmlStreamReader &xml) const;
    QVariantMap parseCompound_docTitleType            (QXmlStreamReader &xml) const;
    QVariantMap parseCompound_docSummaryType          (QXmlStreamReader &xml) const;
    QVariantMap parseCompound_docParaType             (QXmlStreamReader &xml) const;
    QVariantMap parseCompound_docMarkupType           (QXmlStreamReader &xml) const;
    QVariantMap parseCompound_docURLLink              (QXmlStreamReader &xml) const;
    QVariantMap parseCompound_docAnchorType           (QXmlStreamReader &xml) const;
    QVariantMap parseCompound_docFormulaType          (QXmlStreamReader &xml) const;
    QVariantMap parseCompound_docIndexEntryType       (QXmlStreamReader &xml) const;
    QVariantMap parseCompound_docListType             (QXmlStreamReader &xml) const;
    QVariantMap parseCompound_docListItemType         (QXmlStreamReader &xml) const;
    QVariantMap parseCompound_docSimpleSectType       (QXmlStreamReader &xml) const;
    QVariantMap parseCompound_docVarListEntryType     (QXmlStreamReader &xml) const;
    QVariantMap parseCompound_docVariableListType     (QXmlStreamReader &xml) const;
    QVariantMap parseCompound_docRefTextType          (QXmlStreamReader &xml) const;
    QVariantMap parseCompound_docTableType            (QXmlStreamReader &xml) const;
    QVariantMap parseCompound_docRowType              (QXmlStreamReader &xml) const;
    QVariantMap parseCompound_docEntryType            (QXmlStreamReader &xml) const;
    QVariantMap parseCompound_docCaptionType          (QXmlStreamReader &xml) const;
    QVariantMap parseCompound_docHeadingType          (QXmlStreamReader &xml) const;
    QVariantMap parseCompound_docImageType            (QXmlStreamReader &xml) const;
    QVariantMap parseCompound_docDotMscType           (QXmlStreamReader &xml) const;
    QVariantMap parseCompound_docImageFileType        (QXmlStreamReader &xml) const;
    QVariantMap parseCompound_docPlantumlType         (QXmlStreamReader &xml) const;
    QVariantMap parseCompound_docTocItemType          (QXmlStreamReader &xml) const;
    QVariantMap parseCompound_docTocListType          (QXmlStreamReader &xml) const;
    QVariantMap parseCompound_docLanguageType         (QXmlStreamReader &xml) const;
    QVariantMap parseCompound_docParamListType        (QXmlStreamReader &xml) const;
    QVariantMap parseCompound_docParamListItem        (QXmlStreamReader &xml) const;
    QVariantMap parseCompound_docParamNameList        (QXmlStreamReader &xml) const;
    QVariantMap parseCompound_docParamType            (QXmlStreamReader &xml) const;
    QVariantMap parseCompound_docParamName            (QXmlStreamReader &xml) const;
    QVariantMap parseCompound_docXRefSectType         (QXmlStreamReader &xml) const;
    QVariantMap parseCompound_docCopyType             (QXmlStreamReader &xml) const;
    QVariantMap parseCompound_docDetailsType          (QXmlStreamReader &xml) const;
    QVariantMap parseCompound_docBlockQuoteType       (QXmlStreamReader &xml) const;
    QVariantMap parseCompound_docParBlockType         (QXmlStreamReader &xml) const;
    QVariantMap parseCompound_docEmptyType            (QXmlStreamReader &xml) const;
    QVariantMap parseCompound_tableofcontentsType     (QXmlStreamReader &xml) const;
    QVariantMap parseCompound_tableofcontentsKindType (QXmlStreamReader &xml) const;
    QVariantMap parseCompound_docEmojiType            (QXmlStreamReader &xml) const;

    QVariantMap parseDoxyfile                         (QXmlStreamReader &xml) const;
    QVariantMap parseDoxyfile_DoxygenFileType         (QXmlStreamReader &xml) const;
    QVariantMap parseDoxyfile_OptionType              (QXmlStreamReader &xml) const;

    QVariantMap parseIndex                            (QXmlStreamReader &xml) const;
    QVariantMap parseIndex_DoxygenType                (QXmlStreamReader &xml) const;
    QVariantMap parseIndex_CompoundType               (QXmlStreamReader &xml) const;
    QVariantMap parseIndex_MemberType                 (QXmlStreamReader &xml) const;
};

} // namespace doxlee

#endif // DOXLEE_DOXML_H
