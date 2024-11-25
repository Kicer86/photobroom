
if(BUILD_UPDATER)
    set(CppRestAPI_QtBackend ON)
    set(CppRestAPI_UseQt6 TRUE)

    add_subdirectory(cpp_restapi)

    if(BUILD_SHARED_LIBS)
        install(TARGETS cpp_restapi RUNTIME DESTINATION ${PATH_LIBS}
                                    LIBRARY DESTINATION ${PATH_LIBS})
    endif()
endif()

add_subdirectory(magic_enum)
add_subdirectory(nanobench)
