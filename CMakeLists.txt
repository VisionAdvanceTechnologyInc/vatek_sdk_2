cmake_minimum_required(VERSION 3.16)

project(vatek_sdk_2)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

if(MSVC)

    option(SDK2_EN_SAMPLE "build sample app enable" ON)

elseif(UNIX AND NOT APPLE)

    option(SDK2_EN_SAMPLE "build sample app enable" ON)
elseif(APPLE)

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
