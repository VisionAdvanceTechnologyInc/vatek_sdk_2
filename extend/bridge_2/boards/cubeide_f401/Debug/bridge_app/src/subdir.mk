################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
D:/vatek/master/vatek_bridge_2/app/bridge_2/src/bridge_app.c 

OBJS += \
./bridge_app/src/bridge_app.o 

C_DEPS += \
./bridge_app/src/bridge_app.d 


# Each subdirectory must supply rules for building sources it contributes
bridge_app/src/bridge_app.o: D:/vatek/master/vatek_bridge_2/app/bridge_2/src/bridge_app.c bridge_app/src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F401xC -c -I../Core/Inc -I../USB_DEVICE/App -I../USB_DEVICE/Target -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CustomHID/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../../stm32/core/inc -I../../../api/inc -I"D:/SDK/Code/SDK_master_20220111/master/vatek_sdk_2/api/core/inc/core" -I"D:/SDK/Code/SDK_master_20220111/master/vatek_sdk_2/api/core/inc" -O0 -ffunction-sections -fdata-sections -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-bridge_app-2f-src

clean-bridge_app-2f-src:
	-$(RM) ./bridge_app/src/bridge_app.d ./bridge_app/src/bridge_app.o ./bridge_app/src/bridge_app.su

.PHONY: clean-bridge_app-2f-src

