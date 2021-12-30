################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include src/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: Synthetiseur.axf

dependents:
	-cd C:/Users/Sasa/Desktop/M1_E3A/Info_indus/Programme/Synthetiseur/Release && $(MAKE) all
	-cd C:/Users/Sasa/Desktop/M1_E3A/Info_indus/Programme/peripherals_lib/flash && $(MAKE) all
	-cd C:/Users/Sasa/Desktop/M1_E3A/Info_indus/Programme/utilities_lib/flash && $(MAKE) all

# Tool invocations
Synthetiseur.axf: $(OBJS) $(USER_OBJS) C:/Users/Sasa/Desktop/M1_E3A/Info_indus/Programme/Synthetiseur/Release/Synthetiseur.axf C:/Users/Sasa/Desktop/M1_E3A/Info_indus/Programme/peripherals_lib/flash/libperipherals_lib.a C:/Users/Sasa/Desktop/M1_E3A/Info_indus/Programme/utilities_lib/flash/libutilities_lib.a
	@echo 'Building target: $@'
	@echo 'Invoking: MCU Linker'
	arm-none-eabi-gcc -nostdlib -L"C:\Users\Sasa\Desktop\M1_E3A\Info_indus\Programme\utilities_lib\flash" -L"C:\Users\Sasa\Desktop\M1_E3A\Info_indus\Programme\peripherals_lib\flash" -Xlinker -Map="Synthetiseur.map" -Xlinker --cref -Xlinker --gc-sections -Xlinker -print-memory-usage -mcpu=cortex-m0 -mthumb -T "Synthetiseur_Debug.ld" -o "Synthetiseur.axf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '
	$(MAKE) --no-print-directory post-build

# Other Targets
clean:
	-$(RM) $(EXECUTABLES)$(OBJS)$(C_DEPS) Synthetiseur.axf
	-@echo ' '

post-build:
	-@echo 'Performing post-build steps'
	-arm-none-eabi-size "Synthetiseur.axf"; # arm-none-eabi-objcopy -v -O binary "Synthetiseur.axf" "Synthetiseur.bin" ; # checksum -p LPC804 -d "Synthetiseur.bin";
	-@echo ' '

.PHONY: all clean dependents post-build
C:/Users/Sasa/Desktop/M1_E3A/Info_indus/Programme/Synthetiseur/Release/Synthetiseur.axf:
C:/Users/Sasa/Desktop/M1_E3A/Info_indus/Programme/peripherals_lib/flash/libperipherals_lib.a:
C:/Users/Sasa/Desktop/M1_E3A/Info_indus/Programme/utilities_lib/flash/libutilities_lib.a:

-include ../makefile.targets
