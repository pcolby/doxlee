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

#ifndef DOXLEE_VARIANT_H
#define DOXLEE_VARIANT_H

#include <QHash>
#include <QVariantMap>
#include <QXmlStreamReader>

namespace doxlee {

QVariantMap toVariant(const QHash<QString,QVariantList> &hash);

QVariantMap toVariant(QXmlStreamReader &xml, const QString &prefix=QStringLiteral("."),
                      const int maxDepth=1024);

} // namespace doxlee

#endif // DOXLEE_VARIANT_H
