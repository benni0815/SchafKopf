add_library(kdegames OBJECT)

target_sources(kdegames
  PRIVATE
    cardcache.cpp
    cardcache.h
    cardcache_p.h
    carddeckinfo.cpp
    carddeckinfo.h
    carddeckinfo_p.h
    kcarddialog.cpp
    kcarddialog.h
)

target_link_libraries(kdegames
  PUBLIC
    KF5::GuiAddons
  PRIVATE
    KF5::KIOWidgets
    KF5::I18n
    Qt::Core
    Qt::Gui
    Qt::Svg
)

target_include_directories(kdegames
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_precompile_headers(kdegames
  PUBLIC
    <QtCore>
    <QtGui>
    <QtWidgets>
)
