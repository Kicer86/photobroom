
find_package(Qt5 REQUIRED COMPONENTS Gui Svg)

add_executable(img_manip EXCLUDE_FROM_ALL img_manip.cpp)
target_link_libraries(img_manip Qt::Gui Qt::Svg)

if(VCPKG_TRIPLET_DIR AND WIN32)

    get_filename_component(qt_bin_dir ${QT_MOC_EXECUTABLE} DIRECTORY)
    set(CMAKE_MSVCIDE_RUN_PATH ${qt_bin_dir})

	file(WRITE ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/qt.conf 
		"[Paths]
		 Prefix = ${qt_bin_dir}"
	)
endif()
