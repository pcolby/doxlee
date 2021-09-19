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

} // namespace doxml

} // namespace doxlee

#endif // DOXLEE_DOXML_H
