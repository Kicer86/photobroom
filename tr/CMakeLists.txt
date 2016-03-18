
find_program(LUPDATE lupdate)
find_program(LRELEASE lrelease)

if(LUPDATE AND LRELEASE)
    add_custom_target(generate_ts_files
                      COMMAND ${LUPDATE} ${PROJECT_SOURCE_DIR}/src/ -ts ${PROJECT_SOURCE_DIR}/tr/photo_broom_pl.ts
                      COMMAND ${LUPDATE} ${PROJECT_SOURCE_DIR}/tools/internal_crash_catcher/crash_dialog/ -ts ${PROJECT_SOURCE_DIR}/tr/crash_catcher_dialog_pl.ts
                     )

    set_source_files_properties(${PROJECT_SOURCE_DIR}/tr/photo_broom_pl.ts PROPERTIES GENERATED TRUE)
    set_source_files_properties(${PROJECT_SOURCE_DIR}/tr/crash_catcher_dialog_pl.ts PROPERTIES GENERATED TRUE)

    add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/photo_broom_pl.qm
                       COMMAND ${LRELEASE} ${PROJECT_SOURCE_DIR}/tr/photo_broom_pl.ts -qm ${CMAKE_CURRENT_BINARY_DIR}/photo_broom_pl.qm
                       DEPENDS ${PROJECT_SOURCE_DIR}/tr/photo_broom_pl.ts
                      )
                      
    add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/crash_catcher_dialog_pl.qm
                       COMMAND ${LRELEASE} ${PROJECT_SOURCE_DIR}/tr/crash_catcher_dialog_pl.ts -qm ${CMAKE_CURRENT_BINARY_DIR}/crash_catcher_dialog_pl.qm
                       DEPENDS ${PROJECT_SOURCE_DIR}/tr/crash_catcher_dialog_pl.ts
                      )

    add_custom_target(translations ALL
                      DEPENDS generate_ts_files
                      DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/photo_broom_pl.qm
                      DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/crash_catcher_dialog_pl.qm
                     )

    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/photo_broom_pl.qm 
                  ${CMAKE_CURRENT_BINARY_DIR}/crash_catcher_dialog_pl.qm
            DESTINATION ${PATH_TR}
    )
    
endif()
