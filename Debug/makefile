################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include Drivers/STM32L0xx_HAL_Driver/Src/subdir.mk
-include Core/Startup/subdir.mk
-include Core/Src/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
EXECUTABLES += \
LoRa_fSTM32.elf \

SIZE_OUTPUT += \
default.size.stdout \

OBJDUMP_LIST += \
LoRa_fSTM32.list \

OBJCOPY_BIN += \
LoRa_fSTM32.bin \


# All Target
all: LoRa_fSTM32.elf secondary-outputs

# Tool invocations
LoRa_fSTM32.elf: $(OBJS) $(USER_OBJS) F:\arm_Mx\LoRa_fSTM32\STM32L073CZTX_FLASH.ld
	arm-none-eabi-gcc -o "LoRa_fSTM32.elf" @"objects.list" $(USER_OBJS) $(LIBS) -mcpu=cortex-m0plus -T"F:\arm_Mx\LoRa_fSTM32\STM32L073CZTX_FLASH.ld" --specs=nosys.specs -Wl,-Map="LoRa_fSTM32.map" -Wl,--gc-sections -static --specs=nano.specs -mfloat-abi=soft -mthumb -Wl,--start-group -lc -lm -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

default.size.stdout: $(EXECUTABLES)
	arm-none-eabi-size  $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

LoRa_fSTM32.list: $(EXECUTABLES)
	arm-none-eabi-objdump -h -S $(EXECUTABLES) > "LoRa_fSTM32.list"
	@echo 'Finished building: $@'
	@echo ' '

LoRa_fSTM32.bin: $(EXECUTABLES)
	arm-none-eabi-objcopy  -O binary $(EXECUTABLES) "LoRa_fSTM32.bin"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) *
	-@echo ' '

secondary-outputs: $(SIZE_OUTPUT) $(OBJDUMP_LIST) $(OBJCOPY_BIN)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
