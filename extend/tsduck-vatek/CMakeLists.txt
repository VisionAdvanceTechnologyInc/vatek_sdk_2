cmake_minimum_required(VERSION 3.16)

project(tsduck-vatek)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(TSDUCK_VATEK_PLUGIN	tsplugin_vatek)
set(TSDUCK_VATEK_TOOL tsvatek)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SDK2_DIR "../../" CACHE STRING "default vatek_sdk_2 folder")
set(TSDUCK_BUILDCONFIG "Debug" CACHE STRING "build configure type")
set(TSDUCK_DIR "../../output/tsduck-3.29-2651" CACHE STRING "default tsduck source folder")
set(SDK2_EN_STATIC_ONLY ON)
set(SDK2_EN_QT OFF)
set(SDK2_EN_APP OFF)
set(SDK2_EN_SAMPLE OFF)

if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${SDK2_DIR}/)
    message(FATAL_ERROR "vatek_sdk_2 not found please set SDK2_DIR before config")
endif()

if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${TSDUCK_DIR}/)
    message(FATAL_ERROR "tsduck source not found please set TSDUCK_DIR before config")
endif()

if(MSVC)
	if(CMAKE_SIZEOF_VOID_P EQUAL 8)
		set(TSDUCK_LIB_DIR	${TSDUCK_DIR}/bin/${TSDUCK_BUILDCONFIG}-x64)
	else()
		set(TSDUCK_LIB_DIR	${TSDUCK_DIR}/bin/${TSDUCK_BUILDCONFIG}-Win32)
	endif()
	
	if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${TSDUCK_LIB_DIR}/tsduck.lib)
		message(FATAL_ERROR "tsduck must build tsduck dll first ${TSDUCK_LIB_DIR}")
	endif()

elseif (UNUX)
	set(TSDUCK_LIB_DIR	/home/richie/Downloads/tsduck-3.29-2651/bin/release-x86_64-ubuntu/)
	
	if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${TSDUCK_LIB_DIR}/libtsduck.so)
		message(FATAL_ERROR "tsduck must build tsduck library first ${TSDUCK_LIB_DIR}")
	endif()
endif()

add_subdirectory(${SDK2_DIR} vatek_sdk_2.out)

set(TSDUCK_DIR_ROOT 
	${TSDUCK_DIR}/src/libtsduck/)
	
set(TSDUCK_DIR_BASE 
	${TSDUCK_DIR}/src/libtsduck/base)
	
set(TSDUCK_DIR_PLUGIN 
	${TSDUCK_DIR}/src/libtsduck/plugins
	${TSDUCK_DIR}/src/libtsduck/plugins/plugins
	${TSDUCK_DIR}/src/libtsduck/plugins/infra)
	
set(TSDUCK_DIR_DTV
	${TSDUCK_DIR}/src/libtsduck/dtv
	${TSDUCK_DIR}/src/libtsduck/dtv/standards
	${TSDUCK_DIR}/src/libtsduck/dtv/transport
	${TSDUCK_DIR}/src/libtsduck/dtv/charset
	${TSDUCK_DIR}/src/libtsduck/dtv/signalization)

set(TSDUCK_VATEK_PLUGIN_SRC
	src/tsduck_vatek_output.cpp)
	
set(TSDUCK_VATEK_PLUGIN_INC
	inc/tsduck_vatek_output.h)
	
set(TSDUCK_VATEK_TOOL_SRC 
	src/tsduck_vatek_main.cpp)

## tsvatek ##

add_executable(${TSDUCK_VATEK_TOOL} ${TSDUCK_VATEK_TOOL_SRC})
add_dependencies(${TSDUCK_VATEK_TOOL} libvatek_core_static)
target_include_directories(${TSDUCK_VATEK_TOOL} PRIVATE
						   ${SDK2_DIR}/api/core/inc
						   ${TSDUCK_DIR_ROOT} 
						   ${TSDUCK_DIR_BASE}
						   ${TSDUCK_DIR_BASE}/app
						   ${TSDUCK_DIR_BASE}/report
						   ${TSDUCK_DIR_BASE}/types
						   ${TSDUCK_DIR_BASE}/system
						   ${TSDUCK_DIR_BASE}/algo
						   ${TSDUCK_DIR_PLUGIN}
						   ${TSDUCK_DIR_DTV})

if(MSVC)
	target_compile_definitions(${TSDUCK_VATEK_TOOL} PRIVATE -D_TSDUCKDLL_USE -D_VA2_STATIC_)
	target_link_directories(${TSDUCK_VATEK_TOOL} PRIVATE ${TSDUCK_LIB_DIR})
	
	set_target_properties(${TSDUCK_VATEK_TOOL} PROPERTIES LINK_FLAGS /SUBSYSTEM:CONSOLE)
    target_compile_options(${TSDUCK_VATEK_TOOL} PRIVATE /wd26495 PRIVATE /wd26812 PRIVATE /wd26498)
    set_property(TARGET ${TSDUCK_VATEK_TOOL} PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
	
	target_link_libraries(${TSDUCK_VATEK_TOOL} PRIVATE 
						  tsduck.lib
						  libvatek_core_static)
    source_group("src" FILES ${TSDUCK_VATEK_TOOL_SRC})

elseif(UNIX AND NOT APPLE)
    target_compile_definitions(libvatek_core PRIVATE -D_VA2_DLL_)
    target_link_libraries(libvatek_core PRIVATE usb-1.0)
    target_link_libraries(libvatek_core PRIVATE ${CMAKE_DL_LIBS})

    set_target_properties(libvatek_core PROPERTIES
                          ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
                          LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
                          RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
                          
    target_link_directories(${TSDUCK_VATEK_TOOL} PRIVATE ${TSDUCK_LIB_DIR})
	
    target_link_libraries(${TSDUCK_VATEK_TOOL} PRIVATE 
						  libtsduck.so
						  libtsduck.a
						  libvatek_core
						  usb-1.0)
    source_group("src" FILES ${TSDUCK_VATEK_TOOL_SRC})

elseif(APPLE)
    target_compile_definitions(libvatek_core PRIVATE -D_VA2_DLL_)
    target_link_libraries(libvatek_core PRIVATE usb-1.0)
    target_link_libraries(libvatek_core PRIVATE ${CMAKE_DL_LIBS})

    include_directories("/usr/local/Cellar/libusb/1.0.26/include")
    target_link_directories(libvatek_core PRIVATE "/usr/local/Cellar/libusb/1.0.26/lib")

    set_target_properties(libvatek_core PROPERTIES
                          ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
                          LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
                          RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
                          
    target_link_directories(${TSDUCK_VATEK_TOOL} PRIVATE ${TSDUCK_LIB_DIR})
	
    target_link_libraries(${TSDUCK_VATEK_TOOL} PRIVATE 
						  libtsduck.so
						  libtsduck.a
						  libvatek_core
						  usb-1.0)
    source_group("src" FILES ${TSDUCK_VATEK_TOOL_SRC})
endif()

## tsplugin_vatek ##

add_library(${TSDUCK_VATEK_PLUGIN} SHARED ${TSDUCK_VATEK_PLUGIN_SRC} ${TSDUCK_VATEK_PLUGIN_INC})
add_dependencies(${TSDUCK_VATEK_PLUGIN} libvatek_core_static)
target_include_directories(${TSDUCK_VATEK_PLUGIN} PRIVATE 
						   ${SDK2_DIR}/api/core/inc
						   ${TSDUCK_DIR_ROOT} 
						   ${TSDUCK_DIR_BASE}
						   ${TSDUCK_DIR_BASE}/app
						   ${TSDUCK_DIR_BASE}/report
						   ${TSDUCK_DIR_BASE}/types
						   ${TSDUCK_DIR_BASE}/system
						   ${TSDUCK_DIR_BASE}/algo
						   ${TSDUCK_DIR_PLUGIN}
						   ${TSDUCK_DIR_DTV})

if(MSVC)
	target_compile_definitions(${TSDUCK_VATEK_PLUGIN} PRIVATE -D_TSDUCKDLL_USE -D_VA2_STATIC_)
	target_link_directories(${TSDUCK_VATEK_PLUGIN} PRIVATE ${TSDUCK_LIB_DIR})
	
	set_target_properties(${TSDUCK_VATEK_PLUGIN} PROPERTIES LINK_FLAGS /SUBSYSTEM:WINDOWS)
    target_compile_options(${TSDUCK_VATEK_PLUGIN} PRIVATE /wd26495 PRIVATE /wd26812 PRIVATE /wd26498)
    set_property(TARGET ${TSDUCK_VATEK_PLUGIN} PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
	
	target_link_libraries(${TSDUCK_VATEK_PLUGIN} PRIVATE 
						  tsduck.lib
						  libvatek_core_static)
				
	source_group("inc" FILES ${TSDUCK_VATEK_PLUGIN_INC})
    source_group("src" FILES ${TSDUCK_VATEK_PLUGIN_SRC})
    
elseif(UNIX AND NOT APPLE)
    target_compile_definitions(libvatek_core PRIVATE -D_VA2_DLL_)
    target_link_libraries(libvatek_core PRIVATE usb-1.0)
    target_link_libraries(libvatek_core PRIVATE ${CMAKE_DL_LIBS})

    set_target_properties(libvatek_core PROPERTIES
                          ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
                          LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
                          RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
                          
    target_link_directories(${TSDUCK_VATEK_PLUGIN} PRIVATE ${TSDUCK_LIB_DIR})
	
    target_link_libraries(${TSDUCK_VATEK_PLUGIN} PRIVATE 
						  libtsduck.so
						  libtsduck.a
						  libvatek_core
						  usb-1.0)
				
    source_group("inc" FILES ${TSDUCK_VATEK_PLUGIN_INC})
    source_group("src" FILES ${TSDUCK_VATEK_PLUGIN_SRC})

elseif(APPLE)
    target_compile_definitions(libvatek_core PRIVATE -D_VA2_DLL_)
    target_link_libraries(libvatek_core PRIVATE usb-1.0)
    target_link_libraries(libvatek_core PRIVATE ${CMAKE_DL_LIBS})
    
    target_link_directories(libvatek_core PRIVATE "/usr/local/Cellar/libusb/1.0.26/lib")
    include_directories("/usr/local/Cellar/libusb/1.0.26/include")

    set_target_properties(libvatek_core PROPERTIES
                          ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
                          LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
                          RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
                          
    target_link_directories(${TSDUCK_VATEK_PLUGIN} PRIVATE ${TSDUCK_LIB_DIR})
	
    target_link_libraries(${TSDUCK_VATEK_PLUGIN} PRIVATE 
						  libtsduck.so
						  libtsduck.a
						  libvatek_core
						  usb-1.0)
				
    source_group("inc" FILES ${TSDUCK_VATEK_PLUGIN_INC})
    source_group("src" FILES ${TSDUCK_VATEK_PLUGIN_SRC})
endif()


