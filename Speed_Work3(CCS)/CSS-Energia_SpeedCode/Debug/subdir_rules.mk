################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
CSS-Energia_SpeedCode.cpp: ../CSS-Energia_SpeedCode.ino
	@echo 'Building file: "$<"'
	@echo 'Invoking: Resource Custom Build Step'
	
	@echo 'Finished building: "$<"'
	@echo ' '

%.o: ./%.cpp $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: GNU Compiler'
	"C:/Users/louis/AppData/Local/Energia15/packages/energia/tools/msp430-gcc/4.6.6/bin/msp430-gcc-4.6.3.exe" -c -mmcu=msp430f5529 -DF_CPU=25000000L -DENERGIA_MSP_EXP430F5529LP -DENERGIA_ARCH_MSP430 -DENERGIA=23 -DARDUINO=10610 -I"C:/Users/louis/AppData/Local/Energia15/packages/energia/tools/msp430-gcc/4.6.6/include" -I"C:/Users/louis/AppData/Local/Energia15/packages/energia/hardware/msp430/1.0.7/cores/msp430" -I"C:/Users/louis/AppData/Local/Energia15/packages/energia/hardware/msp430/1.0.7/variants/MSP-EXP430F5529LP" -I"C:/Users/louis/workspace_v11/CSS-Energia_SpeedCode" -I"C:/Users/louis/AppData/Local/Energia15/packages/energia/tools/msp430-gcc/4.6.6/msp430/include" -Os -ffunction-sections -fdata-sections -g -gdwarf-3 -gstrict-dwarf -w -Wall -MMD -MP -MF"$(basename $(<F)).d_raw" -MT"$(@)"   -fno-threadsafe-statics -fno-exceptions $(GEN_OPTS__FLAG) -o"$@" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


