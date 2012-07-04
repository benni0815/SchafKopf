/*
    This file is part of the KDE games library
    Copyright 2008 Andreas Pakulat <apaku@gmx.de>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License version 2 as published by the Free Software Foundation.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#include "kcarddialog.h"
#include "ui_kgamecardselector.h"

#include <QPainter>
#include <QPixmap>
#include <QListWidgetItem>
#include <QFileInfo>
#include <QDir>

#include <klocale.h>
#include <kstandarddirs.h>
#include <krandom.h>
#include <kdebug.h>

#include "carddeckinfo.h"
#include "carddeckinfo_p.h"


/**
 * Local information of the dialog.
 */
class KCardWidgetPrivate
{
public:

  /** Constructor.
   */
  KCardWidgetPrivate()
  {
  }

  /** Currently chosen front side name.
   */
  QString currentDeck;

  /** The UI elements.
   */
  Ui::KGameCardSelectorBase ui;

};

// Create the dialog from a config group
KCardWidget::KCardWidget(QWidget* parent)
           : QWidget(parent), d(new KCardWidgetPrivate)
{
  // GUI
  setupGUI();
  insertCardIcons();
  setDeckName(CardDeckInfo::defaultDeckName());
}

void KCardWidget::readSettings(const KConfigGroup& group)
{
  setDeckName(CardDeckInfo::deckName(group));
}

// Store the config group settings
void KCardWidget::saveSettings(KConfigGroup& group) const
{
  CardDeckInfo::writeDeckName(group, d->currentDeck );
}


// Setup the user interface
void KCardWidget::setupGUI()
{
  // Inner widget
  Ui::KGameCardSelectorBase* ui = &(d->ui);
  d->ui.setupUi(this);

  // Set lists and preview
  insertCardIcons();

  // Connect signals
  connect(ui->list, SIGNAL(itemSelectionChanged()),
          this, SLOT( updateSelection()));
}


// Destroy the dialog
KCardWidget::~KCardWidget()
{
  delete d;
}

// Retrieve selected card name
QString KCardWidget::deckName() const
{
  return d->currentDeck;
}

// Build list widget
void KCardWidget::insertCardIcons()
{
  // Clear GUI
  d->ui.list->clear();

  // Rebuild list
  QSize itemSize;
  foreach(const QString &name, CardDeckInfo::deckNames())
  {
    KCardThemeInfo v = CardDeckInfo::deckInfo(name);
    // Show only SVG files?
    if (v.svgfile.isEmpty()) continue;

    const int iconSize = 48;
    QPixmap resizedCard = v.preview.scaled(QSize(iconSize, iconSize), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    QPixmap previewPixmap(iconSize, iconSize);
    previewPixmap.fill(Qt::transparent);
    QPainter p(&previewPixmap);
    p.drawPixmap((iconSize-resizedCard.width())/2, (iconSize-resizedCard.height())/2, resizedCard);
    p.end();

    QListWidgetItem *item = new QListWidgetItem(v.name, d->ui.list);
    item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    item->setToolTip(v.name);
    item->setData(Qt::DecorationRole, previewPixmap);
    item->setData(Qt::UserRole, v.noi18Name);
    itemSize = itemSize.expandedTo(previewPixmap.size());
  }

    setDeckName(CardDeckInfo::defaultDeckName());

  d->ui.list->setIconSize(itemSize);
}


// Update front preview
void KCardWidget::updateSelection()
{
  QList<QListWidgetItem*> l = d->ui.list->selectedItems();
  if(!l.isEmpty())
        setDeckName(l.first()->data(Qt::UserRole).toString());
}


// Update front preview

void KCardWidget::setDeckName(const QString& name)
{
  // Clear item?
  if (name.isEmpty())
  {
    QList<QListWidgetItem*> items = d->ui.list->selectedItems();
    if(!items.isEmpty())
        items.first()->setSelected(false);
    d->ui.previewImage->setPixmap(QPixmap());
    d->ui.cardName->setText(QString());
    d->ui.cardDescription->setText(QString());
  }
  else
  {
    for (int i = 0; i < d->ui.list->count(); ++i)
    {
      QListWidgetItem *item = d->ui.list->item(i);
      if (item->data(Qt::UserRole).toString() == name)
      {
        item->setSelected(true);
        d->ui.list->scrollToItem(item);
        break;
      }
    }

    KCardThemeInfo info = CardDeckInfo::deckInfo(name);
    QFont font;
    font.setBold(true);
    d->ui.cardName->setText(info.name);
    d->ui.cardName->setFont(font);

    d->ui.cardDescription->setText(info.comment);
    QPixmap pixmap= info.preview;
    if (pixmap.height() > d->ui.previewImage->height())
      pixmap = pixmap.scaledToHeight(d->ui.previewImage->height(), Qt::SmoothTransformation);
    if (pixmap.width() > d->ui.previewImage->width())
      pixmap = pixmap.scaledToWidth(d->ui.previewImage->width(), Qt::SmoothTransformation);
    d->ui.previewImage->setPixmap(pixmap);
  }
  d->currentDeck = name;
}


KCardDialog::KCardDialog( KCardWidget* widget )
{
  setMainWidget(widget);
  setCaption(i18n("Card Deck Selection"));
  setButtons(KDialog::Ok | KDialog::Cancel);
}

#include "kcarddialog.moc"
