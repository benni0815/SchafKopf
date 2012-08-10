#!/bin/bash

## copy all neccessary files for open tarock in the right places

## FIXME: include in Makefile so this is done in make install
##        don't know cmake so cannot do this

WD=/usr/share/kde4/apps/carddecks

## copy background
sudo cp decks/bavarian_tux2.svgz $WD/decks
sudo cp decks/bavarian_tux2.desktop $WD/decks

## copy decks
sudo cp -r svg-OpenTarock $WD