################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../adc.c \
../delay.c \
../fonts.c \
../main.c \
../sw_uart.c \
../twi.c \
../uart.c 

OBJS += \
./adc.o \
./delay.o \
./fonts.o \
./main.o \
./sw_uart.o \
./twi.o \
./uart.o 

C_DEPS += \
./adc.d \
./delay.d \
./fonts.d \
./main.d \
./sw_uart.d \
./twi.d \
./uart.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: AVR Compiler'
	avr-gcc -Wall -g2 -gstabs -Os -fpack-struct -fshort-enums -std=gnu99 -funsigned-char -funsigned-bitfields -mmcu=atmega8 -DF_CPU=8000000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


