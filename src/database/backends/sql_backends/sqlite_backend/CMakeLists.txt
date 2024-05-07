
find_package(Qt6 REQUIRED COMPONENTS Core Gui Sql)

if(STATIC_PLUGINS)
    add_definitions(-DSTATIC_PLUGINS)
endif()
include_directories(${CMAKE_SOURCE_DIR}/src)   #required by moc

qt6_wrap_cpp(BACKEND_MOCS ${BACKEND_HEADERS})


###########################################################################################################################
#######################################################   BACKEND   #######################################################
###########################################################################################################################

add_library(database_sqlite_backend STATIC          # backend needs to be static (with current architecture) as it would not be allowed to be installed along with plugin file: there cannot be any nonplugin (regular so files for example) files in 'plugins' directory
    backend.cpp
    backend.hpp
)

set_target_properties(database_sqlite_backend PROPERTIES AUTOMOC TRUE)

target_link_libraries(database_sqlite_backend
    PUBLIC
        database
        sql_backend_base

    PRIVATE
        Qt::Core
        Qt::Sql

        core
)

target_include_directories(database_sqlite_backend
                            PUBLIC
                                ${CMAKE_CURRENT_BINARY_DIR}
)

hideSymbols(database_sqlite_backend)
generate_export_header(database_sqlite_backend)


###########################################################################################################################
####################################################### PLUGIN PART #######################################################
###########################################################################################################################

add_library(database_sqlite_plugin MODULE
    plugin.cpp
    plugin.hpp
)

set_target_properties(database_sqlite_plugin PROPERTIES AUTOMOC TRUE)

target_link_libraries(database_sqlite_plugin
    PRIVATE
        Qt::Core
        database_sqlite_backend
)

target_include_directories(database_sqlite_plugin
    PRIVATE
        ${CMAKE_CURRENT_BINARY_DIR}
)

hideSymbols(database_sqlite_plugin)
generate_export_header(database_sqlite_plugin)

install(TARGETS database_sqlite_plugin RUNTIME LIBRARY DESTINATION ${PATH_PLUGINS}/database)
