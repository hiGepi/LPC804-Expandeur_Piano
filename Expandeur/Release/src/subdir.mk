################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_SRCS += \
../src/aeabi_romdiv_patch.s 

C_SRCS += \
../src/Expander_poly_v2.c \
../src/cr_startup_lpc80x.c \
../src/crp.c \
../src/lib_ENS_II1_lcd.c 

OBJS += \
./src/Expander_poly_v2.o \
./src/aeabi_romdiv_patch.o \
./src/cr_startup_lpc80x.o \
./src/crp.o \
./src/lib_ENS_II1_lcd.o 

C_DEPS += \
./src/Expander_poly_v2.d \
./src/cr_startup_lpc80x.d \
./src/crp.d \
./src/lib_ENS_II1_lcd.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -DNDEBUG -D__CODE_RED -DCORE_M0PLUS -D__USE_ROMDIVIDE -D__LPC80X__ -D__REDLIB__ -I"C:\Users\Sasa\Desktop\M1_E3A\Info_indus\Programme\Synthetiseur\inc" -Os -fno-common -g -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -fmacro-prefix-map="../$(@D)/"=. -mcpu=cortex-m0 -mthumb -fstack-usage -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/%.o: ../src/%.s
	@echo 'Building file: $<'
	@echo 'Invoking: MCU Assembler'
	arm-none-eabi-gcc -c -x assembler-with-cpp -DNDEBUG -D__CODE_RED -DCORE_M0PLUS -D__USE_ROMDIVIDE -D__LPC80X__ -D__REDLIB__ -I"C:\Users\Sasa\Desktop\M1_E3A\Info_indus\Programme\Synthetiseur\inc" -mcpu=cortex-m0 -mthumb -specs=redlib.specs -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


