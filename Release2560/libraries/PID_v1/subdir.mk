################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../libraries/PID_v1/PID_v1.cpp 

OBJS += \
./libraries/PID_v1/PID_v1.o 

CPP_DEPS += \
./libraries/PID_v1/PID_v1.d 


# Each subdirectory must supply rules for building sources it contributes
libraries/PID_v1/%.o: ../libraries/PID_v1/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: AVR C++ Compiler'
	avr-g++ -I"/home/robert/gitrep/Arduino_Mega_ATmega2560/arduino" -I"/home/robert/gitrep/freeboardPLC/libraries/MemoryFree" -I"/home/robert/gitrep/freeboardPLC/libraries/SPI" -I"/home/robert/gitrep/freeboardPLC/libraries/PString" -I"/home/robert/gitrep/freeboardPLC/libraries/AverageList" -I"/home/robert/gitrep/freeboardPLC/libraries/PID_v1" -I"/home/robert/gitrep/freeboardPLC/libraries/FlexiTimer2" -I"/home/robert/gitrep/freeboardPLC/libraries/EEPROM" -I"/home/robert/gitrep/freeboardPLC/libraries/NMEA" -I"/home/robert/gitrep/freeboardPLC/libraries/AltSoftSerial" -I"/home/robert/gitrep/freeboardPLC/libraries/MultiSerial" -I"/home/robert/gitrep/freeboardPLC/libraries/Kangaroo" -I"/home/robert/gitrep/freeboardPLC/libraries/JsonStream" -I"/home/robert/gitrep/freeboardPLC/libraries" -I/usr/lib/gcc/avr/4.7.2/include-fixed -I/usr/lib/gcc/avr/4.7.2/include -D__IN_ECLIPSE__=1 -DARDUINO=100 -Wall -g2 -gstabs -Os -ffunction-sections -fdata-sections -fno-exceptions -g -felide-constructors -std=c++0x -MMD -mmcu=atmega2560 -DF_CPU=16000000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -c -o "$@" -x c++ "$<"
	@echo 'Finished building: $<'
	@echo ' '


