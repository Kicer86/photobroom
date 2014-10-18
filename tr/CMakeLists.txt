
find_package(Qt5LinguistTools REQUIRED)

qt5_create_translation(broom_qm_files ${PROJECT_SOURCE_DIR}/src/ photo_broom_pl.ts)

add_custom_target(translations ALL
                  DEPENDS ${broom_qm_files})

install(FILES ${broom_qm_files} DESTINATION ${PATH_TR})
