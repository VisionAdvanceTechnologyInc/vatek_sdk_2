################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
D:/vatek/master/vatek_bridge_2/boards/stm32/core/src/stm32_core.c \
D:/vatek/master/vatek_bridge_2/boards/stm32/core/src/stm32_core_gpio.c \
D:/vatek/master/vatek_bridge_2/boards/stm32/core/src/stm32_core_i2c.c \
D:/vatek/master/vatek_bridge_2/boards/stm32/core/src/stm32_core_spi.c \
D:/vatek/master/vatek_bridge_2/boards/stm32/core/src/stm32_core_uart.c \
D:/vatek/master/vatek_bridge_2/boards/stm32/core/src/stm32_hal_board.c \
D:/vatek/master/vatek_bridge_2/boards/stm32/core/src/stm32_hal_bridge.c \
D:/vatek/master/vatek_bridge_2/boards/stm32/core/src/stm32_usb_bridge.c 

OBJS += \
./stm32_core/src/stm32_core.o \
./stm32_core/src/stm32_core_gpio.o \
./stm32_core/src/stm32_core_i2c.o \
./stm32_core/src/stm32_core_spi.o \
./stm32_core/src/stm32_core_uart.o \
./stm32_core/src/stm32_hal_board.o \
./stm32_core/src/stm32_hal_bridge.o \
./stm32_core/src/stm32_usb_bridge.o 

C_DEPS += \
./stm32_core/src/stm32_core.d \
./stm32_core/src/stm32_core_gpio.d \
./stm32_core/src/stm32_core_i2c.d \
./stm32_core/src/stm32_core_spi.d \
./stm32_core/src/stm32_core_uart.d \
./stm32_core/src/stm32_hal_board.d \
./stm32_core/src/stm32_hal_bridge.d \
./stm32_core/src/stm32_usb_bridge.d 


# Each subdirectory must supply rules for building sources it contributes
stm32_core/src/stm32_core.o: D:/vatek/master/vatek_bridge_2/boards/stm32/core/src/stm32_core.c stm32_core/src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F401xC -c -I../Core/Inc -I../USB_DEVICE/App -I../USB_DEVICE/Target -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CustomHID/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../../stm32/core/inc -I../../../api/inc -I"D:/SDK/Code/SDK_master_20220111/master/vatek_sdk_2/api/core/inc/core" -I"D:/SDK/Code/SDK_master_20220111/master/vatek_sdk_2/api/core/inc" -O0 -ffunction-sections -fdata-sections -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
stm32_core/src/stm32_core_gpio.o: D:/vatek/master/vatek_bridge_2/boards/stm32/core/src/stm32_core_gpio.c stm32_core/src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F401xC -c -I../Core/Inc -I../USB_DEVICE/App -I../USB_DEVICE/Target -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CustomHID/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../../stm32/core/inc -I../../../api/inc -I"D:/SDK/Code/SDK_master_20220111/master/vatek_sdk_2/api/core/inc/core" -I"D:/SDK/Code/SDK_master_20220111/master/vatek_sdk_2/api/core/inc" -O0 -ffunction-sections -fdata-sections -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
stm32_core/src/stm32_core_i2c.o: D:/vatek/master/vatek_bridge_2/boards/stm32/core/src/stm32_core_i2c.c stm32_core/src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F401xC -c -I../Core/Inc -I../USB_DEVICE/App -I../USB_DEVICE/Target -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CustomHID/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../../stm32/core/inc -I../../../api/inc -I"D:/SDK/Code/SDK_master_20220111/master/vatek_sdk_2/api/core/inc/core" -I"D:/SDK/Code/SDK_master_20220111/master/vatek_sdk_2/api/core/inc" -O0 -ffunction-sections -fdata-sections -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
stm32_core/src/stm32_core_spi.o: D:/vatek/master/vatek_bridge_2/boards/stm32/core/src/stm32_core_spi.c stm32_core/src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F401xC -c -I../Core/Inc -I../USB_DEVICE/App -I../USB_DEVICE/Target -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CustomHID/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../../stm32/core/inc -I../../../api/inc -I"D:/SDK/Code/SDK_master_20220111/master/vatek_sdk_2/api/core/inc/core" -I"D:/SDK/Code/SDK_master_20220111/master/vatek_sdk_2/api/core/inc" -O0 -ffunction-sections -fdata-sections -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
stm32_core/src/stm32_core_uart.o: D:/vatek/master/vatek_bridge_2/boards/stm32/core/src/stm32_core_uart.c stm32_core/src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F401xC -c -I../Core/Inc -I../USB_DEVICE/App -I../USB_DEVICE/Target -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CustomHID/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../../stm32/core/inc -I../../../api/inc -I"D:/SDK/Code/SDK_master_20220111/master/vatek_sdk_2/api/core/inc/core" -I"D:/SDK/Code/SDK_master_20220111/master/vatek_sdk_2/api/core/inc" -O0 -ffunction-sections -fdata-sections -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
stm32_core/src/stm32_hal_board.o: D:/vatek/master/vatek_bridge_2/boards/stm32/core/src/stm32_hal_board.c stm32_core/src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F401xC -c -I../Core/Inc -I../USB_DEVICE/App -I../USB_DEVICE/Target -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CustomHID/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../../stm32/core/inc -I../../../api/inc -I"D:/SDK/Code/SDK_master_20220111/master/vatek_sdk_2/api/core/inc/core" -I"D:/SDK/Code/SDK_master_20220111/master/vatek_sdk_2/api/core/inc" -O0 -ffunction-sections -fdata-sections -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
stm32_core/src/stm32_hal_bridge.o: D:/vatek/master/vatek_bridge_2/boards/stm32/core/src/stm32_hal_bridge.c stm32_core/src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F401xC -c -I../Core/Inc -I../USB_DEVICE/App -I../USB_DEVICE/Target -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CustomHID/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../../stm32/core/inc -I../../../api/inc -I"D:/SDK/Code/SDK_master_20220111/master/vatek_sdk_2/api/core/inc/core" -I"D:/SDK/Code/SDK_master_20220111/master/vatek_sdk_2/api/core/inc" -O0 -ffunction-sections -fdata-sections -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
stm32_core/src/stm32_usb_bridge.o: D:/vatek/master/vatek_bridge_2/boards/stm32/core/src/stm32_usb_bridge.c stm32_core/src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F401xC -c -I../Core/Inc -I../USB_DEVICE/App -I../USB_DEVICE/Target -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CustomHID/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../../stm32/core/inc -I../../../api/inc -I"D:/SDK/Code/SDK_master_20220111/master/vatek_sdk_2/api/core/inc/core" -I"D:/SDK/Code/SDK_master_20220111/master/vatek_sdk_2/api/core/inc" -O0 -ffunction-sections -fdata-sections -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-stm32_core-2f-src

clean-stm32_core-2f-src:
	-$(RM) ./stm32_core/src/stm32_core.d ./stm32_core/src/stm32_core.o ./stm32_core/src/stm32_core.su ./stm32_core/src/stm32_core_gpio.d ./stm32_core/src/stm32_core_gpio.o ./stm32_core/src/stm32_core_gpio.su ./stm32_core/src/stm32_core_i2c.d ./stm32_core/src/stm32_core_i2c.o ./stm32_core/src/stm32_core_i2c.su ./stm32_core/src/stm32_core_spi.d ./stm32_core/src/stm32_core_spi.o ./stm32_core/src/stm32_core_spi.su ./stm32_core/src/stm32_core_uart.d ./stm32_core/src/stm32_core_uart.o ./stm32_core/src/stm32_core_uart.su ./stm32_core/src/stm32_hal_board.d ./stm32_core/src/stm32_hal_board.o ./stm32_core/src/stm32_hal_board.su ./stm32_core/src/stm32_hal_bridge.d ./stm32_core/src/stm32_hal_bridge.o ./stm32_core/src/stm32_hal_bridge.su ./stm32_core/src/stm32_usb_bridge.d ./stm32_core/src/stm32_usb_bridge.o ./stm32_core/src/stm32_usb_bridge.su

.PHONY: clean-stm32_core-2f-src

