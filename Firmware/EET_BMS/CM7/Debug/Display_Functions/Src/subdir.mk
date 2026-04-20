################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (14.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Display_Functions/Src/Display_Functions.c 

OBJS += \
./Display_Functions/Src/Display_Functions.o 

C_DEPS += \
./Display_Functions/Src/Display_Functions.d 


# Each subdirectory must supply rules for building sources it contributes
Display_Functions/Src/%.o Display_Functions/Src/%.su Display_Functions/Src/%.cyclo: ../Display_Functions/Src/%.c Display_Functions/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DDEBUG -DCORE_CM7 -DUSE_HAL_DRIVER -DSTM32H755xx -DUSE_PWR_LDO_SUPPLY -c -I../Core/Inc -I../../Drivers/STM32H7xx_HAL_Driver/Inc -I../../Drivers/STM32H7xx_HAL_Driver/Inc/Legacy -I../../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../../Drivers/CMSIS/Include -I"C:/Users/pavel/Nextcloud/Arbeit/Projekte/STM32_Projekte/EET_BMS/EET_BMS_4s/Firmware/EET_BMS/CM7/OLED_Lib/ssd1306" -I"C:/Users/pavel/Nextcloud/Arbeit/Projekte/STM32_Projekte/EET_BMS/EET_BMS_4s/Firmware/EET_BMS/Common/Inter_Process_Communication/Inc" -I"C:/Users/pavel/Nextcloud/Arbeit/Projekte/STM32_Projekte/EET_BMS/EET_BMS_4s/Firmware/EET_BMS/CM7/Display_Functions/Inc" -I"C:/Users/pavel/Nextcloud/Arbeit/Projekte/STM32_Projekte/EET_BMS/EET_BMS_4s/Firmware/EET_BMS/CM7/BMS_State_Estimation/Inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Display_Functions-2f-Src

clean-Display_Functions-2f-Src:
	-$(RM) ./Display_Functions/Src/Display_Functions.cyclo ./Display_Functions/Src/Display_Functions.d ./Display_Functions/Src/Display_Functions.o ./Display_Functions/Src/Display_Functions.su

.PHONY: clean-Display_Functions-2f-Src

