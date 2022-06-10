cmake_minimum_required(VERSION 3.16)

project(libvatek_qt)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 COMPONENTS Widgets Core Gui REQUIRED)

set(qtv_INC_CHART
    inc/chart/qtv_chart_define.h
    inc/chart/qtv_chart_ui.h)

set(qtv_INC_CORE
    inc/core/qtv_define.h
    inc/core/qtv_device.h
    inc/core/qtv_dtv.h
    inc/core/qtv_media.h
    inc/core/qtv_mux.h
    inc/core/qtv_property.h
    inc/core/qtv_service.h
    inc/core/qtv_service_usb.h
    inc/core/qtv_source.h)

set(qtv_INC_UI_BASE
    inc/ui/base/qtv_ui_config.h
    inc/ui/base/qtv_ui_core.h
    inc/ui/base/qtv_ui_define.h
    inc/ui/base/qtv_ui_list.h
    inc/ui/base/qtv_ui_tab.h
    inc/ui/base/qtv_ui_table.h
    inc/ui/base/qtv_ui_tabs.h
    inc/ui/base/qtv_ui_tree.h
    inc/ui/base/qtv_ui_wizard.h
    inc/ui/base/qtv_ui_storage.h
    inc/ui/base/qtv_ui_hex.h
    inc/ui/base/qtv_ui_bootlogo.h)

set(qtv_INC_UI
    inc/ui/qtv_ui_app.h
    inc/ui/qtv_ui_service_base.h
    inc/ui/qtv_ui_service_usb.h)

set(qtv_INC_INTERNAL
    src/internal/qinternal_broadcast.h
    src/internal/qinternal_core.h
    src/internal/qinternal_dtv.h
    src/internal/qinternal_mux.h
    src/internal/qinternal_properties.h
    src/internal/qinternal_transform.h
    src/internal/qinternal_ui.h
    src/internal/qinternal_usbmuxer.h
    src/internal/qinternal_chart.h
    src/internal/dtv/dtv_channel.h
    src/internal/dtv/dtv_locale.h
    src/internal/source/qsource_base.h
    src/internal/source/qsource_bridge.h
    src/internal/source/qsource_usbstream.h)

set(qtv_SRC_CORE
    src/qtv_device.cpp
    src/qtv_dtv.cpp
    src/qtv_media.cpp
    src/qtv_mux.cpp
    src/qtv_properties.cpp
    src/qtv_property.cpp
    src/qtv_service_base.cpp
    src/qtv_service_broadcast.cpp
    src/qtv_service_transform.cpp
    src/qtv_service_usbmuxer.cpp
    src/qtv_service_handle.cpp
    src/qtv_source_base.cpp
    src/qtv_source_usbstream.cpp
    src/qtv_source_encoder.cpp
    src/qtv_source_bridge.cpp)

set(qtv_SRC_UI
    src/qtv_ui_app.cpp
    src/qtv_ui_config.cpp
    src/qtv_ui_editor.cpp
    src/qtv_ui_list.cpp
    src/qtv_ui_main.cpp
    src/qtv_ui_resource.cpp
    src/qtv_ui_service_base.cpp
    src/qtv_ui_service_usb.cpp
    src/qtv_ui_tab.cpp
    src/qtv_ui_table.cpp
    src/qtv_ui_tree.cpp
    src/qtv_ui_wizard.cpp
    src/qtv_ui_hex.cpp)

set(qtv_SRC_TABS
    src/tabs/qtvUITabConfigs.cpp
    src/tabs/qtvUITabContext.cpp
    src/tabs/qtvUITabCalibration.cpp
    src/tabs/qtvUITabDevice.cpp
    src/tabs/qtvUITabDtv.cpp
    src/tabs/qtvUITabEnum.cpp
    src/tabs/qtvUITabNoDevice.cpp
    src/tabs/qtvUITabProperties.cpp
    src/tabs/qtvUITabSimple.cpp
    src/tabs/qtvUITabSource.cpp
    src/tabs/qtvUITabStatus.cpp)

set(qtv_SRC_STORAGE
    src/storage/qtvUIStorage.cpp
    src/storage/qtvUIStorageInfo.cpp
    src/storage/qtvUIStorageProps.cpp
    src/storage/qtvUIStorageR2Tune.cpp
    src/storage/bootlogo.cpp)

set(qtv_SRC_CHART
    src/qtv_chart_ui.cpp
    src/qtv_chart_base.cpp
    src/qtv_chart_service_chip.cpp)

set(qtv_UI_TABS
    src/tabs/qtvUITabConfigs.ui
    src/tabs/qtvUITabContext.ui
    src/tabs/qtvUITabCalibration.ui
    src/tabs/qtvUITabDevice.ui
    src/tabs/qtvUITabDtv.ui
    src/tabs/qtvUITabEnum.ui
    src/tabs/qtvUITabProperties.ui
    src/tabs/qtvUITabSimple.ui
    src/tabs/qtvUITabSource.ui
    src/tabs/qtvUITabStatus.ui
    src/storage/qtvUIStorageInfo.ui
    src/storage/qtvUIStorageProps.ui
    src/storage/qtvUIStorageR2Tune.ui
    src/storage/bootlogo.ui)

set(qtv_RES_GLOBAL
    src/resource/qtv_toolkit.qrc)

set(qtv_SOURCES
    ${qtv_SRC_CORE}
    ${qtv_SRC_TABS}
    ${qtv_SRC_UI}
    ${qtv_INC_INTERNAL}
    ${qtv_RES_GLOBAL}
	${qtv_SRC_CHART}
    ${qtv_SRC_STORAGE})

set(qtv_HEADERS
    ${qtv_INC_CHART}
    ${qtv_INC_CORE}
    ${qtv_INC_UI}
    ${qtv_INC_UI_BASE})

if(MSVC)
    source_group("inc\\chart" FILES ${qtv_INC_CHART})
    source_group("inc\\core" FILES ${qtv_INC_CORE})
    source_group("inc\\ui" FILES ${qtv_INC_UI})
    source_group("inc\\ui\\base" FILES ${qtv_INC_UI_BASE})
        
    source_group("src\\internal" FILES ${qtv_INC_INTERNAL})
    source_group("src\\core" FILES ${qtv_SRC_CORE})
    source_group("src\\tabs" FILES ${qtv_SRC_TABS})
    source_group("src\\ui" FILES ${qtv_SRC_UI})
    source_group("src\\resource" FILES ${qtv_RES_GLOBAL})
    source_group("src\\tabs\\ui" FILES ${qtv_UI_TABS})
    source_group("src\\chart" FILES ${qtv_SRC_CHART})
    source_group("src\\storage" FILES ${qtv_SRC_STORAGE})
endif()

include_directories("../core/inc")
include_directories("inc")
include_directories(${CMAKE_CURRENT_BINARY_DIR})

include_directories(${Qt5Core_INCLUDES} ${Qt5Widgets_INCLUDES} ${Qt5Gui_INCLUDES})

qt5_wrap_ui(qtv_SOURCES ${qtv_UI_TABS})

if(NOT TARGET libvatek_core AND NOT TARGET libvatek_core_static)
    message(FATAL_ERROR "libvatek_qt need build with libvatek_core")
endif()

if(MSVC)

    # libvatek_qt #
    
	if(NOT ${SDK2_EN_STATIC_ONLY})
        set(qtv_RCFILE ${CMAKE_BINARY_DIR}/libvatek_qt.rc)
        configure_file(./cmake/qt.rc.in ${qtv_RCFILE})

		add_library(libvatek_qt SHARED ${qtv_SOURCES} ${qtv_RCFILE} ${qtv_HEADERS})
		add_dependencies(libvatek_qt libvatek_core)
		target_compile_options(libvatek_qt PRIVATE /d2FH4-)
		target_compile_options(libvatek_qt PRIVATE /wd26495 PRIVATE /wd26812 PRIVATE /wd26498)
		target_compile_definitions(libvatek_qt PRIVATE -DLIBVATEK_QT_LIB)
		target_link_libraries(libvatek_qt libvatek_core Qt5::Widgets Qt5::Core Qt5::Gui)
		set_property(TARGET libvatek_qt PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
		set_target_properties(libvatek_qt PROPERTIES LINK_FLAGS /SUBSYSTEM:WINDOWS)
		set_target_properties(libvatek_qt PROPERTIES 
							  ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
							  LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
							  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
	endif()

    # libvatek_qt_static #
    add_library(libvatek_qt_static STATIC ${qtv_SOURCES} ${qtv_HEADERS})
    target_compile_options(libvatek_qt_static PRIVATE /d2FH4-)
    target_compile_options(libvatek_qt_static PRIVATE /wd26495 PRIVATE /wd26812 PRIVATE /wd26498)
    target_compile_definitions(libvatek_qt_static PRIVATE -DLIBVATEK_QT_STATIC PRIVATE -D_VA2_STATIC_)
    target_link_libraries(libvatek_qt_static Qt5::Widgets Qt5::Core Qt5::Gui)
    set_property(TARGET libvatek_qt_static PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    set_target_properties(libvatek_qt_static PROPERTIES 
                          ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
                          LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
                          RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
endif()