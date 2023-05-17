################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/LCD/ST7789/Display.c \
../Drivers/LCD/ST7789/Draw.c \
../Drivers/LCD/ST7789/FontsImage.c \
../Drivers/LCD/ST7789/Icons.c \
../Drivers/LCD/ST7789/ST7789.c \
../Drivers/LCD/ST7789/fonts.c 

C_DEPS += \
./Drivers/LCD/ST7789/Display.d \
./Drivers/LCD/ST7789/Draw.d \
./Drivers/LCD/ST7789/FontsImage.d \
./Drivers/LCD/ST7789/Icons.d \
./Drivers/LCD/ST7789/ST7789.d \
./Drivers/LCD/ST7789/fonts.d 

OBJS += \
./Drivers/LCD/ST7789/Display.o \
./Drivers/LCD/ST7789/Draw.o \
./Drivers/LCD/ST7789/FontsImage.o \
./Drivers/LCD/ST7789/Icons.o \
./Drivers/LCD/ST7789/ST7789.o \
./Drivers/LCD/ST7789/fonts.o 


# Each subdirectory must supply rules for building sources it contributes
Drivers/LCD/ST7789/%.o Drivers/LCD/ST7789/%.su: ../Drivers/LCD/ST7789/%.c Drivers/LCD/ST7789/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F411xE -c -I../Drivers/Audio/WM8960 -I../Drivers/Utilities -I../Drivers/Flash -I../Drivers/Setting -I../Drivers/BatteryMonitoring -I../Drivers/SDCard -I../Drivers/IO_Expand -I../Drivers/Audio/Volume -I../Drivers/LCD/ST7789 -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../FATFS/Target -I../FATFS/App -I../Middlewares/Third_Party/FatFs/src -I../Middlewares/ST/touchgfx/framework/include -I../TouchGFX/generated/fonts/include -I../TouchGFX/generated/gui_generated/include -I../TouchGFX/generated/images/include -I../TouchGFX/generated/texts/include -I../TouchGFX/generated/videos/include -I../TouchGFX/gui/include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Drivers-2f-LCD-2f-ST7789

clean-Drivers-2f-LCD-2f-ST7789:
	-$(RM) ./Drivers/LCD/ST7789/Display.d ./Drivers/LCD/ST7789/Display.o ./Drivers/LCD/ST7789/Display.su ./Drivers/LCD/ST7789/Draw.d ./Drivers/LCD/ST7789/Draw.o ./Drivers/LCD/ST7789/Draw.su ./Drivers/LCD/ST7789/FontsImage.d ./Drivers/LCD/ST7789/FontsImage.o ./Drivers/LCD/ST7789/FontsImage.su ./Drivers/LCD/ST7789/Icons.d ./Drivers/LCD/ST7789/Icons.o ./Drivers/LCD/ST7789/Icons.su ./Drivers/LCD/ST7789/ST7789.d ./Drivers/LCD/ST7789/ST7789.o ./Drivers/LCD/ST7789/ST7789.su ./Drivers/LCD/ST7789/fonts.d ./Drivers/LCD/ST7789/fonts.o ./Drivers/LCD/ST7789/fonts.su

.PHONY: clean-Drivers-2f-LCD-2f-ST7789

