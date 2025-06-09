// SPDX-FileCopyrightText: Copyright 2004 by Dominik Seichter <domseichter@web.de>
// SPDX-License-Identifier: GPL-2.0-or-later

#include "schafkopf.h"

#include <KLocalizedString>

#include <QApplication>


int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    KLocalizedString::setApplicationDomain(QByteArrayLiteral("schafkopf"));
    app.setQuitOnLastWindowClosed(true);

    auto mainWin = new SchafKopf();
    mainWin->show();

    return app.exec();
}
