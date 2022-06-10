cmake_minimum_required(VERSION 3.16)

project(vatek_toolkit)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 COMPONENTS Widgets Core Gui REQUIRED)

if(NOT TARGET libvatek_qt)
    message(FATAL_ERROR "vatek_toolkit need build with libvatek_qt")
endif()

set(toolkit_SRC
    src/main.cpp
    src/vatek_toolkit.cpp)

set(toolkit_INC
    inc/vatek_toolkit.h)

set(toolkit_QRC
    src/resource/vatek_toolkit.qrc)

set(toolkit_UI
    src/resource/vatek_toolkit.ui)

include_directories("inc" "../../api/core/inc" "../../api/qt/inc")
include_directories(${CMAKE_CURRENT_BINARY_DIR})
    
include_directories(${Qt5Core_INCLUDES} ${Qt5Widgets_INCLUDES} ${Qt5Gui_INCLUDES})
    
qt5_wrap_ui(toolkit_SRC ${toolkit_UI})

if(MSVC)
    set(QT_USE_QTMAIN TRUE)

    set(toolkit_RCFILE ${CMAKE_BINARY_DIR}/vatek_toolkit.rc)
    configure_file(./cmake/toolkit.rc.in ${toolkit_RCFILE})
    
    add_executable(vatek_toolkit ${toolkit_SRC} ${toolkit_QRC} ${toolkit_RCFILE} ${toolkit_INC})
    add_dependencies(vatek_toolkit libvatek_qt)
    target_link_libraries(vatek_toolkit libvatek_qt Qt5::Widgets Qt5::Core Qt5::Gui Qt5::WinMain)
    target_compile_options(vatek_toolkit PRIVATE /d2FH4-)
    target_compile_options(vatek_toolkit PRIVATE /wd26495 PRIVATE /wd26812 PRIVATE /wd26498)
    #set_property(TARGET vatek_toolkit PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    set_target_properties(vatek_toolkit PROPERTIES LINK_FLAGS /SUBSYSTEM:WINDOWS)
    set_target_properties(vatek_toolkit PROPERTIES VS_DEBUGGER_ENVIRONMENT "PATH=${SDK2_QTDIR}\\bin;${SDK2_QTDIR}\\plugins\\platforms;%PATH%")
    set_target_properties(vatek_toolkit PROPERTIES 
                          ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
                          LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
                          RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
endif()