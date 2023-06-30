cmake_minimum_required(VERSION 3.16)

project(libvatek_core)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(core_INC_BRIDGE
    inc/bridge/bridge_base.h
    inc/bridge/bridge_demod.h
    inc/bridge/bridge_device.h
    inc/bridge/bridge_rfmixer.h
    inc/bridge/bridge_source.h
    inc/bridge/bridge_usb.h
    inc/bridge/ui/ui_bridge_base.h
    inc/bridge/ui/ui_bridge_source.h)

set(core_INC_CORE
    inc/core/vatek_base.h
    inc/core/base/binary_rom.h
    inc/core/base/chip_define.h
    inc/core/base/device_usb.h
    inc/core/base/media_audio.h
    inc/core/base/media_video.h
    inc/core/base/output_base.h
    inc/core/base/output_modulator.h
    inc/core/base/output_rfmixer.h
    inc/core/base/stream_base.h
    inc/core/base/stream_encoder.h
    inc/core/base/calibration_define.h
    inc/core/hal/halreg_define.h
    inc/core/hal/halreg_mux.h
    inc/core/hal/halreg_output.h
    inc/core/hal/halreg_playload.h
    inc/core/hal/halreg_rfmixer.h
    inc/core/hal/halreg_stream.h
    inc/core/hal/halreg_calibration.h
    inc/core/hal/halservice_base.h
    inc/core/hal/halservice_broadcast.h
    inc/core/hal/halservice_rescure.h
    inc/core/hal/halservice_transform.h
    inc/core/storage/storage_broadcast.h
    inc/core/storage/storage_config.h
    inc/core/storage/storage_r2tune.h
    inc/core/storage/storage_section.h
    inc/core/storage/storage_transform.h
    inc/core/tools/tool_8vsb.h
    inc/core/tools/tool_binary_crc.h
    inc/core/tools/tool_bufstream.h
    inc/core/tools/tool_crc32.h
    inc/core/tools/tool_dvb.h
    inc/core/tools/tool_dvb_t2.h
    inc/core/tools/tool_dtmb.h
    inc/core/tools/tool_helpful.h
    inc/core/tools/tool_isdb_t.h
    inc/core/tools/tool_j83abc.h
    inc/core/tools/tool_ofdm.h
    inc/core/tools/tool_output.h
    inc/core/ui/ui_props_api.h
    inc/core/ui/ui_props_define.h
    inc/core/ui/ui_props/ui_props_chip.h
    inc/core/ui/ui_props/ui_props_encoder.h
    inc/core/ui/ui_props/ui_props_modulator.h
    inc/core/ui/ui_props/ui_props_output.h
    inc/core/ui/ui_props/ui_props_stream.h)

if(MSVC)
	set(core_INC_CROSS
    		inc/cross/cross_bridge.h
    		inc/cross/cross_os_api.h
    		inc/cross/cross_stream.h
    		inc/cross/cross_usb_device_winusb.h)
elseif(UNIX)
	set(core_INC_CROSS
    		inc/cross/cross_bridge.h
    		inc/cross/cross_os_api.h
    		inc/cross/cross_stream.h
    		inc/cross/cross_usb_device.h)
endif()

set(core_INC_MUX
    inc/mux/mux_define.h
    inc/mux/mux_epg.h
    inc/mux/mux_handle.h
    inc/mux/mux_packet.h
    inc/mux/mux_rule.h
    inc/mux/mux_spec.h
    inc/mux/mux_time.h
    inc/mux/spec/spec_arib.h
    inc/mux/spec/spec_dvb.h
    inc/mux/spec/spec_iso_13818.h
    inc/mux/spec/spec_psip.h
    inc/mux/ui/spec/ui_mux_spec_arib.h
    inc/mux/ui/spec/ui_mux_spec_dvb.h
    inc/mux/ui/spec/ui_mux_spec_iso.h
    inc/mux/ui/spec/ui_mux_spec_psip.h
    inc/mux/ui/ui_props_mux.h
    inc/mux/ui/ui_tool_arib.h
    inc/mux/ui/ui_tool_country.h)

set(core_INC_SERVICE
    inc/service/service_base.h
    inc/service/service_broadcast.h
    inc/service/service_transform.h
    inc/service/ui/ui_service_base.h
    inc/service/ui/ui_service_broadcast.h
    inc/service/ui/ui_service_transform.h)

set(core_INC_SDK
    inc/vatek_sdk_bridge.h
    inc/vatek_sdk_broadcast.h
    inc/vatek_sdk_device.h
    inc/vatek_sdk_storage.h
    inc/vatek_sdk_transform.h
    inc/vatek_sdk_usbmux.h
    inc/vatek_sdk_usbstream.h)

set(core_SRC_CORE
    src/core/chip_define.c
    src/core/calibration_define.c
    src/core/output_modulator.c
    src/core/output_rfmixer.c
    src/core/storage_section.c
    src/core/storage_default.c
    src/core/stream_base.c
    src/core/stream_encoder.c
    src/core/device_usb.c
    src/core/tool_binary_crc.c
    src/core/tool_bufstream.c
    src/core/tool_crc32.c
    src/core/tool_dvb.c
    src/core/tool_dvb_t2.c
    src/core/tool_dtmb.c
    src/core/tool_ofdm.c
    src/core/tool_helpful_bitstream.c
    src/core/tool_helpful_circlebuf.c
    src/core/tool_helpful_fifo.c
    src/core/tool_helpful_mempool.c
    src/core/tool_helpful_memquery.c
    src/core/tool_helpful_objectlist.c
    src/core/tool_isdb_t.c
    src/core/tool_j83abc.c
    src/core/tool_8vsb.c
    src/core/tool_output.c
    src/core/ui_props_api.c)

if(MSVC)
	set(core_SRC_CROSS
    		src/cross/cross_bridge.c
    		src/cross/cross_bridge_board.c
    		src/cross/cross_os_api.c
    		src/cross/cross_os_socket.c
    		src/cross/cross_stream.c
    		src/cross/cross_stream_file.c
    		src/cross/cross_stream_test.c
    		src/cross/cross_stream_udp.c
    		src/cross/cross_stream_mux.c
    		src/cross/cross_usb_device_winusb.c)
elseif(UNIX)
	set(core_SRC_CROSS
    		src/cross/cross_bridge.c
    		src/cross/cross_bridge_board.c
    		src/cross/cross_os_api.c
    		src/cross/cross_os_socket.c
    		src/cross/cross_stream.c
    		src/cross/cross_stream_file.c
    		src/cross/cross_stream_test.c
    		src/cross/cross_stream_udp.c
    		src/cross/cross_stream_mux.c
    		src/cross/cross_usb_device_libusb.c
    		src/cross/libusb_tool.c)
endif()

set(core_SRC_DEVICE
    src/device/cross_device_bridge.c
    src/device/cross_device_tool.c
    src/device/cross_device_usb.c
    src/device/mux_usbstream.c)

set(core_SRC_MUX
    src/mux/mux_epg.c
    src/mux/mux_hadnel_playload.c
    src/mux/mux_handle_broadcast.c
    src/mux/mux_handle_channel.c
    src/mux/mux_handle_core.c
    src/mux/mux_handle_raw.c
    src/mux/mux_packet_base.c
    src/mux/mux_packet_pes.c
    src/mux/mux_rule.c
    src/mux/mux_spec.c
    src/mux/mux_time.c
    src/mux/mux_tool_tick.c)

set(core_SRC_SERVICE
    src/service/service_base.c
    src/service/service_broadcast.c
    src/service/service_transform.c)

set(core_SRC_SDK
    src/vatek_sdk_base.c
    src/vatek_sdk_bridge.c
    src/vatek_sdk_broadcast.c
    src/vatek_sdk_device.c
    src/vatek_sdk_storage.c
    src/vatek_sdk_transform.c
    src/vatek_sdk_usbmux.c
    src/vatek_sdk_usbstream.c)

if(MSVC)
    set(core_SRC_OS
        src/cross/os/win/win_os_bridge.c
        src/cross/os/win/win_os_common.c
        src/cross/os/win/win_os_event_api.c
        src/cross/os/win/win_os_mutex_api.c
        src/cross/os/win/win_os_process.c
        src/cross/os/win/win_os_smem_api.c
        src/cross/os/win/win_os_sys_api.c
        src/cross/os/win/win_os_thread_api.c
        src/cross/os/win/win_os_time_api.c
        src/cross/os/win/win_os_uart_api.c)
elseif(UNIX)
    set(core_SRC_OS
        src/cross/os/linux/cross_bridge_libusb.c
        src/cross/os/linux/linux_core.c
        src/cross/os/linux/linux_dynamic_dll.c
        src/cross/os/linux/linux_event.c
        src/cross/os/linux/linux_mutex.c
        src/cross/os/linux/linux_process.c
        src/cross/os/linux/linux_smem.c
        src/cross/os/linux/linux_thread.c
        src/cross/os/linux/linux_time.c)
endif()

if($(core_SRC_OS))
    message(FATAL_ERROR "OS not supported [supported : win|linux]")
endif()

set(sdk2_SOURCES
    ${core_SRC_CORE}
    ${core_SRC_CROSS}
    ${core_SRC_DEVICE}
    ${core_SRC_MUX}
    ${core_SRC_OS}
    ${core_SRC_SDK}
    ${core_SRC_SERVICE})

set(sdk2_HEADERS
    ${core_INC_BRIDGE}
    ${core_INC_CORE}
    ${core_INC_CROSS}
    ${core_INC_MUX}
    ${core_INC_SDK}
    ${core_INC_SERVICE})

if(MSVC)
    source_group("inc/bridge" FILES ${core_INC_BRIDGE})
    source_group("inc/core" FILES ${core_INC_CORE})
    source_group("inc/cross" FILES ${core_INC_CROSS})
    source_group("inc/mux" FILES ${core_INC_MUX})
    source_group("inc/service" FILES ${core_INC_SERVICE})
    source_group("inc" FILES ${core_INC_SDK})

    source_group("src/core" FILES ${core_SRC_CORE})
    source_group("src/cross" FILES ${core_SRC_CROSS})
    source_group("src/device" FILES ${core_SRC_DEVICE})
    source_group("src/mux" FILES ${core_SRC_MUX})
    source_group("src/os" FILES ${core_SRC_OS})
    source_group("src/service" FILES ${core_SRC_SERVICE})
    source_group("src" FILES ${core_SRC_SDK})
endif()

include_directories("inc")

if(MSVC)
    add_definitions(-DUNICODE -D_UNICODE -D_CRT_SECURE_NO_WARNINGS -D_CRT_NONSTDC_NO_WARNINGS)

    # libvatek_core #
	
	if(NOT ${SDK2_EN_STATIC_ONLY})
        set(sdk2_RCFILE ${CMAKE_BINARY_DIR}/libvatek_core.rc)
        configure_file(./cmake/core.rc.in ${sdk2_RCFILE})

		add_library(libvatek_core SHARED ${sdk2_SOURCES} ${sdk2_HEADERS} ${sdk2_RCFILE})
		target_compile_definitions(libvatek_core PRIVATE -D_VA2_DLL_)
		target_compile_options(libvatek_core PRIVATE /wd26495 PRIVATE /wd26812 PRIVATE /wd26498)
		set_property(TARGET libvatek_core PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
		set_target_properties(libvatek_core PROPERTIES LINK_FLAGS "/LTCG /SUBSYSTEM:WINDOWS")
		set_target_properties(libvatek_core PROPERTIES 
							  ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
							  LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
							  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
	endif()

    # libvatek_core_static #
    add_library(libvatek_core_static STATIC ${sdk2_SOURCES} ${sdk2_HEADERS})
    target_compile_options(libvatek_core_static PRIVATE /wd26495 PRIVATE /wd26812 PRIVATE /wd26498)
    target_compile_definitions(libvatek_core_static PRIVATE -D_VA2_STATIC_)
    set_property(TARGET libvatek_core_static PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

    set_target_properties(libvatek_core_static PROPERTIES
                          ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
                          LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
                          RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
    
elseif(UNIX AND NOT APPLE)

	add_library(vatek_core SHARED ${sdk2_SOURCES} ${sdk2_HEADERS})
	target_compile_definitions(vatek_core PRIVATE -D_VA2_DLL_)
	target_link_libraries(vatek_core PRIVATE usb-1.0)
	target_link_libraries(vatek_core PRIVATE ${CMAKE_DL_LIBS})   

	set_target_properties(vatek_core PROPERTIES
		          ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
		          LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
		          RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")                	

	set(ORIGINAL_STATIC_LIB_NAME vatek_core_static)
	add_library(${ORIGINAL_STATIC_LIB_NAME} STATIC ${sdk2_SOURCES} ${sdk2_HEADERS})
	set_target_properties(${ORIGINAL_STATIC_LIB_NAME} PROPERTIES OUTPUT_NAME vatek_core)

	target_link_libraries(${ORIGINAL_STATIC_LIB_NAME} PRIVATE usb-1.0)
	target_link_libraries(${ORIGINAL_STATIC_LIB_NAME} PRIVATE ${CMAKE_DL_LIBS})

	set_target_properties(${ORIGINAL_STATIC_LIB_NAME} PROPERTIES
			  ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
		          LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
		          RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin") 
    
    	INSTALL(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/inc/." DESTINATION "${CMAKE_INSTALL_PREFIX}/include/vatek")
    			                        
	INSTALL(TARGETS vatek_core LIBRARY DESTINATION "${CMAKE_INSTALL_PREFIX}/lib")         
		
	INSTALL(TARGETS ${ORIGINAL_STATIC_LIB_NAME} ARCHIVE DESTINATION "${CMAKE_INSTALL_PREFIX}/lib")     
    
    
elseif(APPLE)    
    add_library(vatek_core SHARED ${sdk2_SOURCES} ${sdk2_HEADERS})
    target_compile_definitions(vatek_core PRIVATE -D_VA2_DLL_)
    target_link_libraries(vatek_core PRIVATE usb-1.0)
    target_link_libraries(vatek_core PRIVATE ${CMAKE_DL_LIBS})

    execute_process(COMMAND uname -m OUTPUT_VARIABLE OSX_ARCH OUTPUT_STRIP_TRAILING_WHITESPACE)
    if(OSX_ARCH STREQUAL "x86_64")
        include_directories("/usr/local/include")
        target_link_directories(vatek_core PRIVATE "/usr/local/lib")
    else()
        include_directories("/opt/homebrew/include")
        target_link_directories(vatek_core PRIVATE "/opt/homebrew/lib")
    endif()

    set_target_properties(vatek_core PROPERTIES
                          ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
                          LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
                          RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

	set(ORIGINAL_STATIC_LIB_NAME vatek_core_static)
	add_library(${ORIGINAL_STATIC_LIB_NAME} STATIC ${sdk2_SOURCES} ${sdk2_HEADERS})
	set_target_properties(${ORIGINAL_STATIC_LIB_NAME} PROPERTIES OUTPUT_NAME vatek_core)

	target_link_libraries(${ORIGINAL_STATIC_LIB_NAME} PRIVATE usb-1.0)
	target_link_libraries(${ORIGINAL_STATIC_LIB_NAME} PRIVATE ${CMAKE_DL_LIBS})

	set_target_properties(${ORIGINAL_STATIC_LIB_NAME} PROPERTIES
			    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
		        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
		        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin") 
    
    INSTALL(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/inc/." DESTINATION "${CMAKE_INSTALL_PREFIX}/include/vatek")
    			                        
    INSTALL(TARGETS vatek_core
            LIBRARY DESTINATION "${CMAKE_INSTALL_PREFIX}/lib")
 
	INSTALL(TARGETS ${ORIGINAL_STATIC_LIB_NAME} ARCHIVE DESTINATION "${CMAKE_INSTALL_PREFIX}/lib")     
    


endif()
