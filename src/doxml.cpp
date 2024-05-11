// SPDX-FileCopyrightText: 2021-2024 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "doxml.h"

#include "variant.h"

#include <QCoreApplication>
#include <QDebug>
#include <QJsonDocument> // \todo Remove; only for early dev / debugging.
#include <QLoggingCategory>
#include <QXmlStreamReader>

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

} // namespace doxlee
