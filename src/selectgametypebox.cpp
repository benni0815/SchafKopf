// SPDX-FileCopyrightText: Copyright 2004 by Dominik Seichter <domseichter@web.de>
// SPDX-License-Identifier: GPL-2.0-or-later

#include "selectgametypebox.h"

#include "canvascard.h"
#include "cards.h"
#include "gameinfo.h"
#include "settings.h"

#include <KLocalizedString>

#include <QButtonGroup>
#include <QGroupBox>
#include <QLabel>
#include <QPainter>
#include <QRadioButton>
#include <QVBoxLayout>


SelectGameTypeBox::SelectGameTypeBox(QWidget *parent)
: QWizardPage(parent)
{
  setTitle(i18n("Step 1/2: Select Game"));

  QWidget *row1 = new QWidget;

  preview = new QLabel;
  infoLabel = new QLabel;
  infoLabel->setMargin(11);
  infoLabel->setAlignment(Qt::AlignBottom | Qt::AlignLeft);
  infoLabel->setWordWrap(true);
  infoLabel->setMinimumHeight(100);

  checkRufspiel = new QRadioButton(i18n("&Callgame"));
  checkSolo = new QRadioButton(i18n("&Solo"));
  checkGeier = new QRadioButton(i18n("&Geier"));
  checkWenz = new QRadioButton(i18n("&Wenz"));
  checkDachs = new QRadioButton(i18n("&Dachs"));

  checkRufspiel->setChecked(true);

  const AllowedGames allowed = Settings::instance()->allowedGames();
  checkGeier->setEnabled(allowed.geier);
  checkWenz->setEnabled(allowed.wenz);
  checkDachs->setEnabled(allowed.dachs);

  QButtonGroup *myGroup = new QButtonGroup;
  myGroup->addButton(checkRufspiel);
  myGroup->addButton(checkSolo);
  myGroup->addButton(checkGeier);
  myGroup->addButton(checkWenz);
  myGroup->addButton(checkDachs);
  connect(myGroup, &QButtonGroup::buttonClicked, this, &SelectGameTypeBox::typeChanged);

  QGroupBox *group_TypeSelect = new QGroupBox(i18n("Game"));
  QVBoxLayout *button_layout = new QVBoxLayout;
  button_layout->addWidget(checkRufspiel);
  button_layout->addWidget(checkSolo);
  button_layout->addWidget(checkGeier);
  button_layout->addWidget(checkWenz);
  button_layout->addWidget(checkDachs);
  group_TypeSelect->setLayout(button_layout);

  QVBoxLayout *row_layout = new QVBoxLayout;
  row_layout->addWidget(preview);
  row_layout->addWidget(infoLabel);
  row1->setLayout(row_layout);

  QHBoxLayout *layout = new QHBoxLayout;
  layout->setSpacing(8);
  layout->addWidget(row1);
  layout->addWidget(group_TypeSelect);
  this->setLayout(layout);

  typeChanged();
  infoLabel->setMaximumWidth(preview->size().width());
}

void SelectGameTypeBox::typeChanged()
{
  const auto HelpGametypeSolo = i18n("When you play a \"Solo\", you have to additionally choose a color. "
                                     "Trumps are all \"Ober\", \"Unter\" and the color you chose. "
                                     "You will play alone against the other three players.");
  const auto HelpGametypeDachs = i18n("When you play a \"Dachs\", the only trumps are aces and tenners. You will play "
                                      "alone against the other three players.");
  const auto HelpGametypeRufspiel = i18n("When you play a callgame, you will have to choose a color. "
                                         "You will then play in team with the player which has the ace of the color you chose. "
                                         "Hearts are trumps.");
  const auto HelpGametypeWenz = i18n("When you play a \"Wenz\", you can choose a color. If you don't choose a color, "
                                     "only all \"Unter\" will be trumps. If you do, the cards of the chosen color will "
                                     "also be trumps. You will play alone against the other three players.");
  const auto HelpGametypeGeier = i18n("When you play a \"Geier\", you can choose a color. If you don't choose a color, "
                                      "only all \"Ober\" will be trumps. If you do, the cards of the chosen color will "
                                      "also be trumps. You will play alone against the other three players.");

  const auto mode = gameMode();
  updatePreview();
  switch (mode)
  {
  case (GameVariant::RUFSPIEL):
    setInfoText(HelpGametypeRufspiel);
    break;
  case (GameVariant::WENZ):
    setInfoText(HelpGametypeWenz);
    break;
  case (GameVariant::STICHT):
    setInfoText(HelpGametypeSolo);
    break;
  case (GameVariant::GEIER):
    setInfoText(HelpGametypeGeier);
    break;
  case (GameVariant::DACHS):
    setInfoText(HelpGametypeDachs);
    break;
  // DS: Fix a compiler warning
  default:
    break;
  }
}

void SelectGameTypeBox::setInfoText(const QString &Text)
{
  infoLabel->setText(Text);
}

void SelectGameTypeBox::updatePreview()
{
  const auto info = GameInfo({gameMode(), CardId::NoColor});

  std::vector<CardId> trumpf;
  for (const auto card : Cards::initIds())
  {
    if (info.istTrumpf(card))
    {
      trumpf.push_back(card);
    }
  }

  std::sort(std::begin(trumpf), std::end(trumpf), [&] (CardId lhs, CardId rhs) { return info.evalCard(lhs) < info.evalCard(rhs); });

  const auto cardSize = CanvasCard::frontsidePixmap({CardId::Sau, CardId::Eichel}).size();
  QPixmap pix(cardSize.width() + (cardSize.width() / 2) * 7,
              cardSize.height() * (int(trumpf.size() / 4) + (trumpf.size() % 4)));
  int x = cardSize.width() / 2 * (trumpf.size() - 1);
  pix.fill(Qt::darkGreen);
  QPainter p(&pix);
  int y = 0;
  for (size_t i = 0; i <= trumpf.size() - 1; ++i)
  {
    const auto pixmap = CanvasCard::frontsidePixmap(trumpf.at(i));
    p.drawPixmap(x, y, pixmap);
    x -= pixmap.width() / 2;

    int pos = trumpf.size() + i;
    if (!(pos + 1) % 4)
    {
      x = 0;
      y += pixmap.height();
    }
  }

  p.end();
  pix = pix.copy(0, 0, pix.width(), y + (x == 0 ? 0 : cardSize.height()));
  preview->setPixmap(pix);
  preview->setMinimumWidth(pix.width() + 10);
}

GameVariant::Mode SelectGameTypeBox::gameMode() const
{
  if (checkRufspiel->isChecked()) { return GameVariant::Mode::RUFSPIEL; }
  else if (checkSolo->isChecked()) { return GameVariant::Mode::STICHT; }
  else if (checkGeier->isChecked()) { return GameVariant::Mode::GEIER; }
  else if (checkWenz->isChecked()) { return GameVariant::Mode::WENZ; }
  else if (checkDachs->isChecked()) { return GameVariant::Mode::DACHS; }
  return GameVariant::Mode::RAMSCH;
}
