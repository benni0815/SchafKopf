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
#ifndef __CARDDECKINFO_H_
#define __CARDDECKINFO_H_

#include <QtCore/QString>
#include <QtCore/QStringList>

class KConfigGroup;

/**
 * \headerfile carddeckinfo.h <KCardDeckInfo>
 *
 * Namespace to supply access to card deck information, such as a list of all
 * card decks as well as allowing to access the actual files to render the
 * decks.
 */
namespace CardDeckInfo
{
   /** Retrieve the SVG file belonging to the given card deck (back side). 
    * @param name The untranslated name of the back deck.
    * @return The file name and path to the SVG file or QString() if not available. 
    */
   QString backSVGFilePath(const QString& name);

   /** Retrieve the SVG file belonging to the given card set (front side). 
    * The SVG IDs used for the card back is '1_club' for Ace of clubs, '10_spade' for
    * 10 of spades, 'queen_heart' for Queen of Hearts, '2_diamond' for 2 of diamonds and
    * so on.
    * @param name The untranslated name of the card set.
    * @return The file name and path to the SVG file or QString() if not available. 
    */
   QString svgFilePath(const QString& name);

   /** Retrieve the untranslated name of the default card set (front side).
    * @return The default card set name.
    */
   QString defaultDeckName();

   /** Retrieve the untranslated name of the default card deck (back side).
    * @return The default card deck name.
    */
   QString defaultBackName();

   /** Retrieve a untranslated name random card set (front side).
    * @return A radnom card set name.
    */
   QString randomDeckName();

   /** Retrieve a untranslated name random card deck (back side).
    * @return A radnom card deck name.
    */
   QString randomBackName();

   /**
    * Retrieve the filename of the card back side. 
    * For SVG decks use @ref backSVGFilePath.
    * @param name The untranslated name of the card deck.
    * @return The filename.
    */
   QString backFilename(const QString& name);

   /**
    * retrieve a list of the untranslated names of all installed backsides
    * @returns a list of backside names, which can be 
    * used as input to the other functions.
    */
   QStringList backNames();

   /**
    * retrieve a list of the untranslated names of all installed frontsides
    * @return a list of frontside names, which can be
    * used as input to the other functions.
    */
   QStringList deckNames();

   /**
    * retrieve the configured front side untranslated theme name
    * from the @p group
    * @param group the KConfigGroup to read from
    * @param default the default theme to return if the config group has no setting for this
    * @returns the name of the front side theme name
    */
   QString deckName( const KConfigGroup& group, const QString& defaultTheme = defaultDeckName() );

   /**
    * retrieve the configured back side untranslated theme name
    * from the @p group
    * @param group the KConfigGroup to read from
    * @param default the default theme to return if the config group has no setting for this
    * @returns the name of the back side theme name
    */
   QString backTheme( const KConfigGroup& group, const QString& defaultTheme = defaultBackName() );

   /**
    * retrieve the current value for the lock front-to-backside
    * option from the @p group
    * @param group the KConfigGroup to read from
    * @param lockDefault the default value in case the group has no setting
    * @returns true when front and backside theme are locked together, else false
    */
   bool lockFrontToBackside( const KConfigGroup& group, bool lockDefault = true );

   /**
    * store the given frontside @p theme name in the @p group
    * @param group the KConfigGroup to write to from
    * @param theme the theme untranslated name to store
    */
   void writeDeckName( KConfigGroup& group, const QString& theme );

   /**
    * store the given backside @p theme name in the @p group
    * @param group the KConfigGroup to write to from
    * @param theme the theme untranslated name to store
    */
   void writeBackTheme( KConfigGroup& group, const QString& theme );

   /**
    * store the whether front and backside theme selection is locked to the @p group
    * @param group the KConfigGroup to write to from
    * @param allowFixedSize whether front and backside theme selection is locked
    */
   void writeLockFrontToBackside( KConfigGroup& group, bool lock );
}

#endif
