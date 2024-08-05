set(schafkopf_icons
  ${CMAKE_CURRENT_SOURCE_DIR}/16-apps-schafkopf.png
  ${CMAKE_CURRENT_SOURCE_DIR}/22-apps-schafkopf.png
  ${CMAKE_CURRENT_SOURCE_DIR}/32-apps-schafkopf.png
  ${CMAKE_CURRENT_SOURCE_DIR}/48-apps-schafkopf.png
  ${CMAKE_CURRENT_SOURCE_DIR}/64-apps-schafkopf.png
  ${CMAKE_CURRENT_SOURCE_DIR}/128-apps-schafkopf.png
)

ecm_install_icons(ICONS ${schafkopf_icons}
  DESTINATION ${KDE_INSTALL_FULL_ICONDIR}
)
