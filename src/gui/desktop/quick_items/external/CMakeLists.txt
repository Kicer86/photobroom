
find_package(Qt6 REQUIRED COMPONENTS
                 Quick
                 QuickWidgets
                 Gui
)

find_package(OpenGL REQUIRED)

qt6_wrap_cpp(WRAPPED qphotosphere/plugins/photosphere.h)

add_library(qphotosphere STATIC
    qphotosphere/plugins/photosphere.cpp
    qphotosphere/plugins/photosphere.h
    ${WRAPPED}
)

target_link_libraries(qphotosphere
    PUBLIC
        Qt::Quick
        Qt::Gui
        OpenGL::GLU
)

target_include_directories(qphotosphere
    INTERFACE
        qphotosphere/plugins
)
