add_subdirectory(fromlibkdegames)

add_executable(schafkopf)

target_sources(schafkopf
  PRIVATE
    schafkopfevent.cpp
    schafkopfevent.h
    card.cpp
    card.h
    selectgamewizard.cpp
    selectgamewizard.h
    player.cpp
    player.h
    gameinfo.cpp
    gameinfo.h
    schafkopf.cpp
    schafkopf.h
    schafkopfdef.h
    selectgametypebox.cpp
    selectgametypebox.h
    results.cpp
    results.h
    main.cpp
    newgamewizard.cpp
    newgamewizard.h
    game.cpp
    game.h
    humanplayer.cpp
    humanplayer.h
    cardlist.cpp
    cardlist.h
    computerplayer.cpp
    computerplayer.h
    stichdlg.cpp
    stichdlg.h
    gamecanvas.cpp
    gamecanvas.h
    selectgamecolorbox.cpp
    selectgamecolorbox.h
    settings.cpp
    settings.h
    preferencesdlg.cpp
    preferencesdlg.h
    canvasplayer.cpp
    canvasplayer.h
    openbook.cpp
    openbook.h
    canvascard.cpp
    canvascard.h
)

target_link_libraries(schafkopf
  PRIVATE
    kdegames
    KF6::XmlGui
    KF6::KIOWidgets
    Qt::Core
    Qt::Gui
    Qt::Widgets
    Qt::Svg
)

target_include_directories(schafkopf PRIVATE ${CMAKE_BINARY_DIR})

target_precompile_headers(schafkopf REUSE_FROM kdegames)

# This has only an effect on Windows and Mac OS X.
ecm_add_app_icon(schafkopf ICONS
  ${CMAKE_SOURCE_DIR}/icons/16-apps-schafkopf.png
  ${CMAKE_SOURCE_DIR}/icons/22-apps-schafkopf.png
  ${CMAKE_SOURCE_DIR}/icons/32-apps-schafkopf.png
  ${CMAKE_SOURCE_DIR}/icons/48-apps-schafkopf.png
  ${CMAKE_SOURCE_DIR}/icons/64-apps-schafkopf.png
  ${CMAKE_SOURCE_DIR}/icons/128-apps-schafkopf.png
)

INSTALL(TARGETS schafkopf ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})

INSTALL(PROGRAMS schafkopf.desktop DESTINATION ${KDE_INSTALL_FULL_APPDIR})
