# QT Documentation suggest 3.16
cmake_minimum_required(VERSION 3.16)

project(OpenGL_1 VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets OpenGL OpenGLWidgets)

if (COMMAND qt_standard_project_setup)
    qt_standard_project_setup()
else()
    set(CMAKE_AUTOMOC ON)
    set(CMAKE_AUTOUIC ON)
endif()

set(CMAKE_AUTORCC ON)

qt_add_executable(OpenGL_1 WIN32 MACOSX_BUNDLE
    resources.qrc
    mainwindow.ui
    mainwindow.cpp mainwindow.h
    mainview.cpp mainview.h
    userinput.cpp
    vertex.h
    main.cpp
    tools/cylinder.h tools/cylinder.cpp
    envelope.h envelope.cpp
    movement/path.h
    movement/simplepath.h movement/simplepath.cpp
    movement/polynomial.h movement/polynomial.cpp
    movement/cylindermovement.h movement/cylindermovement.cpp
    envelope.h envelope.cpp
    settings.h
    tools/drum.h tools/drum.cpp
    tools/tool.h
    renderers/renderer.h renderers/renderer.cpp
    renderers/toolrenderer.h renderers/toolrenderer.cpp
    renderers/enveloperenderer.h renderers/enveloperenderer.cpp
    renderers/moverenderer.h renderers/moverenderer.cpp
    tools/sphere.h
    mathutility.h mathutility.cpp
    tooltype.h
    tools/tool.cpp


)

target_include_directories(OpenGL_1 PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(OpenGL_1 PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::OpenGL
    Qt${QT_VERSION_MAJOR}::OpenGLWidgets
)

# This is used for interoperability, do not remove even on linux;
# On linux, result is an executable;
# On Windows, result is a Win32 executable, instead of console executable, command prompt window is not created;
# On MacOS, result is executable bundle;
set_target_properties(OpenGL_1 PROPERTIES
    WIN32_EXECUTABLE ON
    MACOSX_BUNDLE ON
)
