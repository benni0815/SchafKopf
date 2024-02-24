# ########## Project setup ##########
CMAKE_MINIMUM_REQUIRED(VERSION 3.13)
PROJECT(schafkopf)

set(CMAKE_CXX_STANDARD 14)

# ######### General setup ##########
SET(CMAKE_AUTOMOC ON)
SET(CMAKE_AUTOUIC ON)
find_package(Qt5 REQUIRED Core Gui Widgets Svg)
find_package(KF5XmlGui REQUIRED)
find_package(KF5GuiAddons REQUIRED)
find_package(KF5KIO REQUIRED)
find_package(Gettext REQUIRED)

add_subdirectory(src)

SET(CPACK_GENERATOR "DEB")
SET(CPACK_DEBIAN_PACKAGE_MAINTAINER "Benni")
SET(CPACK_PACKAGE_NAME "schafkopf")
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Bavarian card game")
SET(CPACK_PACKAGE_VERSION_MAJOR "0")
SET(CPACK_PACKAGE_VERSION_MINOR "7")
SET(CPACK_PACKAGE_VERSION_PATCH "91")
SET(CPACK_SET_DESTDIR "ON")

INCLUDE(CPack)

# add install target:
GETTEXT_PROCESS_PO_FILES(de ALL INSTALL_DESTINATION ${LOCALE_INSTALL_DIR} po/de/schafkopf.po)

set(INSTALL_OPEN_TAROCK OFF CACHE BOOL "Install the OpenTarock card deck")
set(CARDDECK_DIRECTORY ${DATA_INSTALL_DIR}/carddecks CACHE PATH "The directories for carddeck installation.")
if(INSTALL_OPEN_TAROCK)
  install(DIRECTORY carddecks/svg-open-tarock DESTINATION ${CARDDECK_DIRECTORY})
endif(INSTALL_OPEN_TAROCK)

