################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
D:/vatek/master/vatek_sdk_2/api/core/src/core/calibration_define.c \
D:/vatek/master/vatek_sdk_2/api/core/src/core/chip_define.c \
D:/vatek/master/vatek_sdk_2/api/core/src/core/device_usb.c \
D:/vatek/master/vatek_sdk_2/api/core/src/core/output_modulator.c \
D:/vatek/master/vatek_sdk_2/api/core/src/core/output_rfmixer.c \
D:/vatek/master/vatek_sdk_2/api/core/src/core/storage_default.c \
D:/vatek/master/vatek_sdk_2/api/core/src/core/storage_section.c \
D:/vatek/master/vatek_sdk_2/api/core/src/core/stream_base.c \
D:/vatek/master/vatek_sdk_2/api/core/src/core/stream_encoder.c \
D:/vatek/master/vatek_sdk_2/api/core/src/core/tool_8vsb.c \
D:/vatek/master/vatek_sdk_2/api/core/src/core/tool_binary_crc.c \
D:/vatek/master/vatek_sdk_2/api/core/src/core/tool_bufstream.c \
D:/vatek/master/vatek_sdk_2/api/core/src/core/tool_crc32.c \
D:/vatek/master/vatek_sdk_2/api/core/src/core/tool_dtmb.c \
D:/vatek/master/vatek_sdk_2/api/core/src/core/tool_dvb.c \
D:/vatek/master/vatek_sdk_2/api/core/src/core/tool_dvb_t2.c \
D:/vatek/master/vatek_sdk_2/api/core/src/core/tool_helpful_bitstream.c \
D:/vatek/master/vatek_sdk_2/api/core/src/core/tool_helpful_circlebuf.c \
D:/vatek/master/vatek_sdk_2/api/core/src/core/tool_helpful_fifo.c \
D:/vatek/master/vatek_sdk_2/api/core/src/core/tool_helpful_mempool.c \
D:/vatek/master/vatek_sdk_2/api/core/src/core/tool_helpful_memquery.c \
D:/vatek/master/vatek_sdk_2/api/core/src/core/tool_helpful_objectlist.c \
D:/vatek/master/vatek_sdk_2/api/core/src/core/tool_isdb_t.c \
D:/vatek/master/vatek_sdk_2/api/core/src/core/tool_j83abc.c \
D:/vatek/master/vatek_sdk_2/api/core/src/core/tool_ofdm.c \
D:/vatek/master/vatek_sdk_2/api/core/src/core/tool_output.c \
D:/vatek/master/vatek_sdk_2/api/core/src/core/ui_props_api.c 

OBJS += \
./sdk_core/calibration_define.o \
./sdk_core/chip_define.o \
./sdk_core/device_usb.o \
./sdk_core/output_modulator.o \
./sdk_core/output_rfmixer.o \
./sdk_core/storage_default.o \
./sdk_core/storage_section.o \
./sdk_core/stream_base.o \
./sdk_core/stream_encoder.o \
./sdk_core/tool_8vsb.o \
./sdk_core/tool_binary_crc.o \
./sdk_core/tool_bufstream.o \
./sdk_core/tool_crc32.o \
./sdk_core/tool_dtmb.o \
./sdk_core/tool_dvb.o \
./sdk_core/tool_dvb_t2.o \
./sdk_core/tool_helpful_bitstream.o \
./sdk_core/tool_helpful_circlebuf.o \
./sdk_core/tool_helpful_fifo.o \
./sdk_core/tool_helpful_mempool.o \
./sdk_core/tool_helpful_memquery.o \
./sdk_core/tool_helpful_objectlist.o \
./sdk_core/tool_isdb_t.o \
./sdk_core/tool_j83abc.o \
./sdk_core/tool_ofdm.o \
./sdk_core/tool_output.o \
./sdk_core/ui_props_api.o 

C_DEPS += \
./sdk_core/calibration_define.d \
./sdk_core/chip_define.d \
./sdk_core/device_usb.d \
./sdk_core/output_modulator.d \
./sdk_core/output_rfmixer.d \
./sdk_core/storage_default.d \
./sdk_core/storage_section.d \
./sdk_core/stream_base.d \
./sdk_core/stream_encoder.d \
./sdk_core/tool_8vsb.d \
./sdk_core/tool_binary_crc.d \
./sdk_core/tool_bufstream.d \
./sdk_core/tool_crc32.d \
./sdk_core/tool_dtmb.d \
./sdk_core/tool_dvb.d \
./sdk_core/tool_dvb_t2.d \
./sdk_core/tool_helpful_bitstream.d \
./sdk_core/tool_helpful_circlebuf.d \
./sdk_core/tool_helpful_fifo.d \
./sdk_core/tool_helpful_mempool.d \
./sdk_core/tool_helpful_memquery.d \
./sdk_core/tool_helpful_objectlist.d \
./sdk_core/tool_isdb_t.d \
./sdk_core/tool_j83abc.d \
./sdk_core/tool_ofdm.d \
./sdk_core/tool_output.d \
./sdk_core/ui_props_api.d 


# Each subdirectory must supply rules for building sources it contributes
sdk_core/calibration_define.o: D:/vatek/master/vatek_sdk_2/api/core/src/core/calibration_define.c sdk_core/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F401xC -c -I../Core/Inc -I../USB_DEVICE/App -I../USB_DEVICE/Target -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CustomHID/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../../stm32/core/inc -I../../../api/inc -I"D:/SDK/Code/SDK_master_20220111/master/vatek_sdk_2/api/core/inc/core" -I"D:/SDK/Code/SDK_master_20220111/master/vatek_sdk_2/api/core/inc" -O0 -ffunction-sections -fdata-sections -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
sdk_core/chip_define.o: D:/vatek/master/vatek_sdk_2/api/core/src/core/chip_define.c sdk_core/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F401xC -c -I../Core/Inc -I../USB_DEVICE/App -I../USB_DEVICE/Target -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CustomHID/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../../stm32/core/inc -I../../../api/inc -I"D:/SDK/Code/SDK_master_20220111/master/vatek_sdk_2/api/core/inc/core" -I"D:/SDK/Code/SDK_master_20220111/master/vatek_sdk_2/api/core/inc" -O0 -ffunction-sections -fdata-sections -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
sdk_core/device_usb.o: D:/vatek/master/vatek_sdk_2/api/core/src/core/device_usb.c sdk_core/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F401xC -c -I../Core/Inc -I../USB_DEVICE/App -I../USB_DEVICE/Target -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CustomHID/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../../stm32/core/inc -I../../../api/inc -I"D:/SDK/Code/SDK_master_20220111/master/vatek_sdk_2/api/core/inc/core" -I"D:/SDK/Code/SDK_master_20220111/master/vatek_sdk_2/api/core/inc" -O0 -ffunction-sections -fdata-sections -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
sdk_core/output_modulator.o: D:/vatek/master/vatek_sdk_2/api/core/src/core/output_modulator.c sdk_core/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F401xC -c -I../Core/Inc -I../USB_DEVICE/App -I../USB_DEVICE/Target -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CustomHID/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../../stm32/core/inc -I../../../api/inc -I"D:/SDK/Code/SDK_master_20220111/master/vatek_sdk_2/api/core/inc/core" -I"D:/SDK/Code/SDK_master_20220111/master/vatek_sdk_2/api/core/inc" -O0 -ffunction-sections -fdata-sections -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
sdk_core/output_rfmixer.o: D:/vatek/master/vatek_sdk_2/api/core/src/core/output_rfmixer.c sdk_core/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F401xC -c -I../Core/Inc -I../USB_DEVICE/App -I../USB_DEVICE/Target -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CustomHID/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../../stm32/core/inc -I../../../api/inc -I"D:/SDK/Code/SDK_master_20220111/master/vatek_sdk_2/api/core/inc/core" -I"D:/SDK/Code/SDK_master_20220111/master/vatek_sdk_2/api/core/inc" -O0 -ffunction-sections -fdata-sections -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
sdk_core/storage_default.o: D:/vatek/master/vatek_sdk_2/api/core/src/core/storage_default.c sdk_core/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F401xC -c -I../Core/Inc -I../USB_DEVICE/App -I../USB_DEVICE/Target -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CustomHID/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../../stm32/core/inc -I../../../api/inc -I"D:/SDK/Code/SDK_master_20220111/master/vatek_sdk_2/api/core/inc/core" -I"D:/SDK/Code/SDK_master_20220111/master/vatek_sdk_2/api/core/inc" -O0 -ffunction-sections -fdata-sections -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
sdk_core/storage_section.o: D:/vatek/master/vatek_sdk_2/api/core/src/core/storage_section.c sdk_core/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F401xC -c -I../Core/Inc -I../USB_DEVICE/App -I../USB_DEVICE/Target -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CustomHID/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../../stm32/core/inc -I../../../api/inc -I"D:/SDK/Code/SDK_master_20220111/master/vatek_sdk_2/api/core/inc/core" -I"D:/SDK/Code/SDK_master_20220111/master/vatek_sdk_2/api/core/inc" -O0 -ffunction-sections -fdata-sections -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
sdk_core/stream_base.o: D:/vatek/master/vatek_sdk_2/api/core/src/core/stream_base.c sdk_core/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F401xC -c -I../Core/Inc -I../USB_DEVICE/App -I../USB_DEVICE/Target -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CustomHID/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../../stm32/core/inc -I../../../api/inc -I"D:/SDK/Code/SDK_master_20220111/master/vatek_sdk_2/api/core/inc/core" -I"D:/SDK/Code/SDK_master_20220111/master/vatek_sdk_2/api/core/inc" -O0 -ffunction-sections -fdata-sections -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
sdk_core/stream_encoder.o: D:/vatek/master/vatek_sdk_2/api/core/src/core/stream_encoder.c sdk_core/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F401xC -c -I../Core/Inc -I../USB_DEVICE/App -I../USB_DEVICE/Target -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CustomHID/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../../stm32/core/inc -I../../../api/inc -I"D:/SDK/Code/SDK_master_20220111/master/vatek_sdk_2/api/core/inc/core" -I"D:/SDK/Code/SDK_master_20220111/master/vatek_sdk_2/api/core/inc" -O0 -ffunction-sections -fdata-sections -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
sdk_core/tool_8vsb.o: D:/vatek/master/vatek_sdk_2/api/core/src/core/tool_8vsb.c sdk_core/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F401xC -c -I../Core/Inc -I../USB_DEVICE/App -I../USB_DEVICE/Target -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CustomHID/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../../stm32/core/inc -I../../../api/inc -I"D:/SDK/Code/SDK_master_20220111/master/vatek_sdk_2/api/core/inc/core" -I"D:/SDK/Code/SDK_master_20220111/master/vatek_sdk_2/api/core/inc" -O0 -ffunction-sections -fdata-sections -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
sdk_core/tool_binary_crc.o: D:/vatek/master/vatek_sdk_2/api/core/src/core/tool_binary_crc.c sdk_core/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F401xC -c -I../Core/Inc -I../USB_DEVICE/App -I../USB_DEVICE/Target -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CustomHID/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../../stm32/core/inc -I../../../api/inc -I"D:/SDK/Code/SDK_master_20220111/master/vatek_sdk_2/api/core/inc/core" -I"D:/SDK/Code/SDK_master_20220111/master/vatek_sdk_2/api/core/inc" -O0 -ffunction-sections -fdata-sections -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
sdk_core/tool_bufstream.o: D:/vatek/master/vatek_sdk_2/api/core/src/core/tool_bufstream.c sdk_core/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F401xC -c -I../Core/Inc -I../USB_DEVICE/App -I../USB_DEVICE/Target -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CustomHID/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../../stm32/core/inc -I../../../api/inc -I"D:/SDK/Code/SDK_master_20220111/master/vatek_sdk_2/api/core/inc/core" -I"D:/SDK/Code/SDK_master_20220111/master/vatek_sdk_2/api/core/inc" -O0 -ffunction-sections -fdata-sections -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
sdk_core/tool_crc32.o: D:/vatek/master/vatek_sdk_2/api/core/src/core/tool_crc32.c sdk_core/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F401xC -c -I../Core/Inc -I../USB_DEVICE/App -I../USB_DEVICE/Target -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CustomHID/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../../stm32/core/inc -I../../../api/inc -I"D:/SDK/Code/SDK_master_20220111/master/vatek_sdk_2/api/core/inc/core" -I"D:/SDK/Code/SDK_master_20220111/master/vatek_sdk_2/api/core/inc" -O0 -ffunction-sections -fdata-sections -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
sdk_core/tool_dtmb.o: D:/vatek/master/vatek_sdk_2/api/core/src/core/tool_dtmb.c sdk_core/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F401xC -c -I../Core/Inc -I../USB_DEVICE/App -I../USB_DEVICE/Target -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CustomHID/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../../stm32/core/inc -I../../../api/inc -I"D:/SDK/Code/SDK_master_20220111/master/vatek_sdk_2/api/core/inc/core" -I"D:/SDK/Code/SDK_master_20220111/master/vatek_sdk_2/api/core/inc" -O0 -ffunction-sections -fdata-sections -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
sdk_core/tool_dvb.o: D:/vatek/master/vatek_sdk_2/api/core/src/core/tool_dvb.c sdk_core/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F401xC -c -I../Core/Inc -I../USB_DEVICE/App -I../USB_DEVICE/Target -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CustomHID/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../../stm32/core/inc -I../../../api/inc -I"D:/SDK/Code/SDK_master_20220111/master/vatek_sdk_2/api/core/inc/core" -I"D:/SDK/Code/SDK_master_20220111/master/vatek_sdk_2/api/core/inc" -O0 -ffunction-sections -fdata-sections -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
sdk_core/tool_dvb_t2.o: D:/vatek/master/vatek_sdk_2/api/core/src/core/tool_dvb_t2.c sdk_core/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F401xC -c -I../Core/Inc -I../USB_DEVICE/App -I../USB_DEVICE/Target -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CustomHID/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../../stm32/core/inc -I../../../api/inc -I"D:/SDK/Code/SDK_master_20220111/master/vatek_sdk_2/api/core/inc/core" -I"D:/SDK/Code/SDK_master_20220111/master/vatek_sdk_2/api/core/inc" -O0 -ffunction-sections -fdata-sections -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
sdk_core/tool_helpful_bitstream.o: D:/vatek/master/vatek_sdk_2/api/core/src/core/tool_helpful_bitstream.c sdk_core/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F401xC -c -I../Core/Inc -I../USB_DEVICE/App -I../USB_DEVICE/Target -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CustomHID/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../../stm32/core/inc -I../../../api/inc -I"D:/SDK/Code/SDK_master_20220111/master/vatek_sdk_2/api/core/inc/core" -I"D:/SDK/Code/SDK_master_20220111/master/vatek_sdk_2/api/core/inc" -O0 -ffunction-sections -fdata-sections -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
sdk_core/tool_helpful_circlebuf.o: D:/vatek/master/vatek_sdk_2/api/core/src/core/tool_helpful_circlebuf.c sdk_core/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F401xC -c -I../Core/Inc -I../USB_DEVICE/App -I../USB_DEVICE/Target -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CustomHID/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../../stm32/core/inc -I../../../api/inc -I"D:/SDK/Code/SDK_master_20220111/master/vatek_sdk_2/api/core/inc/core" -I"D:/SDK/Code/SDK_master_20220111/master/vatek_sdk_2/api/core/inc" -O0 -ffunction-sections -fdata-sections -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
sdk_core/tool_helpful_fifo.o: D:/vatek/master/vatek_sdk_2/api/core/src/core/tool_helpful_fifo.c sdk_core/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F401xC -c -I../Core/Inc -I../USB_DEVICE/App -I../USB_DEVICE/Target -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CustomHID/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../../stm32/core/inc -I../../../api/inc -I"D:/SDK/Code/SDK_master_20220111/master/vatek_sdk_2/api/core/inc/core" -I"D:/SDK/Code/SDK_master_20220111/master/vatek_sdk_2/api/core/inc" -O0 -ffunction-sections -fdata-sections -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
sdk_core/tool_helpful_mempool.o: D:/vatek/master/vatek_sdk_2/api/core/src/core/tool_helpful_mempool.c sdk_core/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F401xC -c -I../Core/Inc -I../USB_DEVICE/App -I../USB_DEVICE/Target -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CustomHID/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../../stm32/core/inc -I../../../api/inc -I"D:/SDK/Code/SDK_master_20220111/master/vatek_sdk_2/api/core/inc/core" -I"D:/SDK/Code/SDK_master_20220111/master/vatek_sdk_2/api/core/inc" -O0 -ffunction-sections -fdata-sections -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
sdk_core/tool_helpful_memquery.o: D:/vatek/master/vatek_sdk_2/api/core/src/core/tool_helpful_memquery.c sdk_core/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F401xC -c -I../Core/Inc -I../USB_DEVICE/App -I../USB_DEVICE/Target -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CustomHID/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../../stm32/core/inc -I../../../api/inc -I"D:/SDK/Code/SDK_master_20220111/master/vatek_sdk_2/api/core/inc/core" -I"D:/SDK/Code/SDK_master_20220111/master/vatek_sdk_2/api/core/inc" -O0 -ffunction-sections -fdata-sections -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
sdk_core/tool_helpful_objectlist.o: D:/vatek/master/vatek_sdk_2/api/core/src/core/tool_helpful_objectlist.c sdk_core/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F401xC -c -I../Core/Inc -I../USB_DEVICE/App -I../USB_DEVICE/Target -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CustomHID/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../../stm32/core/inc -I../../../api/inc -I"D:/SDK/Code/SDK_master_20220111/master/vatek_sdk_2/api/core/inc/core" -I"D:/SDK/Code/SDK_master_20220111/master/vatek_sdk_2/api/core/inc" -O0 -ffunction-sections -fdata-sections -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
sdk_core/tool_isdb_t.o: D:/vatek/master/vatek_sdk_2/api/core/src/core/tool_isdb_t.c sdk_core/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F401xC -c -I../Core/Inc -I../USB_DEVICE/App -I../USB_DEVICE/Target -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CustomHID/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../../stm32/core/inc -I../../../api/inc -I"D:/SDK/Code/SDK_master_20220111/master/vatek_sdk_2/api/core/inc/core" -I"D:/SDK/Code/SDK_master_20220111/master/vatek_sdk_2/api/core/inc" -O0 -ffunction-sections -fdata-sections -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
sdk_core/tool_j83abc.o: D:/vatek/master/vatek_sdk_2/api/core/src/core/tool_j83abc.c sdk_core/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F401xC -c -I../Core/Inc -I../USB_DEVICE/App -I../USB_DEVICE/Target -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CustomHID/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../../stm32/core/inc -I../../../api/inc -I"D:/SDK/Code/SDK_master_20220111/master/vatek_sdk_2/api/core/inc/core" -I"D:/SDK/Code/SDK_master_20220111/master/vatek_sdk_2/api/core/inc" -O0 -ffunction-sections -fdata-sections -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
sdk_core/tool_ofdm.o: D:/vatek/master/vatek_sdk_2/api/core/src/core/tool_ofdm.c sdk_core/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F401xC -c -I../Core/Inc -I../USB_DEVICE/App -I../USB_DEVICE/Target -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CustomHID/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../../stm32/core/inc -I../../../api/inc -I"D:/SDK/Code/SDK_master_20220111/master/vatek_sdk_2/api/core/inc/core" -I"D:/SDK/Code/SDK_master_20220111/master/vatek_sdk_2/api/core/inc" -O0 -ffunction-sections -fdata-sections -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
sdk_core/tool_output.o: D:/vatek/master/vatek_sdk_2/api/core/src/core/tool_output.c sdk_core/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F401xC -c -I../Core/Inc -I../USB_DEVICE/App -I../USB_DEVICE/Target -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CustomHID/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../../stm32/core/inc -I../../../api/inc -I"D:/SDK/Code/SDK_master_20220111/master/vatek_sdk_2/api/core/inc/core" -I"D:/SDK/Code/SDK_master_20220111/master/vatek_sdk_2/api/core/inc" -O0 -ffunction-sections -fdata-sections -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
sdk_core/ui_props_api.o: D:/vatek/master/vatek_sdk_2/api/core/src/core/ui_props_api.c sdk_core/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F401xC -c -I../Core/Inc -I../USB_DEVICE/App -I../USB_DEVICE/Target -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CustomHID/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../../stm32/core/inc -I../../../api/inc -I"D:/SDK/Code/SDK_master_20220111/master/vatek_sdk_2/api/core/inc/core" -I"D:/SDK/Code/SDK_master_20220111/master/vatek_sdk_2/api/core/inc" -O0 -ffunction-sections -fdata-sections -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-sdk_core

clean-sdk_core:
	-$(RM) ./sdk_core/calibration_define.d ./sdk_core/calibration_define.o ./sdk_core/calibration_define.su ./sdk_core/chip_define.d ./sdk_core/chip_define.o ./sdk_core/chip_define.su ./sdk_core/device_usb.d ./sdk_core/device_usb.o ./sdk_core/device_usb.su ./sdk_core/output_modulator.d ./sdk_core/output_modulator.o ./sdk_core/output_modulator.su ./sdk_core/output_rfmixer.d ./sdk_core/output_rfmixer.o ./sdk_core/output_rfmixer.su ./sdk_core/storage_default.d ./sdk_core/storage_default.o ./sdk_core/storage_default.su ./sdk_core/storage_section.d ./sdk_core/storage_section.o ./sdk_core/storage_section.su ./sdk_core/stream_base.d ./sdk_core/stream_base.o ./sdk_core/stream_base.su ./sdk_core/stream_encoder.d ./sdk_core/stream_encoder.o ./sdk_core/stream_encoder.su ./sdk_core/tool_8vsb.d ./sdk_core/tool_8vsb.o ./sdk_core/tool_8vsb.su ./sdk_core/tool_binary_crc.d ./sdk_core/tool_binary_crc.o ./sdk_core/tool_binary_crc.su ./sdk_core/tool_bufstream.d ./sdk_core/tool_bufstream.o ./sdk_core/tool_bufstream.su ./sdk_core/tool_crc32.d ./sdk_core/tool_crc32.o ./sdk_core/tool_crc32.su ./sdk_core/tool_dtmb.d ./sdk_core/tool_dtmb.o ./sdk_core/tool_dtmb.su ./sdk_core/tool_dvb.d ./sdk_core/tool_dvb.o ./sdk_core/tool_dvb.su ./sdk_core/tool_dvb_t2.d ./sdk_core/tool_dvb_t2.o ./sdk_core/tool_dvb_t2.su ./sdk_core/tool_helpful_bitstream.d ./sdk_core/tool_helpful_bitstream.o ./sdk_core/tool_helpful_bitstream.su ./sdk_core/tool_helpful_circlebuf.d ./sdk_core/tool_helpful_circlebuf.o ./sdk_core/tool_helpful_circlebuf.su ./sdk_core/tool_helpful_fifo.d ./sdk_core/tool_helpful_fifo.o ./sdk_core/tool_helpful_fifo.su ./sdk_core/tool_helpful_mempool.d ./sdk_core/tool_helpful_mempool.o ./sdk_core/tool_helpful_mempool.su ./sdk_core/tool_helpful_memquery.d ./sdk_core/tool_helpful_memquery.o ./sdk_core/tool_helpful_memquery.su ./sdk_core/tool_helpful_objectlist.d ./sdk_core/tool_helpful_objectlist.o ./sdk_core/tool_helpful_objectlist.su ./sdk_core/tool_isdb_t.d ./sdk_core/tool_isdb_t.o ./sdk_core/tool_isdb_t.su ./sdk_core/tool_j83abc.d ./sdk_core/tool_j83abc.o ./sdk_core/tool_j83abc.su ./sdk_core/tool_ofdm.d ./sdk_core/tool_ofdm.o ./sdk_core/tool_ofdm.su ./sdk_core/tool_output.d ./sdk_core/tool_output.o ./sdk_core/tool_output.su ./sdk_core/ui_props_api.d ./sdk_core/ui_props_api.o ./sdk_core/ui_props_api.su

.PHONY: clean-sdk_core

