################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
D:/vatek/master/vatek_bridge_2/api/src/avl68xx_dvbc.c \
D:/vatek/master/vatek_bridge_2/api/src/avl68xx_dvbsx.c \
D:/vatek/master/vatek_bridge_2/api/src/avl68xx_dvbtx.c \
D:/vatek/master/vatek_bridge_2/api/src/avl68xx_internal.c \
D:/vatek/master/vatek_bridge_2/api/src/avl68xx_isdbt.c \
D:/vatek/master/vatek_bridge_2/api/src/avsource_adv718x.c \
D:/vatek/master/vatek_bridge_2/api/src/avsource_ep9351.c \
D:/vatek/master/vatek_bridge_2/api/src/avsource_ep9555e.c \
D:/vatek/master/vatek_bridge_2/api/src/avsource_sx1804a.c \
D:/vatek/master/vatek_bridge_2/api/src/bridge_internal.c \
D:/vatek/master/vatek_bridge_2/api/src/cmd_bridge.c \
D:/vatek/master/vatek_bridge_2/api/src/cmd_mod.c \
D:/vatek/master/vatek_bridge_2/api/src/demod_avl68xx.c \
D:/vatek/master/vatek_bridge_2/api/src/ep9xxx_tool.c \
D:/vatek/master/vatek_bridge_2/api/src/hal_bridge_board.c \
D:/vatek/master/vatek_bridge_2/api/src/hal_bridge_usb.c \
D:/vatek/master/vatek_bridge_2/api/src/hal_vatek_core.c \
D:/vatek/master/vatek_bridge_2/api/src/hdmi_tool.c \
D:/vatek/master/vatek_bridge_2/api/src/vatek_bridge_2.c 

OBJS += \
./bridge_api/src/avl68xx_dvbc.o \
./bridge_api/src/avl68xx_dvbsx.o \
./bridge_api/src/avl68xx_dvbtx.o \
./bridge_api/src/avl68xx_internal.o \
./bridge_api/src/avl68xx_isdbt.o \
./bridge_api/src/avsource_adv718x.o \
./bridge_api/src/avsource_ep9351.o \
./bridge_api/src/avsource_ep9555e.o \
./bridge_api/src/avsource_sx1804a.o \
./bridge_api/src/bridge_internal.o \
./bridge_api/src/cmd_bridge.o \
./bridge_api/src/cmd_mod.o \
./bridge_api/src/demod_avl68xx.o \
./bridge_api/src/ep9xxx_tool.o \
./bridge_api/src/hal_bridge_board.o \
./bridge_api/src/hal_bridge_usb.o \
./bridge_api/src/hal_vatek_core.o \
./bridge_api/src/hdmi_tool.o \
./bridge_api/src/vatek_bridge_2.o 

C_DEPS += \
./bridge_api/src/avl68xx_dvbc.d \
./bridge_api/src/avl68xx_dvbsx.d \
./bridge_api/src/avl68xx_dvbtx.d \
./bridge_api/src/avl68xx_internal.d \
./bridge_api/src/avl68xx_isdbt.d \
./bridge_api/src/avsource_adv718x.d \
./bridge_api/src/avsource_ep9351.d \
./bridge_api/src/avsource_ep9555e.d \
./bridge_api/src/avsource_sx1804a.d \
./bridge_api/src/bridge_internal.d \
./bridge_api/src/cmd_bridge.d \
./bridge_api/src/cmd_mod.d \
./bridge_api/src/demod_avl68xx.d \
./bridge_api/src/ep9xxx_tool.d \
./bridge_api/src/hal_bridge_board.d \
./bridge_api/src/hal_bridge_usb.d \
./bridge_api/src/hal_vatek_core.d \
./bridge_api/src/hdmi_tool.d \
./bridge_api/src/vatek_bridge_2.d 


# Each subdirectory must supply rules for building sources it contributes
bridge_api/src/avl68xx_dvbc.o: D:/vatek/master/vatek_bridge_2/api/src/avl68xx_dvbc.c bridge_api/src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F401xC -c -I../Core/Inc -I../USB_DEVICE/App -I../USB_DEVICE/Target -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CustomHID/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../../stm32/core/inc -I../../../api/inc -I"D:/SDK/Code/SDK_master_20220111/master/vatek_sdk_2/api/core/inc/core" -I"D:/SDK/Code/SDK_master_20220111/master/vatek_sdk_2/api/core/inc" -O0 -ffunction-sections -fdata-sections -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
bridge_api/src/avl68xx_dvbsx.o: D:/vatek/master/vatek_bridge_2/api/src/avl68xx_dvbsx.c bridge_api/src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F401xC -c -I../Core/Inc -I../USB_DEVICE/App -I../USB_DEVICE/Target -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CustomHID/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../../stm32/core/inc -I../../../api/inc -I"D:/SDK/Code/SDK_master_20220111/master/vatek_sdk_2/api/core/inc/core" -I"D:/SDK/Code/SDK_master_20220111/master/vatek_sdk_2/api/core/inc" -O0 -ffunction-sections -fdata-sections -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
bridge_api/src/avl68xx_dvbtx.o: D:/vatek/master/vatek_bridge_2/api/src/avl68xx_dvbtx.c bridge_api/src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F401xC -c -I../Core/Inc -I../USB_DEVICE/App -I../USB_DEVICE/Target -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CustomHID/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../../stm32/core/inc -I../../../api/inc -I"D:/SDK/Code/SDK_master_20220111/master/vatek_sdk_2/api/core/inc/core" -I"D:/SDK/Code/SDK_master_20220111/master/vatek_sdk_2/api/core/inc" -O0 -ffunction-sections -fdata-sections -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
bridge_api/src/avl68xx_internal.o: D:/vatek/master/vatek_bridge_2/api/src/avl68xx_internal.c bridge_api/src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F401xC -c -I../Core/Inc -I../USB_DEVICE/App -I../USB_DEVICE/Target -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CustomHID/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../../stm32/core/inc -I../../../api/inc -I"D:/SDK/Code/SDK_master_20220111/master/vatek_sdk_2/api/core/inc/core" -I"D:/SDK/Code/SDK_master_20220111/master/vatek_sdk_2/api/core/inc" -O0 -ffunction-sections -fdata-sections -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
bridge_api/src/avl68xx_isdbt.o: D:/vatek/master/vatek_bridge_2/api/src/avl68xx_isdbt.c bridge_api/src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F401xC -c -I../Core/Inc -I../USB_DEVICE/App -I../USB_DEVICE/Target -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CustomHID/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../../stm32/core/inc -I../../../api/inc -I"D:/SDK/Code/SDK_master_20220111/master/vatek_sdk_2/api/core/inc/core" -I"D:/SDK/Code/SDK_master_20220111/master/vatek_sdk_2/api/core/inc" -O0 -ffunction-sections -fdata-sections -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
bridge_api/src/avsource_adv718x.o: D:/vatek/master/vatek_bridge_2/api/src/avsource_adv718x.c bridge_api/src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F401xC -c -I../Core/Inc -I../USB_DEVICE/App -I../USB_DEVICE/Target -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CustomHID/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../../stm32/core/inc -I../../../api/inc -I"D:/SDK/Code/SDK_master_20220111/master/vatek_sdk_2/api/core/inc/core" -I"D:/SDK/Code/SDK_master_20220111/master/vatek_sdk_2/api/core/inc" -O0 -ffunction-sections -fdata-sections -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
bridge_api/src/avsource_ep9351.o: D:/vatek/master/vatek_bridge_2/api/src/avsource_ep9351.c bridge_api/src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F401xC -c -I../Core/Inc -I../USB_DEVICE/App -I../USB_DEVICE/Target -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CustomHID/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../../stm32/core/inc -I../../../api/inc -I"D:/SDK/Code/SDK_master_20220111/master/vatek_sdk_2/api/core/inc/core" -I"D:/SDK/Code/SDK_master_20220111/master/vatek_sdk_2/api/core/inc" -O0 -ffunction-sections -fdata-sections -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
bridge_api/src/avsource_ep9555e.o: D:/vatek/master/vatek_bridge_2/api/src/avsource_ep9555e.c bridge_api/src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F401xC -c -I../Core/Inc -I../USB_DEVICE/App -I../USB_DEVICE/Target -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CustomHID/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../../stm32/core/inc -I../../../api/inc -I"D:/SDK/Code/SDK_master_20220111/master/vatek_sdk_2/api/core/inc/core" -I"D:/SDK/Code/SDK_master_20220111/master/vatek_sdk_2/api/core/inc" -O0 -ffunction-sections -fdata-sections -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
bridge_api/src/avsource_sx1804a.o: D:/vatek/master/vatek_bridge_2/api/src/avsource_sx1804a.c bridge_api/src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F401xC -c -I../Core/Inc -I../USB_DEVICE/App -I../USB_DEVICE/Target -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CustomHID/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../../stm32/core/inc -I../../../api/inc -I"D:/SDK/Code/SDK_master_20220111/master/vatek_sdk_2/api/core/inc/core" -I"D:/SDK/Code/SDK_master_20220111/master/vatek_sdk_2/api/core/inc" -O0 -ffunction-sections -fdata-sections -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
bridge_api/src/bridge_internal.o: D:/vatek/master/vatek_bridge_2/api/src/bridge_internal.c bridge_api/src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F401xC -c -I../Core/Inc -I../USB_DEVICE/App -I../USB_DEVICE/Target -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CustomHID/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../../stm32/core/inc -I../../../api/inc -I"D:/SDK/Code/SDK_master_20220111/master/vatek_sdk_2/api/core/inc/core" -I"D:/SDK/Code/SDK_master_20220111/master/vatek_sdk_2/api/core/inc" -O0 -ffunction-sections -fdata-sections -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
bridge_api/src/cmd_bridge.o: D:/vatek/master/vatek_bridge_2/api/src/cmd_bridge.c bridge_api/src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F401xC -c -I../Core/Inc -I../USB_DEVICE/App -I../USB_DEVICE/Target -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CustomHID/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../../stm32/core/inc -I../../../api/inc -I"D:/SDK/Code/SDK_master_20220111/master/vatek_sdk_2/api/core/inc/core" -I"D:/SDK/Code/SDK_master_20220111/master/vatek_sdk_2/api/core/inc" -O0 -ffunction-sections -fdata-sections -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
bridge_api/src/cmd_mod.o: D:/vatek/master/vatek_bridge_2/api/src/cmd_mod.c bridge_api/src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F401xC -c -I../Core/Inc -I../USB_DEVICE/App -I../USB_DEVICE/Target -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CustomHID/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../../stm32/core/inc -I../../../api/inc -I"D:/SDK/Code/SDK_master_20220111/master/vatek_sdk_2/api/core/inc/core" -I"D:/SDK/Code/SDK_master_20220111/master/vatek_sdk_2/api/core/inc" -O0 -ffunction-sections -fdata-sections -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
bridge_api/src/demod_avl68xx.o: D:/vatek/master/vatek_bridge_2/api/src/demod_avl68xx.c bridge_api/src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F401xC -c -I../Core/Inc -I../USB_DEVICE/App -I../USB_DEVICE/Target -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CustomHID/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../../stm32/core/inc -I../../../api/inc -I"D:/SDK/Code/SDK_master_20220111/master/vatek_sdk_2/api/core/inc/core" -I"D:/SDK/Code/SDK_master_20220111/master/vatek_sdk_2/api/core/inc" -O0 -ffunction-sections -fdata-sections -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
bridge_api/src/ep9xxx_tool.o: D:/vatek/master/vatek_bridge_2/api/src/ep9xxx_tool.c bridge_api/src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F401xC -c -I../Core/Inc -I../USB_DEVICE/App -I../USB_DEVICE/Target -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CustomHID/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../../stm32/core/inc -I../../../api/inc -I"D:/SDK/Code/SDK_master_20220111/master/vatek_sdk_2/api/core/inc/core" -I"D:/SDK/Code/SDK_master_20220111/master/vatek_sdk_2/api/core/inc" -O0 -ffunction-sections -fdata-sections -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
bridge_api/src/hal_bridge_board.o: D:/vatek/master/vatek_bridge_2/api/src/hal_bridge_board.c bridge_api/src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F401xC -c -I../Core/Inc -I../USB_DEVICE/App -I../USB_DEVICE/Target -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CustomHID/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../../stm32/core/inc -I../../../api/inc -I"D:/SDK/Code/SDK_master_20220111/master/vatek_sdk_2/api/core/inc/core" -I"D:/SDK/Code/SDK_master_20220111/master/vatek_sdk_2/api/core/inc" -O0 -ffunction-sections -fdata-sections -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
bridge_api/src/hal_bridge_usb.o: D:/vatek/master/vatek_bridge_2/api/src/hal_bridge_usb.c bridge_api/src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F401xC -c -I../Core/Inc -I../USB_DEVICE/App -I../USB_DEVICE/Target -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CustomHID/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../../stm32/core/inc -I../../../api/inc -I"D:/SDK/Code/SDK_master_20220111/master/vatek_sdk_2/api/core/inc/core" -I"D:/SDK/Code/SDK_master_20220111/master/vatek_sdk_2/api/core/inc" -O0 -ffunction-sections -fdata-sections -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
bridge_api/src/hal_vatek_core.o: D:/vatek/master/vatek_bridge_2/api/src/hal_vatek_core.c bridge_api/src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F401xC -c -I../Core/Inc -I../USB_DEVICE/App -I../USB_DEVICE/Target -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CustomHID/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../../stm32/core/inc -I../../../api/inc -I"D:/SDK/Code/SDK_master_20220111/master/vatek_sdk_2/api/core/inc/core" -I"D:/SDK/Code/SDK_master_20220111/master/vatek_sdk_2/api/core/inc" -O0 -ffunction-sections -fdata-sections -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
bridge_api/src/hdmi_tool.o: D:/vatek/master/vatek_bridge_2/api/src/hdmi_tool.c bridge_api/src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F401xC -c -I../Core/Inc -I../USB_DEVICE/App -I../USB_DEVICE/Target -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CustomHID/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../../stm32/core/inc -I../../../api/inc -I"D:/SDK/Code/SDK_master_20220111/master/vatek_sdk_2/api/core/inc/core" -I"D:/SDK/Code/SDK_master_20220111/master/vatek_sdk_2/api/core/inc" -O0 -ffunction-sections -fdata-sections -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
bridge_api/src/vatek_bridge_2.o: D:/vatek/master/vatek_bridge_2/api/src/vatek_bridge_2.c bridge_api/src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F401xC -c -I../Core/Inc -I../USB_DEVICE/App -I../USB_DEVICE/Target -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CustomHID/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../../stm32/core/inc -I../../../api/inc -I"D:/SDK/Code/SDK_master_20220111/master/vatek_sdk_2/api/core/inc/core" -I"D:/SDK/Code/SDK_master_20220111/master/vatek_sdk_2/api/core/inc" -O0 -ffunction-sections -fdata-sections -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-bridge_api-2f-src

clean-bridge_api-2f-src:
	-$(RM) ./bridge_api/src/avl68xx_dvbc.d ./bridge_api/src/avl68xx_dvbc.o ./bridge_api/src/avl68xx_dvbc.su ./bridge_api/src/avl68xx_dvbsx.d ./bridge_api/src/avl68xx_dvbsx.o ./bridge_api/src/avl68xx_dvbsx.su ./bridge_api/src/avl68xx_dvbtx.d ./bridge_api/src/avl68xx_dvbtx.o ./bridge_api/src/avl68xx_dvbtx.su ./bridge_api/src/avl68xx_internal.d ./bridge_api/src/avl68xx_internal.o ./bridge_api/src/avl68xx_internal.su ./bridge_api/src/avl68xx_isdbt.d ./bridge_api/src/avl68xx_isdbt.o ./bridge_api/src/avl68xx_isdbt.su ./bridge_api/src/avsource_adv718x.d ./bridge_api/src/avsource_adv718x.o ./bridge_api/src/avsource_adv718x.su ./bridge_api/src/avsource_ep9351.d ./bridge_api/src/avsource_ep9351.o ./bridge_api/src/avsource_ep9351.su ./bridge_api/src/avsource_ep9555e.d ./bridge_api/src/avsource_ep9555e.o ./bridge_api/src/avsource_ep9555e.su ./bridge_api/src/avsource_sx1804a.d ./bridge_api/src/avsource_sx1804a.o ./bridge_api/src/avsource_sx1804a.su ./bridge_api/src/bridge_internal.d ./bridge_api/src/bridge_internal.o ./bridge_api/src/bridge_internal.su ./bridge_api/src/cmd_bridge.d ./bridge_api/src/cmd_bridge.o ./bridge_api/src/cmd_bridge.su ./bridge_api/src/cmd_mod.d ./bridge_api/src/cmd_mod.o ./bridge_api/src/cmd_mod.su ./bridge_api/src/demod_avl68xx.d ./bridge_api/src/demod_avl68xx.o ./bridge_api/src/demod_avl68xx.su ./bridge_api/src/ep9xxx_tool.d ./bridge_api/src/ep9xxx_tool.o ./bridge_api/src/ep9xxx_tool.su ./bridge_api/src/hal_bridge_board.d ./bridge_api/src/hal_bridge_board.o ./bridge_api/src/hal_bridge_board.su ./bridge_api/src/hal_bridge_usb.d ./bridge_api/src/hal_bridge_usb.o ./bridge_api/src/hal_bridge_usb.su ./bridge_api/src/hal_vatek_core.d ./bridge_api/src/hal_vatek_core.o ./bridge_api/src/hal_vatek_core.su ./bridge_api/src/hdmi_tool.d ./bridge_api/src/hdmi_tool.o ./bridge_api/src/hdmi_tool.su ./bridge_api/src/vatek_bridge_2.d ./bridge_api/src/vatek_bridge_2.o ./bridge_api/src/vatek_bridge_2.su

.PHONY: clean-bridge_api-2f-src

