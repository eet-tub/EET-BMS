################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (14.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../BMS_State_Estimation/Src/BMS_State_Estimation.c 

OBJS += \
./BMS_State_Estimation/Src/BMS_State_Estimation.o 

C_DEPS += \
./BMS_State_Estimation/Src/BMS_State_Estimation.d 


# Each subdirectory must supply rules for building sources it contributes
BMS_State_Estimation/Src/%.o BMS_State_Estimation/Src/%.su BMS_State_Estimation/Src/%.cyclo: ../BMS_State_Estimation/Src/%.c BMS_State_Estimation/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DDEBUG -DCORE_CM7 -DUSE_HAL_DRIVER -DSTM32H755xx -DUSE_PWR_LDO_SUPPLY -c -I../Core/Inc -I../../Drivers/STM32H7xx_HAL_Driver/Inc -I../../Drivers/STM32H7xx_HAL_Driver/Inc/Legacy -I../../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../../Drivers/CMSIS/Include -I"C:/Users/pavel/Nextcloud/Arbeit/Projekte/STM32_Projekte/EET_BMS/EET_BMS_4s/Firmware/EET_BMS/CM7/OLED_Lib/ssd1306" -I"C:/Users/pavel/Nextcloud/Arbeit/Projekte/STM32_Projekte/EET_BMS/EET_BMS_4s/Firmware/EET_BMS/Common/Inter_Process_Communication/Inc" -I"C:/Users/pavel/Nextcloud/Arbeit/Projekte/STM32_Projekte/EET_BMS/EET_BMS_4s/Firmware/EET_BMS/CM7/Display_Functions/Inc" -I"C:/Users/pavel/Nextcloud/Arbeit/Projekte/STM32_Projekte/EET_BMS/EET_BMS_4s/Firmware/EET_BMS/CM7/BMS_State_Estimation/Inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-BMS_State_Estimation-2f-Src

clean-BMS_State_Estimation-2f-Src:
	-$(RM) ./BMS_State_Estimation/Src/BMS_State_Estimation.cyclo ./BMS_State_Estimation/Src/BMS_State_Estimation.d ./BMS_State_Estimation/Src/BMS_State_Estimation.o ./BMS_State_Estimation/Src/BMS_State_Estimation.su

.PHONY: clean-BMS_State_Estimation-2f-Src

