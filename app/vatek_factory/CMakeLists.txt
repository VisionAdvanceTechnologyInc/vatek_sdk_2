cmake_minimum_required(VERSION 3.16)

project(vatek_factory)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 COMPONENTS Widgets Core Gui REQUIRED)

if(NOT TARGET libvatek_qt)
    message(FATAL_ERROR "vatek_factory need build with libvatek_qt")
endif()

set(factory_SRC
    src/main.cpp
    src/vatek_factory.cpp
    src/openfiledlg.cpp)

set(factory_INC
    inc/vatek_factory.h
    inc/openfiledlg.h)

set(factory_QRC
    src/resource/vatek_factory.qrc)

set(factory_UI
    src/resource/vatek_factory.ui
    src/resource/openfiledlg.ui)

include_directories("inc" "../../api/core/inc" "../../api/qt/inc")
include_directories(${CMAKE_CURRENT_BINARY_DIR})
    
include_directories(${Qt5Core_INCLUDES} ${Qt5Widgets_INCLUDES} ${Qt5Gui_INCLUDES})
    
qt5_wrap_ui(factory_SRC ${factory_UI})

if(MSVC)
    set(QT_USE_QTMAIN TRUE)

    set(factory_RCFILE ${CMAKE_BINARY_DIR}/vatek_factory.rc)
    configure_file(./cmake/factory.rc.in ${factory_RCFILE})
    
    add_executable(vatek_factory ${factory_SRC} ${factory_QRC} ${factory_RCFILE} ${factory_INC})
    add_dependencies(vatek_factory libvatek_qt)
    target_link_libraries(vatek_factory libvatek_qt Qt5::Widgets Qt5::Core Qt5::Gui Qt5::WinMain)
    target_compile_options(vatek_factory PRIVATE /d2FH4-)
    target_compile_options(vatek_factory PRIVATE /wd26495 PRIVATE /wd26812 PRIVATE /wd26498)
    #set_property(TARGET vatek_factory PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    set_target_properties(vatek_factory PROPERTIES LINK_FLAGS /SUBSYSTEM:WINDOWS)
    set_target_properties(vatek_factory PROPERTIES VS_DEBUGGER_ENVIRONMENT "PATH=${SDK2_QTDIR}\\bin;${SDK2_QTDIR}\\plugins\\platforms;%PATH%")
    set_target_properties(vatek_factory PROPERTIES 
                          ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
                          LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
                          RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
endif()