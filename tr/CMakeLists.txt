
find_program(LUPDATE lupdate)
find_program(LRELEASE lrelease)

if(LUPDATE AND LRELEASE)

    set(translations pl)
    set(qm_files)
    set(tr_update_targets)

    # collect all source files for translations
    file(GLOB_RECURSE source_files
         LIST_DIRECTORIES FALSE
         ${CMAKE_SOURCE_DIR}/src/*.cpp
         ${CMAKE_SOURCE_DIR}/src/*.hpp
         ${CMAKE_SOURCE_DIR}/src/*.ui)

    file(GLOB_RECURSE crash_catcher_source_files
         LIST_DIRECTORIES FALSE
         ${CMAKE_SOURCE_DIR}/tools/internal_crash_catcher/crash_dialog/*.cpp
         ${CMAKE_SOURCE_DIR}/tools/internal_crash_catcher/crash_dialog/*.hpp
         ${CMAKE_SOURCE_DIR}/tools/internal_crash_catcher/crash_dialog/*.ui)

    # write down all paths into a file
    file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/sources.lst "")
    foreach(file ${source_files})
        file(APPEND ${CMAKE_CURRENT_BINARY_DIR}/sources.lst ${file})
        file(APPEND ${CMAKE_CURRENT_BINARY_DIR}/sources.lst "\n")
    endforeach()

    file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/crash_catcher_sources.lst "")
    foreach(file ${crash_catcher_source_files})
        file(APPEND ${CMAKE_CURRENT_BINARY_DIR}/crash_catcher_sources.lst ${file})
        file(APPEND ${CMAKE_CURRENT_BINARY_DIR}/crash_catcher_sources.lst "\n")
    endforeach()

    foreach(tr IN LISTS translations)

        # generate TS files from source files lists
        add_custom_command(OUTPUT ${PROJECT_SOURCE_DIR}/tr/photo_broom_${tr}.ts
                           COMMAND ${LUPDATE}
                           ARGS    @${CMAKE_CURRENT_BINARY_DIR}/sources.lst
                                   -I ${PROJECT_SOURCE_DIR}/src/core
                                   -I ${PROJECT_SOURCE_DIR}/src/database
                                   -I ${PROJECT_SOURCE_DIR}/src/photos_crawler
                                   -I ${PROJECT_SOURCE_DIR}/src/plugins
                                   -noobsolete
                                   -ts ${PROJECT_SOURCE_DIR}/tr/photo_broom_${tr}.ts
                           DEPENDS ${source_files}
                                   ${CMAKE_CURRENT_BINARY_DIR}/sources.lst
        )

        add_custom_command(OUTPUT ${PROJECT_SOURCE_DIR}/tr/crash_catcher_dialog_${tr}.ts
                           COMMAND ${LUPDATE}
                           ARGS    @${CMAKE_CURRENT_BINARY_DIR}/crash_catcher_sources.lst
                                   -noobsolete -ts
                                   ${PROJECT_SOURCE_DIR}/tr/crash_catcher_dialog_${tr}.ts
                           DEPENDS ${source_files}
                                   ${CMAKE_CURRENT_BINARY_DIR}/crash_catcher_sources.lst
        )

        # mark files as generated
        set_source_files_properties(${PROJECT_SOURCE_DIR}/tr/photo_broom_${tr}.ts PROPERTIES GENERATED TRUE)
        set_source_files_properties(${PROJECT_SOURCE_DIR}/tr/crash_catcher_dialog_${tr}.ts PROPERTIES GENERATED TRUE)

        # qm generation rules
        add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/photo_broom_${tr}.qm
                           COMMAND ${LRELEASE} ${PROJECT_SOURCE_DIR}/tr/photo_broom_${tr}.ts -qm ${CMAKE_CURRENT_BINARY_DIR}/photo_broom_${tr}.qm
                           DEPENDS ${PROJECT_SOURCE_DIR}/tr/photo_broom_${tr}.ts
        )

        add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/crash_catcher_dialog_${tr}.qm
                           COMMAND ${LRELEASE} ${PROJECT_SOURCE_DIR}/tr/crash_catcher_dialog_${tr}.ts -qm ${CMAKE_CURRENT_BINARY_DIR}/crash_catcher_dialog_${tr}.qm
                           DEPENDS ${PROJECT_SOURCE_DIR}/tr/crash_catcher_dialog_${tr}.ts
        )

        # installation rules
        install(FILES ${CMAKE_CURRENT_BINARY_DIR}/photo_broom_${tr}.qm
                      ${CMAKE_CURRENT_BINARY_DIR}/crash_catcher_dialog_${tr}.qm
                DESTINATION ${PATH_TR}
        )

        # remember qm files
        list(APPEND qm_files
                    ${CMAKE_CURRENT_BINARY_DIR}/photo_broom_${tr}.qm
                    ${CMAKE_CURRENT_BINARY_DIR}/crash_catcher_dialog_${tr}.qm
        )

    endforeach()

    # translations target to be attached to photo broom target
    add_custom_target(translations
                      DEPENDS ${tr_update_targets}
                      DEPENDS ${qm_files}
    )

    add_dependencies(photo_broom translations)

endif()
