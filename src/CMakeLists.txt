# ########## Project setup ##########
PROJECT(schafkopf)
CMAKE_MINIMUM_REQUIRED(VERSION 2.4.5)

# ######### General setup ##########
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR})

FIND_PACKAGE(KDE4 REQUIRED)
FIND_PACKAGE(LibKDEGames REQUIRED)
FIND_PACKAGE(Qt4 REQUIRED)

add_definitions(${QT_DEFINITIONS} ${KDE4_DEFINITIONS})
add_definitions(-DQT3_SUPPORT -DQT3_SUPPORT_WARNINGS)

include(KDE4Defaults)

include(MacroLibrary)

include_directories(${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} )


# ########## src executable ##########
# Sources:
SET(schafkopf_SRCS
    card.cpp
    selectgamewizard.cpp
    player.cpp
    gameinfo.cpp
    schafkopf.cpp
    selectgametypebox.cpp
    results.cpp
    main.cpp
    newgamewizard.cpp
    game.cpp
    humanplayer.cpp
    cardlist.cpp
    computerplayer.cpp
    stichdlg.cpp
    gamecanvas.cpp
    selectgamedlg.cpp
    selectgamecolorbox.cpp
    settings.cpp
    preferencesdlg.cpp
    canvasplayer.cpp
    openbook.cpp
    canvascard.cpp
)

# Headers:
SET(schafkopf_HDRS
    schafkopf.h
    selectgamewizard.h
    preferencesdlg.h
    gamecanvas.h
    openbook.h
    canvascard.h
    stichdlg.h
    settings.h
    selectgametypebox.h
    selectgamecolorbox.h
    selectgamedlg.h
    newgamewizard.h
)

QT4_WRAP_CPP(schafkopf_HDRS_MOC ${schafkopf_HDRS})

INCLUDE(${QT_USE_FILE})

# actual target:
ADD_EXECUTABLE(schafkopf ${schafkopf_SRCS} ${schafkopf_HDRS_MOC})
TARGET_LINK_LIBRARIES(schafkopf ${QT_LIBRARIES} ${QT_QT3SUPPORT_LIBRARY} ${KDE4_KDEUI_LIBS} ${KDEGAMES_LIBRARY} ${KDE4_KDE3SUPPORT_LIBRARY})

# add install target:
INSTALL(TARGETS schafkopf DESTINATION bin)
