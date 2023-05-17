################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/Audio/WM8960/Play_wav.c \
../Drivers/Audio/WM8960/WM8960.c 

C_DEPS += \
./Drivers/Audio/WM8960/Play_wav.d \
./Drivers/Audio/WM8960/WM8960.d 

OBJS += \
./Drivers/Audio/WM8960/Play_wav.o \
./Drivers/Audio/WM8960/WM8960.o 


# Each subdirectory must supply rules for building sources it contributes
Drivers/Audio/WM8960/%.o Drivers/Audio/WM8960/%.su: ../Drivers/Audio/WM8960/%.c Drivers/Audio/WM8960/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -DUSE_HAL_DRIVER -DSTM32F411xE -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../FATFS/Target -I../FATFS/App -I../Middlewares/Third_Party/FatFs/src -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Drivers-2f-Audio-2f-WM8960

clean-Drivers-2f-Audio-2f-WM8960:
	-$(RM) ./Drivers/Audio/WM8960/Play_wav.d ./Drivers/Audio/WM8960/Play_wav.o ./Drivers/Audio/WM8960/Play_wav.su ./Drivers/Audio/WM8960/WM8960.d ./Drivers/Audio/WM8960/WM8960.o ./Drivers/Audio/WM8960/WM8960.su

.PHONY: clean-Drivers-2f-Audio-2f-WM8960

