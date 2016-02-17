
set(SRCS)

if(WIN32)
    list(APPEND SRCS catcher_win32.cpp)
elseif(UNIX)
    list(APPEND SRCS catcher_posix.cpp)
else()
    message(FATAL_ERROR "Unsupported platform")
endif()


if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    list(APPEND SRCS stack_unwind_gcc.cpp)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    list(APPEND SRCS stack_unwind_vs.cpp)
else()
    message(FATAL_ERROR "Unsupported compiler: '${CMAKE_CXX_COMPILER_ID}'")
endif()


add_library(catcher STATIC ${SRCS})
