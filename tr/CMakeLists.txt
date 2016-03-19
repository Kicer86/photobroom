
find_program(LUPDATE lupdate)
find_program(LRELEASE lrelease)

if(LUPDATE AND LRELEASE)

    set(translations pl)
    set(qm_files)
    set(tr_update_targets)

    foreach(tr IN LISTS translations)

        # create empty ts files if not available
        add_custom_command(OUTPUT ${PROJECT_SOURCE_DIR}/tr/photo_broom_${tr}.ts
                           COMMAND ${CMAKE_COMMAND} -E touch ${PROJECT_SOURCE_DIR}/tr/photo_broom_${tr}.ts)

        add_custom_command(OUTPUT ${PROJECT_SOURCE_DIR}/tr/crash_catcher_dialog_${tr}.ts
                           COMMAND ${CMAKE_COMMAND} -E touch ${PROJECT_SOURCE_DIR}/tr/crash_catcher_dialog_${tr}.ts)


        # We want ts files to be updated every build, so proper commands ar added as target.
        # It would be optimal to make them depend on cpp files with translations but it wouldn't
        # be as convenient as it is now.
        add_custom_target(generate_ts_files_${tr}
                          COMMAND ${LUPDATE} ${PROJECT_SOURCE_DIR}/src/ -ts ${PROJECT_SOURCE_DIR}/tr/photo_broom_${tr}.ts
                          COMMAND ${LUPDATE} ${PROJECT_SOURCE_DIR}/tools/internal_crash_catcher/crash_dialog/ -ts ${PROJECT_SOURCE_DIR}/tr/crash_catcher_dialog_${tr}.ts
        )

        list(APPEND tr_update_targets generate_ts_files_${tr})

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

    # main target
    add_custom_target(translations ALL
                      DEPENDS ${tr_update_targets}
                      DEPENDS ${qm_files}
    )

endif()
