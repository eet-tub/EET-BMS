################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (14.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/TLE9012_Lib/Src/TLE_Abstraction.c \
../Core/TLE9012_Lib/Src/TLE_LL_Driver.c 

OBJS += \
./Core/TLE9012_Lib/Src/TLE_Abstraction.o \
./Core/TLE9012_Lib/Src/TLE_LL_Driver.o 

C_DEPS += \
./Core/TLE9012_Lib/Src/TLE_Abstraction.d \
./Core/TLE9012_Lib/Src/TLE_LL_Driver.d 


# Each subdirectory must supply rules for building sources it contributes
Core/TLE9012_Lib/Src/%.o Core/TLE9012_Lib/Src/%.su Core/TLE9012_Lib/Src/%.cyclo: ../Core/TLE9012_Lib/Src/%.c Core/TLE9012_Lib/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DCORE_CM4 -DUSE_HAL_DRIVER -DSTM32H755xx -DUSE_PWR_LDO_SUPPLY -c -I../Core/Inc -I../../Drivers/STM32H7xx_HAL_Driver/Inc -I../../Drivers/STM32H7xx_HAL_Driver/Inc/Legacy -I../../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../../Drivers/CMSIS/Include -I"C:/Users/pavel/Nextcloud/Arbeit/Projekte/STM32_Projekte/EET_BMS/EET_BMS_4s/Firmware/EET_BMS/CM4/Core/TLE9012_Lib/Inc" -I"C:/Users/pavel/Nextcloud/Arbeit/Projekte/STM32_Projekte/EET_BMS/EET_BMS_4s/Firmware/EET_BMS/Common/Inter_Process_Communication/Inc" -I"C:/Users/pavel/Nextcloud/Arbeit/Projekte/STM32_Projekte/EET_BMS/EET_BMS_4s/Firmware/EET_BMS/Common/Inter_Process_Communication" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-TLE9012_Lib-2f-Src

clean-Core-2f-TLE9012_Lib-2f-Src:
	-$(RM) ./Core/TLE9012_Lib/Src/TLE_Abstraction.cyclo ./Core/TLE9012_Lib/Src/TLE_Abstraction.d ./Core/TLE9012_Lib/Src/TLE_Abstraction.o ./Core/TLE9012_Lib/Src/TLE_Abstraction.su ./Core/TLE9012_Lib/Src/TLE_LL_Driver.cyclo ./Core/TLE9012_Lib/Src/TLE_LL_Driver.d ./Core/TLE9012_Lib/Src/TLE_LL_Driver.o ./Core/TLE9012_Lib/Src/TLE_LL_Driver.su

.PHONY: clean-Core-2f-TLE9012_Lib-2f-Src

