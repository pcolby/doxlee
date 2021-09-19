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

#include "doxml.h"

#include "variant.h"

#include <QCoreApplication>
#include <QDebug>
#include <QXmlStreamReader>

/// Shorten the QStringLiteral macro for readability.
#define QSL(str) QStringLiteral(str)

/// Shorten QCoreApplication::translate calls for readability.
#define QTR(str) QCoreApplication::translate("doxml", str)

namespace doxlee {

namespace doxml {

QPair<QStringList,QStringList> kinds(const QDir &doxmlDir)
{
    return kinds(doxmlDir.absoluteFilePath(QSL("index.xsd")));
}

QPair<QStringList,QStringList> kinds(const QString &indexXsdPath)
{
    // Open the file for reading.
    QFile file(indexXsdPath);
    if (!file.open(QIODevice::ReadOnly|QIODevice::Text)) {
        qWarning().noquote() << QTR("Error opening file for reading: %1").arg(indexXsdPath);
        return { };
    }

    QStringList compoundKinds, memberKinds;

    // Parse the opening <schema> element.
    QXmlStreamReader xml(&file);
    if (!xml.readNextStartElement()) {
        qWarning().noquote() << QTR("Invalid XML file: %1 - %2").arg(indexXsdPath, xml.errorString());
        return { };
    }
    if (xml.name() != QSL("schema")) {
        qWarning().noquote() << QTR("File is not a Doxygen XML index schema: %1 - %2")
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
    qInfo().noquote() << QTR("Parsed %1 compound kind(s), and %2 member kind(s) from %3")
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
        qWarning().noquote() << QTR("Error opening file for reading: %1").arg(indexXmlPath);
        return QVariantMap();
    }

    // Parse the opening <doxygenindex> element.
    QXmlStreamReader xml(&file);
    if (!xml.readNextStartElement()) {
        qWarning().noquote() << QTR("Invalid XML file: %1 - %2").arg(indexXmlPath, xml.errorString());
        return QVariantMap();
    }
    qDebug() << xml.name() << "version" << xml.attributes().value(QSL("version"))
             << xml.attributes().value(QSL("xml:lang")).toString();
    if (xml.name() != QSL("doxygenindex")) {
        qWarning().noquote() << QTR("File is not a Doxygen XML index: %1 - %2")
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
                qWarning().noquote() << QTR(" %1:%2:%3 <compound> does not begin with <name>")
                    .arg(indexXmlPath).arg(xml.lineNumber()).arg(xml.columnNumber());
                return QVariantMap();
            }
            compound.insert(QSL("name"), xml.readElementText());
            //qDebug() << __func__ << "compound" << compound;
            QVariantList members;
            while ((!xml.atEnd()) && (xml.readNextStartElement())) {
                if (xml.name() == QSL("member")) {
                    QVariantMap member;
                    member.insert(QSL("refid"), xml.attributes().value(QSL("refid")).toString());
                    member.insert(QSL("kind"), xml.attributes().value(QSL("kind")).toString());
                    if ((!xml.readNextStartElement()) || (xml.name() != QSL("name"))) {
                        qWarning().noquote() << QTR("%1:%2:%3 <member> does not begin with <name>")
                            .arg(indexXmlPath).arg(xml.lineNumber()).arg(xml.columnNumber());
                        return QVariantMap();
                    }
                    member.insert(QSL("name"), xml.readElementText());
                    //qDebug() << __func__ << "member" << member;
                    members.append(member);
                    xml.skipCurrentElement();
                } else {
                    qWarning().noquote() << QTR("Skipping unknown <%1> element at %2:%3:%4")
                        .arg(xml.name().toString(), indexXmlPath).arg(xml.lineNumber()).arg(xml.columnNumber());
                    xml.skipCurrentElement();
                }
            }
            compound.insert(QSL("members"), members);
            compounds.append(compound);
        } else {
            qWarning().noquote() << QTR("Skipping unknown <%1> element at %2:%3:%4")
                .arg(xml.name().toString(), indexXmlPath).arg(xml.lineNumber()).arg(xml.columnNumber());
            xml.skipCurrentElement();
        }
    }
    qInfo().noquote() << QTR("Parsed %1 compound(s) from %2").arg(compounds.size()).arg(indexXmlPath);
    indexMap.insert(QSL("compoundsList"), compounds);
    if (extraIndexes) indexMap.insert(doxml::extraIndexes(compounds));
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
    QFile file(compoundXmlPath);
    if (!file.open(QIODevice::ReadOnly|QIODevice::Text)) {
        qWarning().noquote() << QTR("Error opening file for reading: %1").arg(compoundXmlPath);
        return QVariantMap();
    }
    QXmlStreamReader xml(&file);

    /// \todo Transform map to our desired structure
    const QVariantMap map = toVariant(xml);

    const QVariantMap compoundDefinition = map.value(QSL("doxygen")).toMap()
        .value(QSL("compounddef")).toMap();
    if (compoundDefinition.isEmpty()) {
        qWarning().noquote() << QTR("Error reading compond defintion: %1").arg(compoundXmlPath);
        return QVariantMap();
    }

    return compoundDefinition;
}

} // namespace doxml

} // namespace doxlee
