# Copyright 1999-2005 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2

inherit kde

DESCRIPTION="The KDE version of a popular Bavarian card game."
HOMEPAGE="http://schafkopf.berlios.de"
SRC_URI="http://download.berlios.de/schafkopf/${P}.tar.gz"

LICENSE="GPL-2"
SLOT=0
KEYWORDS="~amd64 ~x86"
IUSE=""

DEPEND="|| ( kde-base/libkdegames >=kde-base/kdegames-3.2.0 )"

need-kde 3
