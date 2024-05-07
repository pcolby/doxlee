// SPDX-FileCopyrightText: 2021-2024 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef DOXLEE_VARIANT_H
#define DOXLEE_VARIANT_H

#include <QHash>
#include <QVariantMap>
#include <QXmlStreamReader>

namespace doxlee {

void sortBy(QVariantList &list, const QString &key);

QVariantMap toVariant(const QHash<QString,QVariantList> &hash);

QVariantMap toVariant(QXmlStreamReader &xml, const QString &prefix=QStringLiteral("."),
                      const int maxDepth=1024);

} // namespace doxlee

#endif // DOXLEE_VARIANT_H
