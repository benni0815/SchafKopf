cmake_minimum_required(VERSION 3.13)

set(CMAKE_CONFIGURATION_TYPES "Release;Debug;RelWithDebugInfo" CACHE STRING "")

set(RELEASE_SERVICE_VERSION_MAJOR "0")
set(RELEASE_SERVICE_VERSION_MINOR "7")
set(RELEASE_SERVICE_VERSION_MICRO "93")
set(RELEASE_SERVICE_VERSION "${RELEASE_SERVICE_VERSION_MAJOR}.${RELEASE_SERVICE_VERSION_MINOR}.${RELEASE_SERVICE_VERSION_MICRO}")

project(schafkopf VERSION ${RELEASE_SERVICE_VERSION})

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

set(QT_MIN_VERSION "6.6.0")
set(KF_MIN_VERSION "6.0.0")

find_package(Qt6 ${QT_MIN_VERSION} REQUIRED Core Gui Widgets Svg)

find_package(ECM REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

include(ECMAddAppIcon)
include(ECMInstallIcons)
include(ECMSetupVersion)
include(ECMDeprecationSettings)
include(KDEInstallDirs)

ecm_setup_version(${RELEASE_SERVICE_VERSION}
                  VARIABLE_PREFIX SCHAFKOPF
                  VERSION_HEADER "${CMAKE_CURRENT_BINARY_DIR}/schafkopf_version.h"
)

find_package(KF6XmlGui ${KF_MIN_VERSION} REQUIRED)
find_package(KF6GuiAddons ${KF_MIN_VERSION} REQUIRED)
find_package(KF6KIO ${KF_MIN_VERSION} REQUIRED)
find_package(KF6I18n ${KF_MIN_VERSION} REQUIRED)
find_package(Gettext REQUIRED)

add_compile_options(-Wall -Wextra -Wpedantic)

ecm_set_disabled_deprecation_versions(
    QT 6.8.0
    KF 6.14.0
)

add_subdirectory(src)
add_subdirectory(icons)
add_subdirectory(data)

ki18n_install(po)

set(CARDDECK_DIRECTORY ${KDE_INSTALL_FULL_DATADIR}/carddecks CACHE PATH "The directory for carddeck installation.")
install(DIRECTORY carddecks/svg-open-tarock DESTINATION ${CARDDECK_DIRECTORY})

set(CPACK_GENERATOR "DEB")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Benni")
set(CPACK_PACKAGE_NAME "schafkopf")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Bavarian card game")
set(CPACK_PACKAGE_VERSION_MAJOR "${RELEASE_SERVICE_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${RELEASE_SERVICE_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${RELEASE_SERVICE_VERSION_MICRO}")
set(CPACK_SET_DESTDIR "ON")

include(CPack)
