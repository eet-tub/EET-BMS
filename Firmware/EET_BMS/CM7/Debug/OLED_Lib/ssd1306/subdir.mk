################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (14.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../OLED_Lib/ssd1306/ssd1306.c \
../OLED_Lib/ssd1306/ssd1306_fonts.c \
../OLED_Lib/ssd1306/ssd1306_tests.c 

OBJS += \
./OLED_Lib/ssd1306/ssd1306.o \
./OLED_Lib/ssd1306/ssd1306_fonts.o \
./OLED_Lib/ssd1306/ssd1306_tests.o 

C_DEPS += \
./OLED_Lib/ssd1306/ssd1306.d \
./OLED_Lib/ssd1306/ssd1306_fonts.d \
./OLED_Lib/ssd1306/ssd1306_tests.d 


# Each subdirectory must supply rules for building sources it contributes
OLED_Lib/ssd1306/%.o OLED_Lib/ssd1306/%.su OLED_Lib/ssd1306/%.cyclo: ../OLED_Lib/ssd1306/%.c OLED_Lib/ssd1306/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DDEBUG -DCORE_CM7 -DUSE_HAL_DRIVER -DSTM32H755xx -DUSE_PWR_LDO_SUPPLY -c -I../Core/Inc -I../../Drivers/STM32H7xx_HAL_Driver/Inc -I../../Drivers/STM32H7xx_HAL_Driver/Inc/Legacy -I../../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../../Drivers/CMSIS/Include -I"C:/Users/pavel/Nextcloud/Arbeit/Projekte/STM32_Projekte/Open_BMS/Open_BMS/Firmware/EET_BMS/CM7/OLED_Lib/ssd1306" -I"C:/Users/pavel/Nextcloud/Arbeit/Projekte/STM32_Projekte/Open_BMS/Open_BMS/Firmware/EET_BMS/Common/Inter_Process_Communication/Inc" -I"C:/Users/pavel/Nextcloud/Arbeit/Projekte/STM32_Projekte/Open_BMS/Open_BMS/Firmware/EET_BMS/CM7/Display_Functions/Inc" -I"C:/Users/pavel/Nextcloud/Arbeit/Projekte/STM32_Projekte/Open_BMS/Open_BMS/Firmware/EET_BMS/CM7/BMS_State_Estimation/Inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-OLED_Lib-2f-ssd1306

clean-OLED_Lib-2f-ssd1306:
	-$(RM) ./OLED_Lib/ssd1306/ssd1306.cyclo ./OLED_Lib/ssd1306/ssd1306.d ./OLED_Lib/ssd1306/ssd1306.o ./OLED_Lib/ssd1306/ssd1306.su ./OLED_Lib/ssd1306/ssd1306_fonts.cyclo ./OLED_Lib/ssd1306/ssd1306_fonts.d ./OLED_Lib/ssd1306/ssd1306_fonts.o ./OLED_Lib/ssd1306/ssd1306_fonts.su ./OLED_Lib/ssd1306/ssd1306_tests.cyclo ./OLED_Lib/ssd1306/ssd1306_tests.d ./OLED_Lib/ssd1306/ssd1306_tests.o ./OLED_Lib/ssd1306/ssd1306_tests.su

.PHONY: clean-OLED_Lib-2f-ssd1306

