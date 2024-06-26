// SPDX-FileCopyrightText: 2021-2024 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "variant.h"

#include <QCoreApplication>
#include <QDebug>
#include <QLoggingCategory>
#include <QXmlStreamReader>

/// Shorten QCoreApplication::translate calls for readability.
#define QTR(str) QCoreApplication::translate("toVariant", str)

namespace doxlee {

static Q_LOGGING_CATEGORY(lc, "doxlee.variant", QtInfoMsg);

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
        qCWarning(lc).noquote() << QTR("max depth exceeded");
        return QVariantMap();
    }

    if (xml.hasError()) {
        qCWarning(lc).noquote() << xml.errorString();
        return QVariantMap();
    }

    if (xml.tokenType() == QXmlStreamReader::NoToken)
        xml.readNext();

    if ((xml.tokenType() != QXmlStreamReader::StartDocument) &&
        (xml.tokenType() != QXmlStreamReader::StartElement)) {
        qCWarning(lc).noquote() << QTR("unexpected XML tokenType %1 (%2)").arg(xml.tokenString()).arg(xml.tokenType());
        return QVariantMap();
    }

    QMap<QString, QVariant> map;
    if (xml.tokenType() == QXmlStreamReader::StartDocument) {
        map.insert(prefix + QLatin1String("DocumentEncoding"), xml.documentEncoding().toString());
        map.insert(prefix + QLatin1String("DocumentVersion"), xml.documentVersion().toString());
        map.insert(prefix + QLatin1String("StandaloneDocument"), xml.isStandaloneDocument());
    } else {
        if (!xml.namespaceUri().isEmpty())
            map.insert(prefix + QLatin1String("NamespaceUri"), xml.namespaceUri().toString());
        const auto attributes = xml.attributes();
        for (const QXmlStreamAttribute &attribute: attributes) {
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
            qCWarning(lc).noquote() << QTR("unexpected XML tokenType %1 (%2)")
                .arg(xml.tokenString()).arg(xml.tokenType());
        }
    }
    return QVariantMap(map);
}

} // namespace doxlee
