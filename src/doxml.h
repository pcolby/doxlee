// SPDX-FileCopyrightText: 2021-2024 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include <QDir>
#include <QPair>
#include <QVariantMap>

#ifndef DOXLEE_DOXML_H
#define DOXLEE_DOXML_H

namespace doxlee {

namespace doxml {

QPair<QStringList,QStringList> kinds(const QDir &doxmlDir);
QPair<QStringList,QStringList> kinds(const QString &indexXsdPath);

QVariantMap parseIndex(const QDir &doxmlDir, const bool extraIndexes = true);
QVariantMap parseIndex(const QString &indexXmlPath, const bool extraIndexes = true);

QVariantMap extraIndexes(const QVariantList &compounds);

QVariantMap parseCompound(const QDir &doxmlDir, const QString &refId);
QVariantMap parseCompound(const QString &compoundXmlPath);

} // namespace doxml

} // namespace doxlee

#endif // DOXLEE_DOXML_H
