
find_package(Qt5 REQUIRED COMPONENTS LinguistTools)

set(translations pl)
set(tr_files)

foreach(tr IN LISTS translations)
    LIST(APPEND tr_files photo_broom_${tr}.ts)   
endforeach()

qt5_create_translation(qm_files ${PROJECT_SOURCE_DIR}/src ${tr_files})

# translations target to be attached to photo broom target
add_custom_target(translations DEPENDS ${qm_files})

add_dependencies(photo_broom translations)

# installation rules
install(FILES ${qm_files} DESTINATION ${PATH_TR})
