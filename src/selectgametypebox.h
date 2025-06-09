// SPDX-FileCopyrightText: Copyright 2004 by Dominik Seichter <domseichter@web.de>
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <QWizardPage>


class QWidget;
class QLineEdit;
class QLabel;
class QRadioButton;
class GameInfo;


/**
@author Christian Kern
*/
class SelectGameTypeBox : public QWizardPage
{
    Q_OBJECT
public:
    SelectGameTypeBox(QWidget *parent = 0 );

    ~SelectGameTypeBox() = default;
    GameInfo* gameInfo() const;

protected:
    QRadioButton* checkRufspiel;
    QRadioButton* checkSolo;
    QRadioButton* checkGeier;
    QRadioButton* checkWenz;
    QRadioButton* checkDachs;
    QLabel* preview;
    QLabel* infoLabel;

    void setInfoText(QString Text);

protected slots:
    void updatePreview();
    void typeChanged();

};
