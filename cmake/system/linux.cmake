
# install main icon in public area
install(FILES ${CMAKE_SOURCE_DIR}/images/photo_broom.svg DESTINATION ${CMAKE_INSTALL_PREFIX}/share/icons/hicolor/scalable/apps)

#install desktop file
install(FILES ${CMAKE_SOURCE_DIR}/src/env/photo_broom.desktop DESTINATION ${CMAKE_INSTALL_PREFIX}/share/applications)
