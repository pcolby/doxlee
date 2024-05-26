// SPDX-FileCopyrightText: 2021-2024 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "doxml.h"

#include "variant.h"

#include <QCoreApplication>
#include <QDebug>
#include <QJsonDocument> /// \todo Remove; only for early dev / debugging.

/// Shorten the QStringLiteral macro for readability.
#define QSL(str) QStringLiteral(str)

/// Shorten QCoreApplication::translate calls for readability.
#define QTR(str) QCoreApplication::translate("doxml", str)

namespace doxlee {

namespace doxml {

static Q_LOGGING_CATEGORY(lc, "doxlee.doxml", QtInfoMsg);

QPair<QStringList,QStringList> kinds(const QDir &doxmlDir)
{
    return kinds(doxmlDir.absoluteFilePath(QSL("index.xsd")));
}

QPair<QStringList,QStringList> kinds(const QString &indexXsdPath)
{
    // Open the file for reading.
    QFile file(indexXsdPath);
    if (!file.open(QIODevice::ReadOnly|QIODevice::Text)) {
        qCWarning(lc).noquote() << QTR("Error opening file for reading: %1").arg(indexXsdPath);
        return { };
    }

    QStringList compoundKinds, memberKinds;

    // Parse the opening <schema> element.
    QXmlStreamReader xml(&file);
    if (!xml.readNextStartElement()) {
        qCWarning(lc).noquote() << QTR("Invalid XML file: %1 - %2").arg(indexXsdPath, xml.errorString());
        return { };
    }
    if (xml.name() != QSL("schema")) {
        qCWarning(lc).noquote() << QTR("File is not a Doxygen XML index schema: %1 - %2")
            .arg(indexXsdPath, xml.name().toString());
        return { };
    }

    // Parse the contained 'kind' elements.
    while ((!xml.atEnd()) && (xml.readNextStartElement())) {
        if (xml.name() == QSL("simpleType")) {
            const QString nameAttribute = xml.attributes().value(QSL("name")).toString();
            if ((nameAttribute == QSL("CompoundKind")) || (nameAttribute == QSL("MemberKind"))) {
                while ((!xml.atEnd()) && (xml.readNextStartElement())) {
                    if (xml.name() == QSL("restriction")) {
                        QStringList &kindsList = (nameAttribute == QLatin1String("CompoundKind"))
                            ? compoundKinds : memberKinds;
                        while ((!xml.atEnd()) && (xml.readNextStartElement())) {
                            if (xml.name() == QSL("enumeration")) {
                                kindsList.append(xml.attributes().value(QSL("value")).toString());
                            }
                            xml.skipCurrentElement();
                        }
                    } else xml.skipCurrentElement();
                }
            } else xml.skipCurrentElement();
        } else xml.skipCurrentElement();
    }
    qCInfo(lc).noquote() << QTR("Parsed %1 compound kind(s), and %2 member kind(s) from %3")
        .arg(compoundKinds.size()).arg(memberKinds.size()).arg(indexXsdPath);
    return { compoundKinds, memberKinds };
}

QVariantMap parseIndex(const QDir &doxmlDir, const bool extraIndexes)
{
    return parseIndex(doxmlDir.absoluteFilePath(QSL("index.xml")), extraIndexes);
}

QVariantMap parseIndex(const QString &indexXmlPath, const bool extraIndexes)
{
    // Open the file for reading.
    QFile file(indexXmlPath);
    if (!file.open(QIODevice::ReadOnly|QIODevice::Text)) {
        qCWarning(lc).noquote() << QTR("Error opening file for reading: %1").arg(indexXmlPath);
        return QVariantMap();
    }

    // Parse the opening <doxygenindex> element.
    QXmlStreamReader xml(&file);
    if (!xml.readNextStartElement()) {
        qCWarning(lc).noquote() << QTR("Invalid XML file: %1 - %2").arg(indexXmlPath, xml.errorString());
        return QVariantMap();
    }
    qCDebug(lc) << xml.name() << "version" << xml.attributes().value(QSL("version"))
             << xml.attributes().value(QSL("xml:lang")).toString();
    if (xml.name() != QSL("doxygenindex")) {
        qCWarning(lc).noquote() << QTR("File is not a Doxygen XML index: %1 - %2")
                                .arg(indexXmlPath, xml.name().toString());
        return QVariantMap();
    }
    QVariantMap indexMap;
    indexMap.insert(QSL("doxygenVersion"), xml.attributes().value(QSL("version")).toString());
    indexMap.insert(QSL("doxygenLanguage"), xml.attributes().value(QSL("xml:lang")).toString());

    // Parse the contained <compound> elements.
    QVariantList compounds;
    while ((!xml.atEnd()) && (xml.readNextStartElement())) {
        if (xml.name() == QSL("compound")) {
            QVariantMap compound;
            compound.insert(QSL("refid"), xml.attributes().value(QSL("refid")).toString());
            compound.insert(QSL("kind"), xml.attributes().value(QSL("kind")).toString());
            if ((!xml.readNextStartElement()) || (xml.name() != QSL("name"))) {
                qCWarning(lc).noquote() << QTR(" %1:%2:%3 <compound> does not begin with <name>")
                    .arg(indexXmlPath).arg(xml.lineNumber()).arg(xml.columnNumber());
                return QVariantMap();
            }
            compound.insert(QSL("name"), xml.readElementText());
            //qCDebug(lc) << __func__ << "compound" << compound;
            QVariantList members;
            while ((!xml.atEnd()) && (xml.readNextStartElement())) {
                if (xml.name() == QSL("member")) {
                    QVariantMap member;
                    member.insert(QSL("refid"), xml.attributes().value(QSL("refid")).toString());
                    member.insert(QSL("kind"), xml.attributes().value(QSL("kind")).toString());
                    if ((!xml.readNextStartElement()) || (xml.name() != QSL("name"))) {
                        qCWarning(lc).noquote() << QTR("%1:%2:%3 <member> does not begin with <name>")
                            .arg(indexXmlPath).arg(xml.lineNumber()).arg(xml.columnNumber());
                        return QVariantMap();
                    }
                    member.insert(QSL("name"), xml.readElementText());
                    //qCDebug(lc) << __func__ << "member" << member;
                    members.append(member);
                    xml.skipCurrentElement();
                } else {
                    qCWarning(lc).noquote() << QTR("Skipping unknown <%1> element at %2:%3:%4")
                        .arg(xml.name().toString(), indexXmlPath).arg(xml.lineNumber()).arg(xml.columnNumber());
                    xml.skipCurrentElement();
                }
            }
            compound.insert(QSL("members"), members);
            compounds.append(compound);
        } else {
            qCWarning(lc).noquote() << QTR("Skipping unknown <%1> element at %2:%3:%4")
                .arg(xml.name().toString(), indexXmlPath).arg(xml.lineNumber()).arg(xml.columnNumber());
            xml.skipCurrentElement();
        }
    }
    qCInfo(lc).noquote() << QTR("Parsed %1 compound(s) from %2").arg(compounds.size()).arg(indexXmlPath);
    indexMap.insert(QSL("compoundsList"), compounds);
    if (extraIndexes) {
        #if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
        indexMap.insert(doxml::extraIndexes(compounds));
        #else
        const QVariantMap extra = doxml::extraIndexes(compounds);
        for (auto iter = extra.constBegin(); iter != extra.constEnd(); ++iter) {
            indexMap.insert(iter.key(), iter.value());
        }
        #endif
    }
    return indexMap;
}

QVariantMap extraIndexes(const QVariantList &compounds)
{
    QHash<QString,QVariantList> compoundsByKind, membersByKind;
    QVariantMap compoundsByRefId, membersByRefId;
    for (const QVariant &compound: compounds) {
        const QVariantMap compoundMap = compound.toMap();
        {
            const QString kind = compoundMap.value(QSL("kind")).toString();
            const QString refid = compoundMap.value(QSL("refid")).toString();
            compoundsByKind[kind].append(compound);
            compoundsByRefId.insert(refid, compound);
        }

        const QVariantList members = compoundMap.value(QSL("members")).toList();
        for (const QVariant &member: members) {
            const QVariantMap memberMap = member.toMap();
            const QString kind = memberMap.value(QSL("kind")).toString();
            const QString refid = memberMap.value(QSL("refid")).toString();
            membersByKind[kind].append(member);
            membersByRefId.insert(refid, member);
        }
    }
    for (QVariantList &compoundsList: compoundsByKind) sortBy(compoundsList, QSL("name"));
    for (QVariantList &membersList: membersByKind)     sortBy(membersList,   QSL("name"));
    return QVariantMap{
        { QSL("compoundsByKind" ), toVariant(compoundsByKind) },
        { QSL("compoundsByRefId"), compoundsByRefId           },
        { QSL("membersByKind"   ), toVariant(membersByKind)   },
        { QSL("membersByRefId"  ), membersByRefId             },
    };
}

QVariantMap parseCompound(const QDir &doxmlDir, const QString &refId)
{
    return parseCompound(doxmlDir.absoluteFilePath(QSL("%1.xml").arg(refId)));
}

QVariantMap parseCompound(const QString &compoundXmlPath)
{
    // Open the compound XML file.
    QFile file(compoundXmlPath);
    if (!file.open(QIODevice::ReadOnly|QIODevice::Text)) {
        qCWarning(lc).noquote() << QTR("Error opening file for reading: %1").arg(compoundXmlPath);
        return QVariantMap();
    }

    // Parse, and fetch the compoundDef element.
    QXmlStreamReader xml(&file);
    const QVariantMap compoundDef = toVariant(xml).value(QSL("doxygen")).toMap().value(QSL("compounddef")).toMap();
    if (compoundDef.isEmpty()) {
        qCWarning(lc).noquote() << QTR("Error reading compond defintion: %1").arg(compoundXmlPath);
        return QVariantMap();
    }
    qCDebug(lc).noquote() << QJsonDocument::fromVariant(compoundDef).toJson();

    // Map the compoundDef properties to our own compound map.
    QVariantMap compound{
        { QSL("name"), compoundDef.value(QSL("compoundname")).toMap().value(QSL(".Characters")) },
        /// \todo Provide stand way of fetch, and trimming, all text from each of these...
        // { QSL("brief"),       compoundDef.value(QSL("briefdescription")).toMap().value(QSL(".Characters")) },
        // { QSL("description"), compoundDef.value(QSL("fulldescription" )).toMap().value(QSL(".Characters")) },
    };

    // Copy call compoundDef attributes to top-level compound properties.
    for (const QVariant &attribute: compoundDef) {
        const auto map = attribute.toMap();
        const auto name = map.find(QSL("QualifiedName"));
        const auto value = map.find(QSL("Value"));
        if ((name != map.constEnd()) && (value != map.constEnd())) {
            compound.insert(name->toString(), *value);
        }
    }

    /// \todo More parsing here...

    qCDebug(lc).noquote() << QJsonDocument::fromVariant(compound).toJson();
    return compound;
}

} // namespace doxml

Doxml::Doxml(const QString &doxmlDir) : doxmlDir(doxmlDir)
{

}

bool Doxml::isValid() const
{
    return false; /// \todo.
}

QString Doxml::location(const QXmlStreamReader &xml) const
{
    return QStringLiteral("%1:%2:%3").arg(currentXmlFilePath).arg(xml.lineNumber()).arg(xml.columnNumber());
}

QVariantMap Doxml::parseCompound(QXmlStreamReader &xml) const
{
    /// \todo Implement Doxml::parseCompound().
    Q_UNUSED(xml)
    return {};
}

QVariantMap Doxml::parseCompound_DoxygenType(QXmlStreamReader &xml) const
{
    /// \todo Implement Doxml::parseCompound_DoxygenType().
    Q_UNUSED(xml)
    return {};
}

QVariantMap Doxml::parseCompound_compounddefType(QXmlStreamReader &xml) const
{
    /// \todo Implement Doxml::parseCompound_compounddefType().
    Q_UNUSED(xml)
    return {};
}

QVariantMap Doxml::parseCompound_listofallmembersType(QXmlStreamReader &xml) const
{
    /// \todo Implement Doxml::parseCompound_listofallmembersType().
    Q_UNUSED(xml)
    return {};
}

QVariantMap Doxml::parseCompound_memberRefType(QXmlStreamReader &xml) const
{
    /// \todo Implement Doxml::parseCompound_memberRefType().
    Q_UNUSED(xml)
    return {};
}

QVariantMap Doxml::parseCompound_docHtmlOnlyType(QXmlStreamReader &xml) const
{
    /// \todo Implement Doxml::parseCompound_docHtmlOnlyType().
    Q_UNUSED(xml)
    return {};
}

QVariantMap Doxml::parseCompound_compoundRefType(QXmlStreamReader &xml) const
{
    /// \todo Implement Doxml::parseCompound_compoundRefType().
    Q_UNUSED(xml)
    return {};
}

QVariantMap Doxml::parseCompound_reimplementType(QXmlStreamReader &xml) const
{
    /// \todo Implement Doxml::parseCompound_reimplementType().
    Q_UNUSED(xml)
    return {};
}

QVariantMap Doxml::parseCompound_incType(QXmlStreamReader &xml) const
{
    /// \todo Implement Doxml::parseCompound_incType().
    Q_UNUSED(xml)
    return {};
}

QVariantMap Doxml::parseCompound_exportsType(QXmlStreamReader &xml) const
{
    /// \todo Implement Doxml::parseCompound_exportsType().
    Q_UNUSED(xml)
    return {};
}

QVariantMap Doxml::parseCompound_exportType(QXmlStreamReader &xml) const
{
    /// \todo Implement Doxml::parseCompound_exportType().
    Q_UNUSED(xml)
    return {};
}

QVariantMap Doxml::parseCompound_refType(QXmlStreamReader &xml) const
{
    /// \todo Implement Doxml::parseCompound_refType().
    Q_UNUSED(xml)
    return {};
}

QVariantMap Doxml::parseCompound_refTextType(QXmlStreamReader &xml) const
{
    /// \todo Implement Doxml::parseCompound_refTextType().
    Q_UNUSED(xml)
    return {};
}

QVariantMap Doxml::parseCompound_MemberType(QXmlStreamReader &xml) const
{
    /// \todo Implement Doxml::parseCompound_MemberType().
    Q_UNUSED(xml)
    return {};
}

QVariantMap Doxml::parseCompound_sectiondefType(QXmlStreamReader &xml) const
{
    /// \todo Implement Doxml::parseCompound_sectiondefType().
    Q_UNUSED(xml)
    return {};
}

QVariantMap Doxml::parseCompound_memberdefType(QXmlStreamReader &xml) const
{
    /// \todo Implement Doxml::parseCompound_memberdefType().
    Q_UNUSED(xml)
    return {};
}

QVariantMap Doxml::parseCompound_descriptionType(QXmlStreamReader &xml) const
{
    /// \todo Implement Doxml::parseCompound_descriptionType().
    Q_UNUSED(xml)
    return {};
}

QVariantMap Doxml::parseCompound_enumvalueType(QXmlStreamReader &xml) const
{
    /// \todo Implement Doxml::parseCompound_enumvalueType().
    Q_UNUSED(xml)
    return {};
}

QVariantMap Doxml::parseCompound_templateparamlistType(QXmlStreamReader &xml) const
{
    /// \todo Implement Doxml::parseCompound_templateparamlistType().
    Q_UNUSED(xml)
    return {};
}

QVariantMap Doxml::parseCompound_paramType(QXmlStreamReader &xml) const
{
    /// \todo Implement Doxml::parseCompound_paramType().
    Q_UNUSED(xml)
    return {};
}

QVariantMap Doxml::parseCompound_linkedTextType(QXmlStreamReader &xml) const
{
    /// \todo Implement Doxml::parseCompound_linkedTextType().
    Q_UNUSED(xml)
    return {};
}

QVariantMap Doxml::parseCompound_graphType(QXmlStreamReader &xml) const
{
    /// \todo Implement Doxml::parseCompound_graphType().
    Q_UNUSED(xml)
    return {};
}

QVariantMap Doxml::parseCompound_nodeType(QXmlStreamReader &xml) const
{
    /// \todo Implement Doxml::parseCompound_nodeType().
    Q_UNUSED(xml)
    return {};
}

QVariantMap Doxml::parseCompound_childnodeType(QXmlStreamReader &xml) const
{
    /// \todo Implement Doxml::parseCompound_childnodeType().
    Q_UNUSED(xml)
    return {};
}

QVariantMap Doxml::parseCompound_linkType(QXmlStreamReader &xml) const
{
    /// \todo Implement Doxml::parseCompound_linkType().
    Q_UNUSED(xml)
    return {};
}

QVariantMap Doxml::parseCompound_listingType(QXmlStreamReader &xml) const
{
    /// \todo Implement Doxml::parseCompound_listingType().
    Q_UNUSED(xml)
    return {};
}

QVariantMap Doxml::parseCompound_codelineType(QXmlStreamReader &xml) const
{
    /// \todo Implement Doxml::parseCompound_codelineType().
    Q_UNUSED(xml)
    return {};
}

QVariantMap Doxml::parseCompound_highlightType(QXmlStreamReader &xml) const
{
    /// \todo Implement Doxml::parseCompound_highlightType().
    Q_UNUSED(xml)
    return {};
}

QVariantMap Doxml::parseCompound_spType(QXmlStreamReader &xml) const
{
    /// \todo Implement Doxml::parseCompound_spType().
    Q_UNUSED(xml)
    return {};
}

QVariantMap Doxml::parseCompound_referenceType(QXmlStreamReader &xml) const
{
    /// \todo Implement Doxml::parseCompound_referenceType().
    Q_UNUSED(xml)
    return {};
}

QVariantMap Doxml::parseCompound_locationType(QXmlStreamReader &xml) const
{
    /// \todo Implement Doxml::parseCompound_locationType().
    Q_UNUSED(xml)
    return {};
}

QVariantMap Doxml::parseCompound_docSect1Type(QXmlStreamReader &xml) const
{
    /// \todo Implement Doxml::parseCompound_docSect1Type().
    Q_UNUSED(xml)
    return {};
}

QVariantMap Doxml::parseCompound_docSect2Type(QXmlStreamReader &xml) const
{
    /// \todo Implement Doxml::parseCompound_docSect2Type().
    Q_UNUSED(xml)
    return {};
}

QVariantMap Doxml::parseCompound_docSect3Type(QXmlStreamReader &xml) const
{
    /// \todo Implement Doxml::parseCompound_docSect3Type().
    Q_UNUSED(xml)
    return {};
}

QVariantMap Doxml::parseCompound_docSect4Type(QXmlStreamReader &xml) const
{
    /// \todo Implement Doxml::parseCompound_docSect4Type().
    Q_UNUSED(xml)
    return {};
}

QVariantMap Doxml::parseCompound_docInternalType(QXmlStreamReader &xml) const
{
    /// \todo Implement Doxml::parseCompound_docInternalType().
    Q_UNUSED(xml)
    return {};
}

QVariantMap Doxml::parseCompound_docInternalS1Type(QXmlStreamReader &xml) const
{
    /// \todo Implement Doxml::parseCompound_docInternalS1Type().
    Q_UNUSED(xml)
    return {};
}

QVariantMap Doxml::parseCompound_docInternalS2Type(QXmlStreamReader &xml) const
{
    /// \todo Implement Doxml::parseCompound_docInternalS2Type().
    Q_UNUSED(xml)
    return {};
}

QVariantMap Doxml::parseCompound_docInternalS3Type(QXmlStreamReader &xml) const
{
    /// \todo Implement Doxml::parseCompound_docInternalS3Type().
    Q_UNUSED(xml)
    return {};
}

QVariantMap Doxml::parseCompound_docInternalS4Type(QXmlStreamReader &xml) const
{
    /// \todo Implement Doxml::parseCompound_docInternalS4Type().
    Q_UNUSED(xml)
    return {};
}

QVariantMap Doxml::parseCompound_docTitleType(QXmlStreamReader &xml) const
{
    /// \todo Implement Doxml::parseCompound_docTitleType().
    Q_UNUSED(xml)
    return {};
}

QVariantMap Doxml::parseCompound_docSummaryType(QXmlStreamReader &xml) const
{
    /// \todo Implement Doxml::parseCompound_docSummaryType().
    Q_UNUSED(xml)
    return {};
}

QVariantMap Doxml::parseCompound_docParaType(QXmlStreamReader &xml) const
{
    /// \todo Implement Doxml::parseCompound_docParaType().
    Q_UNUSED(xml)
    return {};
}

QVariantMap Doxml::parseCompound_docMarkupType(QXmlStreamReader &xml) const
{
    /// \todo Implement Doxml::parseCompound_docMarkupType().
    Q_UNUSED(xml)
    return {};
}

QVariantMap Doxml::parseCompound_docURLLink(QXmlStreamReader &xml) const
{
    /// \todo Implement Doxml::parseCompound_docURLLink().
    Q_UNUSED(xml)
    return {};
}

QVariantMap Doxml::parseCompound_docAnchorType(QXmlStreamReader &xml) const
{
    /// \todo Implement Doxml::parseCompound_docAnchorType().
    Q_UNUSED(xml)
    return {};
}

QVariantMap Doxml::parseCompound_docFormulaType(QXmlStreamReader &xml) const
{
    /// \todo Implement Doxml::parseCompound_docFormulaType().
    Q_UNUSED(xml)
    return {};
}

QVariantMap Doxml::parseCompound_docIndexEntryType(QXmlStreamReader &xml) const
{
    /// \todo Implement Doxml::parseCompound_docIndexEntryType().
    Q_UNUSED(xml)
    return {};
}

QVariantMap Doxml::parseCompound_docListType(QXmlStreamReader &xml) const
{
    /// \todo Implement Doxml::parseCompound_docListType().
    Q_UNUSED(xml)
    return {};
}

QVariantMap Doxml::parseCompound_docListItemType(QXmlStreamReader &xml) const
{
    /// \todo Implement Doxml::parseCompound_docListItemType().
    Q_UNUSED(xml)
    return {};
}

QVariantMap Doxml::parseCompound_docSimpleSectType(QXmlStreamReader &xml) const
{
    /// \todo Implement Doxml::parseCompound_docSimpleSectType().
    Q_UNUSED(xml)
    return {};
}

QVariantMap Doxml::parseCompound_docVarListEntryType(QXmlStreamReader &xml) const
{
    /// \todo Implement Doxml::parseCompound_docVarListEntryType().
    Q_UNUSED(xml)
    return {};
}

QVariantMap Doxml::parseCompound_docVariableListType(QXmlStreamReader &xml) const
{
    /// \todo Implement Doxml::parseCompound_docVariableListType().
    Q_UNUSED(xml)
    return {};
}

QVariantMap Doxml::parseCompound_docRefTextType(QXmlStreamReader &xml) const
{
    /// \todo Implement Doxml::parseCompound_docRefTextType().
    Q_UNUSED(xml)
    return {};
}

QVariantMap Doxml::parseCompound_docTableType(QXmlStreamReader &xml) const
{
    /// \todo Implement Doxml::parseCompound_docTableType().
    Q_UNUSED(xml)
    return {};
}

QVariantMap Doxml::parseCompound_docRowType(QXmlStreamReader &xml) const
{
    /// \todo Implement Doxml::parseCompound_docRowType().
    Q_UNUSED(xml)
    return {};
}

QVariantMap Doxml::parseCompound_docEntryType(QXmlStreamReader &xml) const
{
    /// \todo Implement Doxml::parseCompound_docEntryType().
    Q_UNUSED(xml)
    return {};
}

QVariantMap Doxml::parseCompound_docCaptionType(QXmlStreamReader &xml) const
{
    /// \todo Implement Doxml::parseCompound_docCaptionType().
    Q_UNUSED(xml)
    return {};
}

QVariantMap Doxml::parseCompound_docHeadingType(QXmlStreamReader &xml) const
{
    /// \todo Implement Doxml::parseCompound_docHeadingType().
    Q_UNUSED(xml)
    return {};
}

QVariantMap Doxml::parseCompound_docImageType(QXmlStreamReader &xml) const
{
    /// \todo Implement Doxml::parseCompound_docImageType().
    Q_UNUSED(xml)
    return {};
}

QVariantMap Doxml::parseCompound_docDotMscType(QXmlStreamReader &xml) const
{
    /// \todo Implement Doxml::parseCompound_docDotMscType().
    Q_UNUSED(xml)
    return {};
}

QVariantMap Doxml::parseCompound_docImageFileType(QXmlStreamReader &xml) const
{
    /// \todo Implement Doxml::parseCompound_docImageFileType().
    Q_UNUSED(xml)
    return {};
}

QVariantMap Doxml::parseCompound_docPlantumlType(QXmlStreamReader &xml) const
{
    /// \todo Implement Doxml::parseCompound_docPlantumlType().
    Q_UNUSED(xml)
    return {};
}

QVariantMap Doxml::parseCompound_docTocItemType(QXmlStreamReader &xml) const
{
    /// \todo Implement Doxml::parseCompound_docTocItemType().
    Q_UNUSED(xml)
    return {};
}

QVariantMap Doxml::parseCompound_docTocListType(QXmlStreamReader &xml) const
{
    /// \todo Implement Doxml::parseCompound_docTocListType().
    Q_UNUSED(xml)
    return {};
}

QVariantMap Doxml::parseCompound_docLanguageType(QXmlStreamReader &xml) const
{
    /// \todo Implement Doxml::parseCompound_docLanguageType().
    Q_UNUSED(xml)
    return {};
}

QVariantMap Doxml::parseCompound_docParamListType(QXmlStreamReader &xml) const
{
    /// \todo Implement Doxml::parseCompound_docParamListType().
    Q_UNUSED(xml)
    return {};
}

QVariantMap Doxml::parseCompound_docParamListItem(QXmlStreamReader &xml) const
{
    /// \todo Implement Doxml::parseCompound_docParamListItem().
    Q_UNUSED(xml)
    return {};
}

QVariantMap Doxml::parseCompound_docParamNameList(QXmlStreamReader &xml) const
{
    /// \todo Implement Doxml::parseCompound_docParamNameList().
    Q_UNUSED(xml)
    return {};
}

QVariantMap Doxml::parseCompound_docParamType(QXmlStreamReader &xml) const
{
    /// \todo Implement Doxml::parseCompound_docParamType().
    Q_UNUSED(xml)
    return {};
}

QVariantMap Doxml::parseCompound_docParamName(QXmlStreamReader &xml) const
{
    /// \todo Implement Doxml::parseCompound_docParamName().
    Q_UNUSED(xml)
    return {};
}

QVariantMap Doxml::parseCompound_docXRefSectType(QXmlStreamReader &xml) const
{
    /// \todo Implement Doxml::parseCompound_docXRefSectType().
    Q_UNUSED(xml)
    return {};
}

QVariantMap Doxml::parseCompound_docCopyType(QXmlStreamReader &xml) const
{
    /// \todo Implement Doxml::parseCompound_docCopyType().
    Q_UNUSED(xml)
    return {};
}

QVariantMap Doxml::parseCompound_docDetailsType(QXmlStreamReader &xml) const
{
    /// \todo Implement Doxml::parseCompound_docDetailsType().
    Q_UNUSED(xml)
    return {};
}

QVariantMap Doxml::parseCompound_docBlockQuoteType(QXmlStreamReader &xml) const
{
    /// \todo Implement Doxml::parseCompound_docBlockQuoteType().
    Q_UNUSED(xml)
    return {};
}

QVariantMap Doxml::parseCompound_docParBlockType(QXmlStreamReader &xml) const
{
    /// \todo Implement Doxml::parseCompound_docParBlockType().
    Q_UNUSED(xml)
    return {};
}

QVariantMap Doxml::parseCompound_docEmptyType(QXmlStreamReader &xml) const
{
    /// \todo Implement Doxml::parseCompound_docEmptyType().
    Q_UNUSED(xml)
    return {};
}

QVariantMap Doxml::parseCompound_tableofcontentsType(QXmlStreamReader &xml) const
{
    /// \todo Implement Doxml::parseCompound_tableofcontentsType().
    Q_UNUSED(xml)
    return {};
}

QVariantMap Doxml::parseCompound_tableofcontentsKindType(QXmlStreamReader &xml) const
{
    /// \todo Implement Doxml::parseCompound_tableofcontentsKindType().
    Q_UNUSED(xml)
    return {};
}

QVariantMap Doxml::parseCompound_docEmojiType(QXmlStreamReader &xml) const
{
    /// \todo Implement Doxml::parseCompound_docEmojiType().
    Q_UNUSED(xml)
    return {};
}

QVariantMap Doxml::parseDoxyfile(QXmlStreamReader &xml) const
{
    if (!xml.readNextStartElement()) {
        /// \todo Better standardise these warnings / errors.
        qCWarning(lc).noquote() << QTR("Invalid XML file: %1 - %2").arg(location(xml), xml.errorString());
        return { };
    }
    if (xml.name() != QSL("doxyfile")) {
        xml.raiseError(QTR("Root element is not \"doxyfile\""));
        return { };
    }
    return parseDoxyfile_DoxygenFileType(xml);
}

QVariantMap Doxml::parseDoxyfile_DoxygenFileType(QXmlStreamReader &xml) const
{
    Q_ASSERT(xml.name() == QSL("doxyfile"));

    // Fetch the XML attributes.
    QVariantMap map;
    map.insert(QSL("version"), xml.attributes().value(QSL("version")).toString());
    map.insert(QSL("language"), xml.attributes().value(QSL("xml:lang")).toString());

    // Parse the <option> elements.
    QVariantMap options;
    while ((!xml.atEnd()) && (xml.readNextStartElement())) {
        if (xml.name() == QSL("option")) {
            options.insert(parseDoxyfile_OptionType(xml));
        } else {
            qCWarning(lc).noquote() << QTR("Skipping unknown <%1> element at %2:%3:%4")
                                           .arg(xml.name().toString(), location(xml));
            xml.skipCurrentElement();
        }
    }
    qCDebug(lc).noquote() << QTR("Parsed %1 options(s) from %2").arg(options.size()).arg(currentXmlFilePath);
    map.insert(QSL("options"), options);
    return map;
}

QVariantMap Doxml::parseDoxyfile_OptionType(QXmlStreamReader &xml) const
{
    Q_ASSERT(xml.name() == QSL("option"));
    const auto attributes = xml.attributes();
    const auto id = attributes.value(QSL("id"));
    const auto type = attributes.value(QSL("type"));
    if (type == QSL("int")) {
        return { { id.toString(), xml.readElementText(QXmlStreamReader::IncludeChildElements).toInt() } };
    } else if (type == QSL("bool")) {
        return { { id.toString(), xml.readElementText(QXmlStreamReader::IncludeChildElements) == QSL("YES") } };
    } else if (type == QSL("string")) {
        return { { id.toString(), xml.readElementText(QXmlStreamReader::IncludeChildElements) } };
    } else if (type == QSL("stringlist")) {
        QStringList values;
        while ((!xml.atEnd()) && (xml.readNextStartElement())) {
            if (xml.name() == QSL("value")) {
                values.append(xml.readElementText());
            } else {
                qCWarning(lc).noquote() << QTR("Skipping unknown <%1> element at %2:%3:%4")
                                               .arg(xml.name().toString(), location(xml));
                xml.skipCurrentElement();
            }
        }
        return { { id.toString(), values } };
    }
    qCWarning(lc).noquote() << QTR("Treating option of unknwn type as string");
    return { { id.toString(), xml.readElementText(QXmlStreamReader::IncludeChildElements) } };
}

QVariantMap Doxml::parseIndex(QXmlStreamReader &xml) const
{
    /// \todo Implement Doxml::parseIndex().
    Q_UNUSED(xml)
    return {};
}

QVariantMap Doxml::parseIndex_DoxygenType(QXmlStreamReader &xml) const
{
    /// \todo Implement Doxml::parseIndex_DoxygenType().
    Q_UNUSED(xml)
    return {};
}

QVariantMap Doxml::parseIndex_CompoundType(QXmlStreamReader &xml) const
{
    /// \todo Implement Doxml::parseIndex_CompoundType().
    Q_UNUSED(xml)
    return {};
}

QVariantMap Doxml::parseIndex_MemberType(QXmlStreamReader &xml) const
{
    /// \todo Implement Doxml::parseIndex_MemberType().
    Q_UNUSED(xml)
    return {};
}

} // namespace doxlee
