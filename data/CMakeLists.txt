configure_file(${CMAKE_CURRENT_SOURCE_DIR}/background01.jpg
               ${CMAKE_BINARY_DIR}/background01.jpg COPYONLY)

INSTALL(FILES
  background01.jpg
  DESTINATION ${KDE_INSTALL_FULL_DATADIR}/schafkopf
)
