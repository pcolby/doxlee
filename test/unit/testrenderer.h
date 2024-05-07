// SPDX-FileCopyrightText: 2021-2024 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include <QtTest>

class TestRenderer: public QObject {
    Q_OBJECT

private slots:
    void compoundPathName_data();
    void compoundPathName();

    void getKindFromFileName_data();
    void getKindFromFileName();

};
