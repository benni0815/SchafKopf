# set the include path for X, qt and KDE
INCLUDES = $(all_includes)

# these are the headers for your project
noinst_HEADERS = schafkopf.h card.h cardlist.h player.h game.h settings.h computerplayer.h humanplayer.h gamecanvas.h canvascard.h timer.h stichdlg.h gameinfo.h

# let automoc handle all of the meta source files (moc)
METASOURCES = AUTO

messages: rc.cpp
	$(XGETTEXT) *.cpp -o $(podir)/schafkopf.pot

KDE_ICON = schafkopf

#########################################################################
# APPLICATION SECTION
#########################################################################
# this is the program that gets installed.  it's name is used for all
# of the other Makefile.am variables
bin_PROGRAMS = schafkopf

# the application source, library search path, and link libraries
schafkopf_SOURCES = main.cpp schafkopf.cpp card.cpp cardlist.cpp player.cpp game.cpp settings.cpp computerplayer.cpp humanplayer.cpp gamecanvas.cpp canvascard.cpp timer.cpp stichdlg.cpp gameinfo.cpp
schafkopf_LDFLAGS = $(KDE_RPATH) $(all_libraries)
schafkopf_LDADD = -lkdegames $(LIB_KDEUI)

# this is where the desktop file will go 
shelldesktopdir   = $(kde_appsdir)/Utilities
shelldesktop_DATA = schafkopf.desktop

# this is where the shell's XML-GUI resource file goes
shellrcdir   = $(kde_datadir)/schafkopf


