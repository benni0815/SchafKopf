cmake_minimum_required(VERSION 3.13)

project(schafkopf)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 REQUIRED Core Gui Widgets Svg)
find_package(KF5XmlGui REQUIRED)
find_package(KF5GuiAddons REQUIRED)
find_package(KF5KIO REQUIRED)
find_package(Gettext REQUIRED)

add_compile_options(-Wall -Wextra -Wpedantic)

add_subdirectory(src)

# add install target:
GETTEXT_PROCESS_PO_FILES(de ALL INSTALL_DESTINATION ${LOCALE_INSTALL_DIR} po/de/schafkopf.po)

set(INSTALL_OPEN_TAROCK OFF CACHE BOOL "Install the OpenTarock card deck")
set(CARDDECK_DIRECTORY ${DATA_INSTALL_DIR}/carddecks CACHE PATH "The directories for carddeck installation.")
if(INSTALL_OPEN_TAROCK)
  install(DIRECTORY carddecks/svg-open-tarock DESTINATION ${CARDDECK_DIRECTORY})
endif(INSTALL_OPEN_TAROCK)

set(CPACK_GENERATOR "DEB")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Benni")
set(CPACK_PACKAGE_NAME "schafkopf")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Bavarian card game")
set(CPACK_PACKAGE_VERSION_MAJOR "0")
set(CPACK_PACKAGE_VERSION_MINOR "7")
set(CPACK_PACKAGE_VERSION_PATCH "91")
set(CPACK_SET_DESTDIR "ON")

include(CPack)
