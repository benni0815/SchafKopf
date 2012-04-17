# ########## Project setup ##########
PROJECT(schafkopf)
CMAKE_MINIMUM_REQUIRED(VERSION 2.4.5)

# ######### General setup ##########
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR})

FIND_PACKAGE(KDE4 REQUIRED)
FIND_PACKAGE(LibKDEGames REQUIRED)
FIND_PACKAGE(Qt4 REQUIRED)
FIND_PACKAGE(QImageBlitz REQUIRED)

add_definitions(${QT_DEFINITIONS} ${KDE4_DEFINITIONS})

include(KDE4Defaults)

include(MacroLibrary)

include_directories(${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} ${QIMAGEBLITZ_INCLUDES})


# ########## src executable ##########
# Sources:
SET(schafkopf_SRCS
    src/schafkopfevent.cpp
    src/card.cpp
    src/selectgamewizard.cpp
    src/player.cpp
    src/gameinfo.cpp
    src/schafkopf.cpp
    src/selectgametypebox.cpp
    src/results.cpp
    src/main.cpp
    src/newgamewizard.cpp
    src/game.cpp
    src/humanplayer.cpp
    src/cardlist.cpp
    src/computerplayer.cpp
    src/stichdlg.cpp
    src/gamecanvas.cpp
    src/selectgamecolorbox.cpp
    src/settings.cpp
    src/preferencesdlg.cpp
    src/canvasplayer.cpp
    src/openbook.cpp
    src/canvascard.cpp
)

# Headers:
SET(schafkopf_HDRS
    src/schafkopfevent.h
    src/schafkopf.h
    src/selectgamewizard.h
    src/preferencesdlg.h
    src/gamecanvas.h
    src/openbook.h
    src/canvascard.h
    src/stichdlg.h
    src/settings.h
    src/selectgametypebox.h
    src/selectgamecolorbox.h
    src/newgamewizard.h
)

SET(CMAKE_BUILD_TYPE Release)

QT4_WRAP_CPP(schafkopf_HDRS_MOC ${schafkopf_HDRS})

INCLUDE(${QT_USE_FILE})

ADD_SUBDIRECTORY(icons)

# actual target:
kde4_add_app_icon(schafkopf_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/src/hi*-apps-schafkopf.png") # for win and mac only(?)
kde4_add_executable(schafkopf ${schafkopf_SRCS})
TARGET_LINK_LIBRARIES(schafkopf ${QT_LIBRARIES} ${KDE4_KIO_LIBS} ${KDE4_KDEUI_LIBS} ${KDEGAMES_LIBRARY} ${QIMAGEBLITZ_LIBRARIES})

# add install target:
INSTALL(TARGETS schafkopf ${INSTALL_TARGETS_DEFAULT_ARGS})
INSTALL(FILES data/background01.jpg DESTINATION ${DATA_INSTALL_DIR}/schafkopf/data/)
