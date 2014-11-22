
if(WIN32)
    add_executable(PhotoBroom WIN32 main.cpp)

    install(TARGETS PhotoBroom RUNTIME DESTINATION ".")
endif()
