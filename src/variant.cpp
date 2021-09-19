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

#include "variant.h"

#include <QCoreApplication>
#include <QDebug>
#include <QXmlStreamReader>

/// Shorten QCoreApplication::translate calls for readability.
#define QTR(str) QCoreApplication::translate("toVariant", str)

namespace doxlee {

void sortBy(QVariantList &list, const QString &key)
{
    std::sort(list.begin(), list.end(),
        [&key](const QVariant &a, const QVariant &b) {
            return a.toMap().value(key).toString() < b.toMap().value(key).toString();
        });
}

QVariantMap toVariant(const QHash<QString,QVariantList> &hash)
{
    QVariantMap map;
    for (auto iter = hash.begin(); iter != hash.end(); ++iter) {
        map.insert(iter.key(), iter.value());
    }
    return map;
}

/// Borrowed from (my own old) gist: https://gist.github.com/pcolby/6558910
QVariantMap toVariant(QXmlStreamReader &xml, const QString &prefix, const int maxDepth)
{
    if (maxDepth < 0) {
        qWarning().noquote() << QTR("max depth exceeded");
        return QVariantMap();
    }

    if (xml.hasError()) {
        qWarning().noquote() << xml.errorString();
        return QVariantMap();
    }

    if (xml.tokenType() == QXmlStreamReader::NoToken)
        xml.readNext();

    if ((xml.tokenType() != QXmlStreamReader::StartDocument) &&
        (xml.tokenType() != QXmlStreamReader::StartElement)) {
        qWarning().noquote() << QTR("unexpected XML tokenType %1 (%2)")
                                .arg(xml.tokenString()).arg(xml.tokenType());
        return QVariantMap();
    }

    QMultiMap<QString, QVariant> map;
    if (xml.tokenType() == QXmlStreamReader::StartDocument) {
        map.insert(prefix + QLatin1String("DocumentEncoding"), xml.documentEncoding().toString());
        map.insert(prefix + QLatin1String("DocumentVersion"), xml.documentVersion().toString());
        map.insert(prefix + QLatin1String("StandaloneDocument"), xml.isStandaloneDocument());
    } else {
        if (!xml.namespaceUri().isEmpty())
            map.insert(prefix + QLatin1String("NamespaceUri"), xml.namespaceUri().toString());
        foreach (const QXmlStreamAttribute &attribute, xml.attributes()) {
            QVariantMap attributeMap;
            attributeMap.insert(QLatin1String("Value"), attribute.value().toString());
            if (!attribute.namespaceUri().isEmpty())
                attributeMap.insert(QLatin1String("NamespaceUri"), attribute.namespaceUri().toString());
            if (!attribute.prefix().isEmpty())
                attributeMap.insert(QLatin1String("Prefix"), attribute.prefix().toString());
            attributeMap.insert(QLatin1String("QualifiedName"), attribute.qualifiedName().toString());
            map.insert(prefix + attribute.name().toString(), attributeMap);
        }
    }

    for (xml.readNext(); (!xml.atEnd()) && (xml.tokenType() != QXmlStreamReader::EndElement)
          && (xml.tokenType() != QXmlStreamReader::EndDocument); xml.readNext()) {
        switch (xml.tokenType()) {
        case QXmlStreamReader::Characters:
        case QXmlStreamReader::Comment:
        case QXmlStreamReader::DTD:
        case QXmlStreamReader::EntityReference:
            map.insert(prefix + xml.tokenString(), xml.text().toString());
            break;
        case QXmlStreamReader::ProcessingInstruction:
            map.insert(prefix + xml.processingInstructionTarget().toString(),
                            xml.processingInstructionData().toString());
            break;
        case QXmlStreamReader::StartElement:
            map.insert(xml.name().toString(), toVariant(xml, prefix, maxDepth-1));
            break;
        default:
            qWarning() << QTR("unexpected XML tokenType %1 (%2)")
                          .arg(xml.tokenString()).arg(xml.tokenType());
        }
    }
    return QVariantMap(map);
}

} // namespace doxlee
