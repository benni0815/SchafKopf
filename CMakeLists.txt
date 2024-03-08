cmake_minimum_required(VERSION 3.13)

set(CMAKE_CONFIGURATION_TYPES "Release;Debug;RelWithDebugInfo" CACHE STRING "")

set(RELEASE_SERVICE_VERSION_MAJOR "0")
set(RELEASE_SERVICE_VERSION_MINOR "7")
set(RELEASE_SERVICE_VERSION_MICRO "92")
set(RELEASE_SERVICE_VERSION "${RELEASE_SERVICE_VERSION_MAJOR}.${RELEASE_SERVICE_VERSION_MINOR}.${RELEASE_SERVICE_VERSION_MICRO}")

project(schafkopf VERSION ${RELEASE_SERVICE_VERSION})

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

find_package(ECM REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

include(ECMAddAppIcon)
include(ECMInstallIcons)
include(ECMSetupVersion)
include(KDEInstallDirs)

ecm_setup_version(${RELEASE_SERVICE_VERSION}
                  VARIABLE_PREFIX SCHAFKOPF
                  VERSION_HEADER "${CMAKE_CURRENT_BINARY_DIR}/schafkopf_version.h"
)

find_package(Qt5 REQUIRED Core Gui Widgets Svg)
find_package(KF5XmlGui REQUIRED)
find_package(KF5GuiAddons REQUIRED)
find_package(KF5KIO REQUIRED)
find_package(Gettext REQUIRED)

add_compile_options(-Wall -Wextra -Wpedantic)
add_compile_definitions(QT_DISABLE_DEPRECATED_BEFORE=0x060000)

add_subdirectory(src)
add_subdirectory(icons)

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
set(CPACK_PACKAGE_VERSION_MAJOR "${RELEASE_SERVICE_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${RELEASE_SERVICE_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${RELEASE_SERVICE_VERSION_MICRO}")
set(CPACK_SET_DESTDIR "ON")

include(CPack)
