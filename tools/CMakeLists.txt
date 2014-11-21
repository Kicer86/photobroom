
if(WIN32)
    add_executable(PhotoBroomLaunch WIN32 main.cpp)

    install(TARGETS PhotoBroomLaunch RUNTIME DESTINATION ".")
endif()
