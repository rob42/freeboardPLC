################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include libraries/PString/subdir.mk
-include libraries/PID_v1/subdir.mk
-include libraries/NMEA/subdir.mk
-include libraries/FlexiTimer2/subdir.mk
-include libraries/EEPROM/subdir.mk
-include libraries/AltSoftSerial/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(INO_DEPS)),)
-include $(INO_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
LSS += \
FreeBoardPLC.lss \

FLASH_IMAGE += \
FreeBoardPLC.hex \

EEPROM_IMAGE += \
FreeBoardPLC.eep \

SIZEDUMMY += \
sizedummy \


# All Target
all: FreeBoardPLC.elf secondary-outputs

dependents:
	-cd /home/robert/gitrep/Arduino_Mega_ATmega2560/Release && $(MAKE) clean all

# Tool invocations
FreeBoardPLC.elf: $(OBJS) $(USER_OBJS) /home/robert/gitrep/Arduino_Mega_ATmega2560/Release/libArduino_Mega_ATmega2560.a
	@echo 'Building target: $@'
	@echo 'Invoking: AVR C++ Linker'
	avr-gcc -Os -Wl,--gc-sections  -L"/home/robert/gitrep/Arduino_Mega_ATmega2560/Release" -mmcu=atmega2560 -o"FreeBoardPLC.elf" $(OBJS) $(USER_OBJS) $(LIBS) -lm
	@echo 'Finished building target: $@'
	@echo ' '

FreeBoardPLC.lss: FreeBoardPLC.elf
	@echo 'Invoking: AVR Create Extended Listing'
	-avr-objdump -h -S FreeBoardPLC.elf  >"FreeBoardPLC.lss"
	@echo 'Finished building: $@'
	@echo ' '

FreeBoardPLC.hex: FreeBoardPLC.elf
	@echo 'Create Flash image (ihex format)'
	-avr-objcopy -R .eeprom -O ihex FreeBoardPLC.elf  "FreeBoardPLC.hex"
	@echo 'Finished building: $@'
	@echo ' '

FreeBoardPLC.eep: FreeBoardPLC.elf
	@echo 'Create eeprom image (ihex format)'
	-avr-objcopy -j .eeprom --no-change-warnings --change-section-lma .eeprom=0 -O ihex FreeBoardPLC.elf  "FreeBoardPLC.eep"
	@echo 'Finished building: $@'
	@echo ' '

sizedummy: FreeBoardPLC.elf
	@echo 'Invoking: Print Size'
	-avr-size --format=avr --mcu=atmega2560 FreeBoardPLC.elf
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS)$(C_DEPS)$(EEPROM_IMAGE)$(FLASH_IMAGE)$(ELFS)$(LSS)$(S_DEPS)$(CXX_DEPS)$(S_UPPER_DEPS)$(C++_DEPS)$(ASM_DEPS)$(CC_DEPS)$(INO_DEPS)$(CPP_DEPS)$(C_UPPER_DEPS)$(SIZEDUMMY) FreeBoardPLC.elf
	-@echo ' '

secondary-outputs: $(LSS) $(FLASH_IMAGE) $(EEPROM_IMAGE) $(SIZEDUMMY)

.PHONY: all clean dependents
.SECONDARY:
/home/robert/gitrep/Arduino_Mega_ATmega2560/Release/libArduino_Mega_ATmega2560.a:

-include ../makefile.targets
