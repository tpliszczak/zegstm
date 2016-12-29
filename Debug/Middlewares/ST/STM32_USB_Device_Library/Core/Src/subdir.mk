################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_core.c \
../Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_ctlreq.c \
../Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_ioreq.c 

OBJS += \
./Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_core.o \
./Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_ctlreq.o \
./Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_ioreq.o 

C_DEPS += \
./Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_core.d \
./Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_ctlreq.d \
./Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_ioreq.d 


# Each subdirectory must supply rules for building sources it contributes
Middlewares/ST/STM32_USB_Device_Library/Core/Src/%.o: ../Middlewares/ST/STM32_USB_Device_Library/Core/Src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo %cd%
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -mfloat-abi=soft -D__weak="__attribute__((weak))" -D__packed="__attribute__((__packed__))" -DUSE_HAL_DRIVER -DSTM32F103xB -I"F:/TP-moje dokumenty/Programowanie/ST/sw_workspace/zegarStm/Inc" -I"F:/TP-moje dokumenty/Programowanie/ST/sw_workspace/zegarStm/Drivers/STM32F1xx_HAL_Driver/Inc" -I"F:/TP-moje dokumenty/Programowanie/ST/sw_workspace/zegarStm/Drivers/STM32F1xx_HAL_Driver/Inc/Legacy" -I"F:/TP-moje dokumenty/Programowanie/ST/sw_workspace/zegarStm/Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM3" -I"F:/TP-moje dokumenty/Programowanie/ST/sw_workspace/zegarStm/Middlewares/ST/STM32_USB_Device_Library/Core/Inc" -I"F:/TP-moje dokumenty/Programowanie/ST/sw_workspace/zegarStm/Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc" -I"F:/TP-moje dokumenty/Programowanie/ST/sw_workspace/zegarStm/Middlewares/Third_Party/FreeRTOS/Source/include" -I"F:/TP-moje dokumenty/Programowanie/ST/sw_workspace/zegarStm/Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS" -I"F:/TP-moje dokumenty/Programowanie/ST/sw_workspace/zegarStm/Drivers/CMSIS/Include" -I"F:/TP-moje dokumenty/Programowanie/ST/sw_workspace/zegarStm/Drivers/CMSIS/Device/ST/STM32F1xx/Include"  -Os -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


