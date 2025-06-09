// SPDX-FileCopyrightText: Copyright 2004 by Dominik Seichter <domseichter@web.de>
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "schafkopfdef.h"

#include <QDialog>


class QLabel;

/**
Show the last trick which was made

@author Dominik Seichter
*/
class StichDlg : public QDialog
{
    Q_OBJECT
    public:
        StichDlg(QWidget *parent = 0 );
        ~StichDlg();
        
        void changed( const QString & name, int* list, QStringList* players );
        
    private:
        void reset();
        
    private:
        QLabel* trick;
        QLabel* cards[PLAYERS];
        QLabel* players[PLAYERS];
};
