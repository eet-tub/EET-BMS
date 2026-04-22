################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (14.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
C:/Users/pavel/Nextcloud/Arbeit/Projekte/STM32_Projekte/Open_BMS/Open_BMS/Firmware/EET_BMS/Common/Inter_Process_Communication/Src/IPPC_Functions.c 

OBJS += \
./Common/Inter_Process_Communication/Src/IPPC_Functions.o 

C_DEPS += \
./Common/Inter_Process_Communication/Src/IPPC_Functions.d 


# Each subdirectory must supply rules for building sources it contributes
Common/Inter_Process_Communication/Src/IPPC_Functions.o: C:/Users/pavel/Nextcloud/Arbeit/Projekte/STM32_Projekte/Open_BMS/Open_BMS/Firmware/EET_BMS/Common/Inter_Process_Communication/Src/IPPC_Functions.c Common/Inter_Process_Communication/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DDEBUG -DCORE_CM7 -DUSE_HAL_DRIVER -DSTM32H755xx -DUSE_PWR_LDO_SUPPLY -c -I../Core/Inc -I../../Drivers/STM32H7xx_HAL_Driver/Inc -I../../Drivers/STM32H7xx_HAL_Driver/Inc/Legacy -I../../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../../Drivers/CMSIS/Include -I"C:/Users/pavel/Nextcloud/Arbeit/Projekte/STM32_Projekte/Open_BMS/Open_BMS/Firmware/EET_BMS/CM7/OLED_Lib/ssd1306" -I"C:/Users/pavel/Nextcloud/Arbeit/Projekte/STM32_Projekte/Open_BMS/Open_BMS/Firmware/EET_BMS/Common/Inter_Process_Communication/Inc" -I"C:/Users/pavel/Nextcloud/Arbeit/Projekte/STM32_Projekte/Open_BMS/Open_BMS/Firmware/EET_BMS/CM7/Display_Functions/Inc" -I"C:/Users/pavel/Nextcloud/Arbeit/Projekte/STM32_Projekte/Open_BMS/Open_BMS/Firmware/EET_BMS/CM7/BMS_State_Estimation/Inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Common-2f-Inter_Process_Communication-2f-Src

clean-Common-2f-Inter_Process_Communication-2f-Src:
	-$(RM) ./Common/Inter_Process_Communication/Src/IPPC_Functions.cyclo ./Common/Inter_Process_Communication/Src/IPPC_Functions.d ./Common/Inter_Process_Communication/Src/IPPC_Functions.o ./Common/Inter_Process_Communication/Src/IPPC_Functions.su

.PHONY: clean-Common-2f-Inter_Process_Communication-2f-Src

