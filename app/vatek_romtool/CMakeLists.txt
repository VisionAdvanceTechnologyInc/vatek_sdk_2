cmake_minimum_required(VERSION 3.16)

project(vatek_romtool)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 COMPONENTS Widgets Core Gui REQUIRED)

if(NOT TARGET libvatek_qt)
    message(FATAL_ERROR "${PROJECT_NAME} need build with libvatek_qt")
endif()

set(app_SRC
    src/main.cpp
    src/vatek_romtool.cpp
    src/storage_device.cpp
	src/progressdlg.cpp)

set(app_INC
    inc/storage_device.h
    inc/vatek_romtool.h
	inc/progressdlg.h)

set(app_QRC
    src/resource/app.qrc)

set(app_UI
    src/resource/vatek_romtool.ui
	src/resource/progressdlg.ui)

include_directories("inc" "../../api/core/inc" "../../api/qt/inc")
include_directories(${CMAKE_CURRENT_BINARY_DIR})
    
include_directories(${Qt5Core_INCLUDES} ${Qt5Widgets_INCLUDES} ${Qt5Gui_INCLUDES})
    
qt5_wrap_ui(app_SRC ${app_UI})

if(MSVC)
    set(QT_USE_QTMAIN TRUE)

    set(app_RCFILE ${CMAKE_BINARY_DIR}/app.rc)
    configure_file(./cmake/app.rc.in ${app_RCFILE})
    
    add_executable(${PROJECT_NAME} ${app_SRC} ${app_QRC} ${app_RCFILE} ${app_INC})
    add_dependencies(${PROJECT_NAME} libvatek_qt)
    target_link_libraries(${PROJECT_NAME} libvatek_qt Qt5::Widgets Qt5::Core Qt5::Gui Qt5::WinMain)
    target_compile_options(${PROJECT_NAME} PRIVATE /d2FH4-)
    target_compile_options(${PROJECT_NAME} PRIVATE /wd26495 PRIVATE /wd26812 PRIVATE /wd26498)
    #set_property(TARGET ${PROJECT_NAME} PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS /SUBSYSTEM:WINDOWS)
    set_target_properties(${PROJECT_NAME} PROPERTIES VS_DEBUGGER_ENVIRONMENT "PATH=${SDK2_QTDIR}\\bin;${SDK2_QTDIR}\\plugins\\platforms;%PATH%")
    set_target_properties(${PROJECT_NAME} PROPERTIES 
                          ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
                          LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
                          RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
endif()