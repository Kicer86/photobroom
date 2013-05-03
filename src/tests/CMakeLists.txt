
project(tests)

include_directories(..)  #add root

set(TESTS
    entrydata_tests.cpp
    )

add_executable(tests RunAllTests.cpp ${TESTS})
set_target_properties(tests PROPERTIES COMPILE_FLAGS --std=c++11)
