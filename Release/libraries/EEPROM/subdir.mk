################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../libraries/EEPROM/EEPROM.cpp 

OBJS += \
./libraries/EEPROM/EEPROM.o 

CPP_DEPS += \
./libraries/EEPROM/EEPROM.d 


# Each subdirectory must supply rules for building sources it contributes
libraries/EEPROM/%.o: ../libraries/EEPROM/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: AVR C++ Compiler'
	avr-g++ -I"/home/robert/devSpace/arduinoWorkspace/Arduino_Mega_ATmega1280/arduino" -I"/home/robert/devSpace/arduinoWorkspace/FreeBoardPLC/libraries" -D__IN_ECLIPSE__=1 -DARDUINO=100 -Wall -g2 -gstabs -Os -ffunction-sections -fdata-sections -fno-exceptions -g -mmcu=atmega1280 -DF_CPU=16000000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -c -o "$@" -x c++ "$<"
	@echo 'Finished building: $<'
	@echo ' '


