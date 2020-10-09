
find_package(Qt6 REQUIRED COMPONENTS Core)

set(kdbgwin_SRCS
    common.h
    config.h
    main.cpp
    process.h
    process.cpp
    abstract_generator.h
    abstract_generator.cpp
    callbacks.h
    callbacks.cpp
    outputters.h
    outputters.cpp
)

set(kdbgwin_TO_MOC
        abstract_generator.h
        outputters.h
)

set(kdbgwin_LIBS Qt::Core dbghelp psapi shlwapi)

if(MINGW)

    find_package(Bfd REQUIRED)
    include_directories(SYSTEM ${BFD_INCLUDE_DIRS})

    list(APPEND kdbgwin_TO_MOC mingw_generator.h)
    list(APPEND kdbgwin_SRCS   mingw_generator.h mingw_generator.cpp)
    list(APPEND kdbgwin_LIBS   ${BFD_LIBRARIES} iberty z)

elseif( MSVC OR ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel"))

    list(APPEND kdbgwin_TO_MOC msvc_generator.h)
    list(APPEND kdbgwin_SRCS   msvc_generator.h msvc_generator.cpp)

endif()

qt5_wrap_cpp(WRAPPED_CPP ${kdbgwin_TO_MOC})

add_executable(kdbgwin ${kdbgwin_SRCS} ${WRAPPED_CPP})
target_link_libraries(kdbgwin ${kdbgwin_LIBS})
install(TARGETS kdbgwin DESTINATION ${PATH_BIN})
