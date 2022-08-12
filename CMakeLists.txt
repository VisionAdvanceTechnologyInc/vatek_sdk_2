cmake_minimum_required(VERSION 3.16)

project(vatek_sdk_2)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

if(MSVC)

    option(SDK2_EN_QT "build vatek_qt enable" OFF)
    option(SDK2_EN_APP "build demo app enable" ON)
    option(SDK2_EN_SAMPLE "build sample app enable" ON)

    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(SDK2_QTDIR "D:\\Qt\\5.15.2\\msvc2019_64" CACHE STRING "default QT_DIR")
    else()
        set(SDK2_QTDIR "D:\\Qt\\5.15.2\\msvc2019" CACHE STRING "default QT_DIR")
    endif()

    set(CMAKE_PREFIX_PATH ${SDK2_QTDIR})

elseif(UNIX AND NOT APPLE)

    option(SDK2_EN_QT "build vatek_qt enable" OFF)
    option(SDK2_EN_APP "build demo app enable" ON)
    option(SDK2_EN_SAMPLE "build sample app enable" ON)
    
    install(FILES "${PROJECT_SOURCE_DIR}/build/linux/udev/50-usb-vatek.rules" DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/udev/rules.d/")

elseif(APPLE)

    option(SDK2_EN_QT "build vatek_qt enable" OFF)
    option(SDK2_EN_APP "build demo app enable" ON)
    option(SDK2_EN_SAMPLE "build sample app enable" ON)
else()
    message(FATAL_ERROR "unsupported operation system")
endif()

option(SDK2_EN_STATIC_ONLY "build static library only" OFF)

add_subdirectory(api/core)

if(${SDK2_EN_APP})

endif()

if(${SDK2_EN_SAMPLE})
	add_subdirectory(sample/app_stream)
	add_subdirectory(sample/app_romtool)
	add_subdirectory(sample/app_broadcast)
	add_subdirectory(sample/app_bridge)
endif()

if(${SDK2_EN_QT})
    add_subdirectory(api/qt)
    if(${SDK2_EN_APP})
        add_subdirectory(app/vatek_toolkit)
		add_subdirectory(app/vatek_factory)
        add_subdirectory(app/vatek_romtool)
    endif()
endif()
