
inherit kde
need-kde 3

ECVS_SERVER="cvs.berlios.de:/cvsroot/schafkopf"
ECVS_MODULE="schafkopf"

LICENSE="GPL-2"
DESCRIPTION="CVS-Version of the famous KDE game SchafKopf."
HOMEPAGE="http://schafkopf.berlios.de"
KEYWORDS="~x86"
SLOT="0"

DEPEND="|| ( kde-base/libkdegames >=kde-base/kdegames-3.2 )"
RDEPEND="|| ( kde-base/libkdegames >=kde-base/kdegames-3.2 )"

inherit cvs

S="${WORKDIR}/${ECVS_MODULE}"

src_compile() {
	PREFIX=`kde-config --prefix`
	myconf="$myconf --prefix=$PREFIX"

	emake -f Makefile.cvs
	
	kde_src_compile $myconf configure
    kde_src_compile make
}

src_install() {
	kde_src_install
	dodoc TODO README ChangeLog COPYING AUTHORS
}

