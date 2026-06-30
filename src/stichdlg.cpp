// SPDX-FileCopyrightText: Copyright 2004 by Dominik Seichter <domseichter@web.de>
// SPDX-License-Identifier: GPL-2.0-or-later

#include "stichdlg.h"

#include "canvascard.h"

#include <KLocalizedString>

#include <QLabel>
#include <QLayout>
#include <QGridLayout>
#include <QPushButton>


StichDlg::StichDlg(QWidget *parent )
    : QDialog( parent )
{
    setWindowTitle(i18n("Last Trick"));
    QGridLayout* layout = new QGridLayout( this );
    trick = new QLabel();
    layout->addWidget( trick, 0, 0, 1, 4);

    for( unsigned int i = 0; i < PLAYERS; i++ )
    {
        cards[i] = new QLabel();
        layout->addWidget( cards[i], 1, i );

        players[i] = new QLabel();
        layout->addWidget( players[i], 2, i );
    }

    QPushButton* close = new QPushButton(i18n("&Close"));
    layout->addWidget( close, 3, 0, 1, 4, Qt::AlignRight );
    connect(close, &QPushButton::clicked, this, &StichDlg::close);

    reset();
}

void StichDlg::changed(const QString& name, const std::vector<Card::Id>& cardIds, const QStringList& playerlist)
{
    if (cardIds.empty())
        trick->setText(i18n("No trick was made."));
    else
      trick->setText(i18n("Trick was made by: <b>") + name + "</b>");

    for (size_t i = 0; i < PLAYERS; i++ )
    {
        if (cardIds.size() > i)
        {
            const auto id = cardIds.at(cardIds.size() - PLAYERS + i);
            cards[i]->setPixmap(CanvasCard::frontsidePixmap(id));
            players[i]->setText("<qt><b>" + playerlist.at(i) + "</b></qt>");
        }
        else
        {
            cards[i]->setPixmap(CanvasCard::backsidePixmap());
            players[i]->setText( QString() );
        }
    }
}

void StichDlg::reset()
{
  changed({}, {}, {});
}
