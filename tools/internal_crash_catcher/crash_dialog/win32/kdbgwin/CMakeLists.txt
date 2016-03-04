
find_package(Qt5Core REQUIRED)

include_directories(SYSTEM ${Qt5Core_INCLUDE_DIRS} D:/tools/mingw-w64/i686-4.9.2-posix-dwarf-rt_v4-rev2/mingw32/include)


set(kdbgwin_SRCS
    common.h
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

qt5_wrap_cpp(WRAPPED_CPP abstract_generator.h outputters.h)

set(COMMON_LIBS dbghelp psapi shlwapi)

if ( MINGW )
    # FIXME this should be handled in a higher level cmake with proper find
    # scripts and configuration
    qt5_wrap_cpp(WRAPPED_CPP_EXTRA mingw_generator.h)
    add_executable(kdbgwin ${kdbgwin_SRCS} ${WRAPPED_CPP} ${WRAPPED_CPP_EXTRA} mingw_generator.h mingw_generator.cpp)
    target_link_libraries(kdbgwin ${COMMON_LIBS} Qt5::Core bfd iberty z)
endif ()
if ( MSVC OR ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel"))
    qt5_wrap_cpp(WRAPPED_CPP_EXTRA msvc_generator.h)
    add_executable(kdbgwin ${kdbgwin_SRCS} ${WRAPPED_CPP} ${WRAPPED_CPP_EXTRA} msvc_generator.h msvc_generator.cpp)
    target_link_libraries(kdbgwin ${COMMON_LIBS})
endif ()

install(TARGETS kdbgwin DESTINATION ${PATH_BIN})
