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
#ifndef __KCARDDIALOG_H_
#define __KCARDDIALOG_H_

#include <QtGui/QWidget>
#include <kdialog.h>
#include <kconfig.h>

#include <libkdegames_export.h>

class QListWidgetItem;
class KCardWidgetPrivate;
class KCardWidget;

/**
 * \class KCardDialog kcarddialog.h <KCardDialog>
 * 
 * @short A convenience class to display a standalone card selection dialog.
 *
 * This is a simple convenience class to embed the @ref KCardWidget into a
 * KDialog that has an Ok and Cancel button and an appropriate caption.
 *
 * Usage Example:
 * \code
 *   KConfigGroup(KGlobal::config(),"CardOptions");
 *   KCardWidget* cardwiget = new KCardwidget();
 *   cardwidget->readSettings(configGroup);
 *   KCardDialog dlg(cardwidget);
 *   if(dlg.exec() == QDialog::Accepted)
 *   {
 *     cardwidget->saveSettings(configGroup);
 *     configGroup.sync();
 *     //now update the graphics
 *   }
 * \endcode
 *
 */
class KDEGAMES_EXPORT KCardDialog : public KDialog
{
  Q_OBJECT
public:
  KCardDialog(KCardWidget* widget);
};

/**
 * \class KCardWidget kcarddialog.h <KCardDialog>
 * 
 * @short A carddeck selection widget for card games.
 *
 * The KCardWidget provides a widget for interactive carddeck selection.
 * It gives cardgames an easy to use interface to select front and
 * back of the card sets. As card sets the KDE default cardsets are
 * offered as well as used specified ones.
 *
 * This class can be used in two ways: Embedding it into an existing
 * dialog or creating a small KDialog just for the carddeck selection.
 *
 * Card sets (front and back) are identified by their (translated) names.
 *
 * You can use a KConfigGroup to initialize the state of the widget or
 * let the widget store its current state to a config group.
 *
 * For an example usage see @ref KCardDialog.
 *
 */
class KDEGAMES_EXPORT KCardWidget : public QWidget
{
  Q_OBJECT

public:

   /**
    * Constructs a card deck selection widget.
    *
    * @param parent The parent widget of the widget, if any.
    */
   KCardWidget (QWidget* parent = NULL);

   /**
    * Read the settings from a config file
    * @param group the configuration group
    */
   void readSettings(const KConfigGroup& group);

   /**
    * Destructs a card deck selection dialog.
    */
   ~KCardWidget();

   /**
    * Saves the KCardWidget config into a config file.
    * These settings are used by @ref KCardWidget.
    */
   void saveSettings(KConfigGroup& group) const;

   /**
    * set the name of the card set (front side)
    * @param name the new name to select as front side
    */
   void setDeckName(const QString& name);

   /**
    * Retrieve the name of the card set (front side) from the dialog.
    * @return The card set name.
    */
   QString deckName() const;

protected:
    void insertCardIcons();

    /**
     * Configure the dialog GUI.
     */
    void setupGUI();

protected Q_SLOTS:
    /**
     * Called by the card set list view when a new item was selected.
     */
    void updateSelection();

private:
   /**
    * The dialog data.
    */
   KCardWidgetPrivate* const d;
};

#endif
