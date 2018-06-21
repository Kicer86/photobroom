
# install main icon in public area
install(FILES ${CMAKE_SOURCE_DIR}/images/photo_broom.svg DESTINATION share/icons/hicolor/scalable/apps)

#install desktop file
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/resources/linux/photo_broom.desktop DESTINATION share/applications)
