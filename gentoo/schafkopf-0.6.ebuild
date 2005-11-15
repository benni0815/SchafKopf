# Copyright 1999-2005 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# $Header: /cvsroot/schafkopf/schafkopf/gentoo/schafkopf-0.6.ebuild,v 1.1 2005-11-15 12:03:29 bloewe Exp $

# we are a kde-ebuild
inherit kde
need-kde 3

DESCRIPTION="SchafKopf is a KDE version of a popular Bavarian card game."
SRC_URI="http://download.berlios.de/schafkopf/schafkopf-${PV}.tar.gz"
HOMEPAGE="http://schafkopf.berlios.de"

# we need kdegames
DEPEND="|| ( kde-base/libkdegames >=kde-base/kdegames-3.2 )"
RDEPEND="|| ( kde-base/libkdegames >=kde-base/kdegames-3.2 )"


# kde is required ofcourse
IUSE="X kde"

# only install one instance of SchafKopf
SLOT=0
LICENSE=GPL-2

# we develop on amd64 and x86 so both can be considered stable
KEYWORDS="amd64 x86" 

src_compile() {
        PREFIX="`kde-config --prefix`"
        kde_src_compile configure make || die
}
