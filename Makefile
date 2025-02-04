NAME = visualizer
# SAMPLES_LG >= 7
SAMPLES_LG = 8

CPU = cortex-m3
MCU = STM32F103xB
LINKER = cmsis-device-f1/Source/Templates/gcc/linker/STM32F103XB_FLASH.ld

INCLUDE  = -Icmsis-device-f1/Include
INCLUDE += -ICMSIS_6/CMSIS/Core/Include

CFLAGS  = -std=c99 -Wall -g
CFLAGS += -mcpu=$(CPU) -D$(MCU)
CFLAGS += -mthumb
CFLAGS += $(INCLUDE)
CFLAGS += -DSAMPLES_LG=$(SAMPLES_LG)

ASFLAGS  = -mcpu=$(CPU)

LDFLAGS  = -T$(LINKER) -lc
LDFLAGS += -mthumb -mcpu=$(CPU)
LDFLAGS += --specs=nosys.specs --specs=nano.specs

SYS  = cmsis-device-f1/Source/Templates/system_stm32f1xx.c
STARTUP = cmsis-device-f1/Source/Templates/gcc/startup_stm32f103xb.s

SRC = $(wildcard src/*.c) $(SYS)
ASM  = $(STARTUP)

OBJ  = $(addprefix obj/,$(notdir $(SRC:.c=.o)))
OBJ += $(addprefix obj/,$(notdir $(ASM:.s=.o)))

CC     = arm-none-eabi-gcc
AS     = arm-none-eabi-as
OBJCPY = arm-none-eabi-objcopy

all: clean build flash

build: $(NAME).bin

src/tables.h: table_gen.py
	./table_gen.py $(SAMPLES_LG)

obj/%.o: src/%.c
	@mkdir -p obj
	$(CC) $(CFLAGS) -c $< -o $@

obj/%.o: cmsis-device-f1/Source/Templates/%.c
	@mkdir -p obj
	$(CC) $(CFLAGS) -c $< -o $@

obj/%.o: cmsis-device-f1/Source/Templates/gcc/%.s
	@mkdir -p obj
	$(AS) $(ASFLAGS) $< -o $@

%.s: src/%.c
	$(CC) $(CFLAGS) -S $< -o $@

$(NAME).elf: src/tables.h $(OBJ)
	$(CC) $(OBJ) $(LDFLAGS) -o $(NAME).elf

$(NAME).bin: $(NAME).elf
	$(OBJCPY) -R .stack -O binary $(NAME).elf $(NAME).bin

flash: $(NAME).bin
	sha256sum $(NAME).bin
	sudo stm32flash /dev/ttyACM0 -w $(NAME).bin

clean:
	rm -f obj/*.o *.s src/tables.h *.elf *.bin
