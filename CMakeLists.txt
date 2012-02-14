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
add_definitions(-DQT3_SUPPORT -DQT3_SUPPORT_WARNINGS)

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

QT4_WRAP_CPP(schafkopf_HDRS_MOC ${schafkopf_HDRS})

INCLUDE(${QT_USE_FILE})

# actual target:
ADD_EXECUTABLE(schafkopf ${schafkopf_SRCS} ${schafkopf_HDRS_MOC})
TARGET_LINK_LIBRARIES(schafkopf ${QT_LIBRARIES} ${QT_QT3SUPPORT_LIBRARY} ${KDE4_KDEUI_LIBS} ${KDEGAMES_LIBRARY} ${KDE4_KDE3SUPPORT_LIBRARY} ${QIMAGEBLITZ_LIBRARIES})

# uncomment next line to suppress Qt3 and KDE3 warnings
#MACRO_ADD_COMPILE_FLAGS(schafkopf -Wno-deprecated-declarations)

# add install target:
INSTALL(TARGETS schafkopf DESTINATION bin)
