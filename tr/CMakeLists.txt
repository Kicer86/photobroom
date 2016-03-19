
find_program(LUPDATE lupdate)
find_program(LRELEASE lrelease)

if(LUPDATE AND LRELEASE)

    # create empty ts files if not available
    add_custom_command(OUTPUT ${PROJECT_SOURCE_DIR}/tr/photo_broom_pl.ts
                       COMMAND ${CMAKE_COMMAND} -E touch ${PROJECT_SOURCE_DIR}/tr/photo_broom_pl.ts)

    add_custom_command(OUTPUT ${PROJECT_SOURCE_DIR}/tr/crash_catcher_dialog_pl.ts
                       COMMAND ${CMAKE_COMMAND} -E touch ${PROJECT_SOURCE_DIR}/tr/crash_catcher_dialog_pl.ts)


    # We want ts files to be updated every build, so proper commands ar added as target.
    # It would be optimal to make them depend on cpp files with translations but it wouldn't
    # be as convenient as it is now.
    add_custom_target(generate_ts_files
                      COMMAND ${LUPDATE} ${PROJECT_SOURCE_DIR}/src/ -ts ${PROJECT_SOURCE_DIR}/tr/photo_broom_pl.ts
                      COMMAND ${LUPDATE} ${PROJECT_SOURCE_DIR}/tools/internal_crash_catcher/crash_dialog/ -ts ${PROJECT_SOURCE_DIR}/tr/crash_catcher_dialog_pl.ts
                     )

    set_source_files_properties(${PROJECT_SOURCE_DIR}/tr/photo_broom_pl.ts PROPERTIES GENERATED TRUE)
    set_source_files_properties(${PROJECT_SOURCE_DIR}/tr/crash_catcher_dialog_pl.ts PROPERTIES GENERATED TRUE)

    # qm generation rules
    add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/photo_broom_pl.qm
                       COMMAND ${LRELEASE} ${PROJECT_SOURCE_DIR}/tr/photo_broom_pl.ts -qm ${CMAKE_CURRENT_BINARY_DIR}/photo_broom_pl.qm
                       DEPENDS ${PROJECT_SOURCE_DIR}/tr/photo_broom_pl.ts
                      )

    add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/crash_catcher_dialog_pl.qm
                       COMMAND ${LRELEASE} ${PROJECT_SOURCE_DIR}/tr/crash_catcher_dialog_pl.ts -qm ${CMAKE_CURRENT_BINARY_DIR}/crash_catcher_dialog_pl.qm
                       DEPENDS ${PROJECT_SOURCE_DIR}/tr/crash_catcher_dialog_pl.ts
                      )

    # main target
    add_custom_target(translations ALL
                      DEPENDS generate_ts_files
                      DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/photo_broom_pl.qm
                      DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/crash_catcher_dialog_pl.qm
                     )

    # installation rules
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/photo_broom_pl.qm
                  ${CMAKE_CURRENT_BINARY_DIR}/crash_catcher_dialog_pl.qm
            DESTINATION ${PATH_TR}
    )

endif()
