
include(FetchContent)

FetchContent_Declare(
  hugin
  HG_REPOSITORY http://hg.code.sf.net/p/hugin/hugin
  GIT_TAG       2023.0.0
)

FetchContent_Populate(hugin)

find_package(OpenMP REQUIRED)

add_custom_command(
  OUTPUT align_image_stack.cpp
  COMMAND ${CMAKE_COMMAND} -E copy ${hugin_SOURCE_DIR}/src/tools/align_image_stack.cpp align_image_stack.cpp
  COMMAND ${CMAKE_COMMAND} -DSOURCE_FILE=${CMAKE_CURRENT_BINARY_DIR}/align_image_stack.cpp -P ${CMAKE_CURRENT_SOURCE_DIR}/patch_align_image_stack.cmake
)

add_custom_target(process_hugin_tools
  DEPENDS align_image_stack.cpp
)

add_library(align_image_stack_wrapper
  align_image_stack_wrapper.cpp
)

add_dependencies(align_image_stack_wrapper
  process_hugin_tools
)

target_include_directories(align_image_stack_wrapper
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${hugin_SOURCE_DIR}/src/hugin_base
    ${hugin_SOURCE_DIR}/src
)

target_link_libraries(align_image_stack_wrapper
  PRIVATE
    OpenMP::OpenMP_CXX
)

set_property(TARGET align_image_stack_wrapper PROPERTY CXX_STANDARD 17)  # limiting c++ standard to 17 due to required std::allocator::rebind
