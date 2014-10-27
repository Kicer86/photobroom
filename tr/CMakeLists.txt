
find_package(Qt5LinguistTools)

if(Qt5LinguistTools)

	qt5_create_translation(broom_qm_files ${PROJECT_SOURCE_DIR}/src/ photo_broom_pl.ts)

	add_custom_target(translations
        	          DEPENDS ${broom_qm_files})

	install(FILES ${broom_qm_files} DESTINATION ${PATH_TR})

endif(Qt5LinguistTools)
