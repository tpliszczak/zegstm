################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Src/adc.c \
../Src/crc.c \
../Src/dma.c \
../Src/eeprom.c \
../Src/freertos.c \
../Src/gpio.c \
../Src/main.c \
../Src/oneWire.c \
../Src/rtc.c \
../Src/stm32f1xx_hal_msp.c \
../Src/stm32f1xx_hal_timebase_TIM.c \
../Src/stm32f1xx_it.c \
../Src/system_stm32f1xx.c \
../Src/tim.c \
../Src/usart.c \
../Src/usb_device.c \
../Src/usbd_cdc_if.c \
../Src/usbd_conf.c \
../Src/usbd_desc.c \
../Src/wyswietlacz7Seg.c 

OBJS += \
./Src/adc.o \
./Src/crc.o \
./Src/dma.o \
./Src/eeprom.o \
./Src/freertos.o \
./Src/gpio.o \
./Src/main.o \
./Src/oneWire.o \
./Src/rtc.o \
./Src/stm32f1xx_hal_msp.o \
./Src/stm32f1xx_hal_timebase_TIM.o \
./Src/stm32f1xx_it.o \
./Src/system_stm32f1xx.o \
./Src/tim.o \
./Src/usart.o \
./Src/usb_device.o \
./Src/usbd_cdc_if.o \
./Src/usbd_conf.o \
./Src/usbd_desc.o \
./Src/wyswietlacz7Seg.o 

C_DEPS += \
./Src/adc.d \
./Src/crc.d \
./Src/dma.d \
./Src/eeprom.d \
./Src/freertos.d \
./Src/gpio.d \
./Src/main.d \
./Src/oneWire.d \
./Src/rtc.d \
./Src/stm32f1xx_hal_msp.d \
./Src/stm32f1xx_hal_timebase_TIM.d \
./Src/stm32f1xx_it.d \
./Src/system_stm32f1xx.d \
./Src/tim.d \
./Src/usart.d \
./Src/usb_device.d \
./Src/usbd_cdc_if.d \
./Src/usbd_conf.d \
./Src/usbd_desc.d \
./Src/wyswietlacz7Seg.d 


# Each subdirectory must supply rules for building sources it contributes
Src/%.o: ../Src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo %cd%
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -mfloat-abi=soft -D__weak="__attribute__((weak))" -D__packed="__attribute__((__packed__))" -DUSE_HAL_DRIVER -DSTM32F103xB -I"F:/TP-moje dokumenty/Programowanie/ST/sw_workspace/zegarStm/Inc" -I"F:/TP-moje dokumenty/Programowanie/ST/sw_workspace/zegarStm/Drivers/STM32F1xx_HAL_Driver/Inc" -I"F:/TP-moje dokumenty/Programowanie/ST/sw_workspace/zegarStm/Drivers/STM32F1xx_HAL_Driver/Inc/Legacy" -I"F:/TP-moje dokumenty/Programowanie/ST/sw_workspace/zegarStm/Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM3" -I"F:/TP-moje dokumenty/Programowanie/ST/sw_workspace/zegarStm/Middlewares/ST/STM32_USB_Device_Library/Core/Inc" -I"F:/TP-moje dokumenty/Programowanie/ST/sw_workspace/zegarStm/Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc" -I"F:/TP-moje dokumenty/Programowanie/ST/sw_workspace/zegarStm/Middlewares/Third_Party/FreeRTOS/Source/include" -I"F:/TP-moje dokumenty/Programowanie/ST/sw_workspace/zegarStm/Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS" -I"F:/TP-moje dokumenty/Programowanie/ST/sw_workspace/zegarStm/Drivers/CMSIS/Include" -I"F:/TP-moje dokumenty/Programowanie/ST/sw_workspace/zegarStm/Drivers/CMSIS/Device/ST/STM32F1xx/Include"  -Os -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


