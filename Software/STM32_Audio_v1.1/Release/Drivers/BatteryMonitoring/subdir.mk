################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/BatteryMonitoring/STC3100.c 

C_DEPS += \
./Drivers/BatteryMonitoring/STC3100.d 

OBJS += \
./Drivers/BatteryMonitoring/STC3100.o 


# Each subdirectory must supply rules for building sources it contributes
Drivers/BatteryMonitoring/%.o Drivers/BatteryMonitoring/%.su: ../Drivers/BatteryMonitoring/%.c Drivers/BatteryMonitoring/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -DUSE_HAL_DRIVER -DSTM32F411xE -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../FATFS/Target -I../FATFS/App -I../Middlewares/Third_Party/FatFs/src -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Drivers-2f-BatteryMonitoring

clean-Drivers-2f-BatteryMonitoring:
	-$(RM) ./Drivers/BatteryMonitoring/STC3100.d ./Drivers/BatteryMonitoring/STC3100.o ./Drivers/BatteryMonitoring/STC3100.su

.PHONY: clean-Drivers-2f-BatteryMonitoring

