/*
    This file is part of the KDE games library
    SPDX-FileCopyrightText: 2008 Andreas Pakulat <apaku@gmx.de>

    SPDX-License-Identifier: LGPL-2.0-only
*/

#ifndef KCARDDIALOG_H
#define KCARDDIALOG_H

#include <QDialog>
#include <QWidget>

#include <KConfig>

class KCardWidgetPrivate;
class KCardWidget;

/**
 * \class KCardDialog kcarddialog.h <KCardDialog>
 *
 * @short A convenience class to display a standalone card selection dialog.
 *
 * This is a simple convenience class to embed the @ref KCardWidget into a
 * QDialog that has an OK and Cancel button and an appropriate caption.
 *
 * Usage Example:
 * \code
 *   KConfigGroup(KSharedConfig::openConfig(),"CardOptions");
 *   KCardWidget *cardwiget = new KCardwidget();
 *   cardwidget->readSettings(configGroup);
 *   KCardDialog dlg(cardwidget);
 *   if (dlg.exec() == QDialog::Accepted)
 *   {
 *     cardwidget->saveSettings(configGroup);
 *     configGroup.sync();
 *     //now update the graphics
 *   }
 * \endcode
 *
 */
class KCardDialog : public QDialog
{
    Q_OBJECT
public:
    explicit KCardDialog(KCardWidget *widget);
};

/**
 * \class KCardWidget kcarddialog.h <KCardDialog>
 *
 * @short A card deck selection widget for card games.
 *
 * The KCardWidget provides a widget for interactive card deck selection.
 * It gives card games an easy to use interface to select front and
 * back of the card sets. As card sets the KDE default card sets are
 * offered as well as used specified ones.
 *
 * This class can be used in two ways: Embedding it into an existing
 * dialog or creating a small QDialog just for the card deck selection.
 *
 * Card sets (front and back) are identified by their (translated) names.
 *
 * You can use a KConfigGroup to initialize the state of the widget or
 * let the widget store its current state to a config group.
 *
 * For an example usage see @ref KCardDialog.
 *
 */
class KCardWidget : public QWidget
{
    Q_OBJECT

public:
    /**
     * Constructs a card deck selection widget.
     *
     * @param parent The parent widget of the widget, if any.
     */
    explicit KCardWidget (QWidget *parent = nullptr);

    /**
     * Read the settings from a config file
     * @param group the configuration group
     */
    void readSettings(const KConfigGroup &group);

    /**
     * Destructs a card deck selection dialog.
     */
    ~KCardWidget() override;

    /**
     * Saves the KCardWidget config into a config file.
     * These settings are used by @ref KCardWidget.
     */
    void saveSettings(KConfigGroup &group) const;

    /**
     * set the name of the card set (front side)
     * @param name the new name to select as front side
     */
    void setDeckName(const QString &name);

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
    /** The dialog data. */
    KCardWidgetPrivate *const d;
};

#endif
