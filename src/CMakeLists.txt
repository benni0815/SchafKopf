# ########## Project setup ##########
PROJECT(schafkopf)
CMAKE_MINIMUM_REQUIRED(VERSION 2.4.5)

# ######### General setup ##########
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR})

find_package(KDE4 REQUIRED)

add_definitions(${QT_DEFINITIONS} ${KDE4_DEFINITIONS})

include(KDE4Defaults)

include(MacroLibrary)

include_directories(${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} )

add_definitions(-DQT3_SUPPORT -DQT3_SUPPORT_WARNINGS)

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
    commbase.h
    schafkopf.h
    gameinfo.h
    card.h
    selectgamewizard.h
    schafkopfdef.h
    game.h
    preferencesdlg.h
    canvasplayer.h
    gamecanvas.h
    openbook.h
    player.h
    results.h
    cardlist.h
    canvascard.h
    stichdlg.h
    humanplayer.h
    settings.h
    computerplayer.h
    selectgametypebox.h
    selectgamecolorbox.h
    selectgamedlg.h
    newgamewizard.h
)

qt4_automoc(${schafkopf_SRCS})

# actual target:
ADD_EXECUTABLE(src ${schafkopf_SRCS})

# add install target:
INSTALL(TARGETS src DESTINATION bin)
