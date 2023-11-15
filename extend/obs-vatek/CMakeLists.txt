cmake_minimum_required(VERSION 3.16)

project(obs-vatek)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(SDK2_DIR "../../" CACHE STRING "default vatek_sdk_2 folder")
set(SDK2_EN_STATIC_ONLY ON)
set(OBS_DIR "../../../" CACHE STRING "default obs-studio folder")

if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${SDK2_DIR}/)
    message(FATAL_ERROR "vatek_sdk_2 not found please set SDK2_DIR before config")
endif()

get_filename_component(OBS_DIR "${OBS_DIR}" REALPATH BASE_DIR "${CMAKE_SOURCE_DIR}")


set(QTDIR ${OBS_DIR}/obs-build-dependencies/windows-deps-2022-05-23-x64/lib CACHE STRING "")
set(DepsPath ${OBS_DIR}/obs-build-dependencies/windows-deps-2022-05-23-x64/include CACHE STRING "")
set(obsPath ${OBS_DIR}/obs-studio CACHE STRING "")
set(LibObs_DIR ${OBS_DIR}/obs-studio/build64/libobs CACHE STRING "")
set(OBS_FRONTEND_LIB ${OBS_DIR}/obs-studio/build64/UI/obs-frontend-api/RelWithDebInfo/obs-frontend-api.lib CACHE STRING "")
set(w32-pthreads_DIR ${OBS_DIR}/obs-studio/build64/deps/w32-pthreads CACHE STRING "")

#overwrite vatek_sdk_2 config
option(SDK2_EN_APP "" OFF)
option(SDK2_EN_SAMPLE "" OFF)
option(SDK2_EN_QT "" ON)
set(SDK2_QTDIR ${QTDIR} CACHE STRING "default QT_DIR")
add_subdirectory(${SDK2_DIR} vatek_sdk_2.out)
#end-section 

if (WIN32 OR APPLE)
    include(cmake/ObsPluginHelpers.cmake)
endif()

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)


find_package(LibObs REQUIRED PATH LibObs_DIR)
find_package(Qt5 COMPONENTS Widgets Core Gui REQUIRED)

set(PLUGIN_SOURCES
    source/src/obs_vatek.cpp
    source/src/obs_vatek_handle.cpp
	source/src/obs_vatek_output.cpp)
	
set(PLUGIN_SOURCES_CODEC
    source/src/ovatek_codec.cpp
    source/src/ovatek_codec_param.cpp
    source/src/ovatek_codec_aac.cpp
    source/src/ovatek_codec_qsv.cpp
	source/src/ovatek_codec_nvenc.cpp
	source/src/ovatek_ffmpeg.cpp
    source/src/ovatek_ffmpeg_video.cpp
    source/src/ovatek_ffmpeg_audio.cpp
	source/src/ovatek_codec_stream.cpp
    source/src/ovatek_frame.cpp
	source/src/ovatek_plugin_output.cpp)

set(PLUGIN_HEADERS
    source/inc/obs_vatek.h
    source/inc/codec/ovatek_codec.h
    source/inc/codec/ovatek_codec_qsv.h
	source/inc/codec/ovatek_codec_nvenc.h
	source/inc/codec/ovatek_codec_aac.h
	source/inc/codec/ffmpeg/ovatek_ffmpeg.h
    source/inc/codec/ffmpeg/ovatek_ffmpeg_nvenc.h
    source/inc/codec/ffmpeg/ovatek_ffmpeg_qsv.h
    source/inc/codec/ffmpeg/ovatek_ffmpeg_sw.h
    source/inc/ui/ovatek_ui_codec.h
    source/inc/ui/ovatek_ui_ffmpeg.h)

if(MSVC)
    source_group("plugin\\inc" FILES ${PLUGIN_HEADERS})
    source_group("plugin\\src" FILES ${PLUGIN_SOURCES})
    source_group("plugin\\src\\internal" FILES ${PLUGIN_INTERNAL})
	source_group("plugin\\src\\codec" FILES ${PLUGIN_SOURCES_CODEC})
endif()

add_library(${CMAKE_PROJECT_NAME} MODULE ${PLUGIN_SOURCES} ${PLUGIN_SOURCES_CODEC} ${PLUGIN_HEADERS})
add_dependencies(${CMAKE_PROJECT_NAME} libvatek_core_static libvatek_qt_static)
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE -D_VA2_STATIC_ PRIVATE -DLIBVATEK_QT_STATIC)

target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    "source/inc"
    "${SDK2_DIR}/api/core/inc"
    "${SDK2_DIR}/api/qt/inc"
	"${LIBOBS_INCLUDE_DIR}/../UI/obs-frontend-api"
    "${obsPath}/libobs"
    "${obsPath}/UI/obs-frontend-api"
    "${DepsPath}"
    ${Qt5Gui_INCLUDES}
	${Qt5Core_INCLUDES}
	${Qt5Widgets_INCLUDES})

if(MSVC)
    if(NOT DEFINED OBS_FRONTEND_LIB)
        set(OBS_FRONTEND_LIB "OBS_FRONTEND_LIB-NOTFOUND" CACHE FILEPATH "OBS frontend library")
        message(FATAL_ERROR "Could not find OBS Frontend API\'s library !")
    endif()

    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        target_link_directories(${CMAKE_PROJECT_NAME} PRIVATE "${SDK2_DIR}/build/deps/win/libusb/1.0.24/VS2019/MS64/static")
    else()
		target_link_options(${CMAKE_PROJECT_NAME} PRIVATE /SAFESEH:NO)
        target_link_directories(${CMAKE_PROJECT_NAME} PRIVATE "${SDK2_DIR}/build/deps/win/libusb/1.0.24/VS2019/MS32/static")
    endif()
    set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES LINK_FLAGS /SUBSYSTEM:WINDOWS)
    target_compile_options(${CMAKE_PROJECT_NAME} PRIVATE /MP PRIVATE /d2FH4-)
    target_compile_options(${CMAKE_PROJECT_NAME} PRIVATE /wd26495 PRIVATE /wd26812 PRIVATE /wd26498)
    set_property(TARGET ${CMAKE_PROJECT_NAME} PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

target_link_libraries(${CMAKE_PROJECT_NAME}
	"${obsPath}/build64/libobs/RelWithDebInfo/obs.lib"
    "${QTDIR}/avcodec.lib"
    "${QTDIR}/avutil.lib"
    "${OBS_FRONTEND_LIB}"
    libvatek_core_static
    libvatek_qt_static
    Qt5::Gui
	Qt5::Core
	Qt5::Widgets
    libusb-1.0.lib
)
