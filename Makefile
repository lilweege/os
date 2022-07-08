SRC = src
BUILD = build
ISO = $(BUILD)/os.iso

ASM_SOURCES = $(wildcard $(SRC)/kernel/*.asm)
C_SOURCES = $(wildcard $(SRC)/kernel/*.c)
HEADERS = $(wildcard $(SRC)/kernel/*.h)
OBJS = $(patsubst $(SRC)/kernel/%.c,$(BUILD)/%.o,$(C_SOURCES)) $(patsubst $(SRC)/kernel/%.asm,$(BUILD)/%.o,$(ASM_SOURCES))
CFLAGS = -std=gnu17 -g -m32 -mgeneral-regs-only -mno-red-zone -ffreestanding -Wall -Wextra
SFLAGS = -g


run: $(ISO)
	qemu-system-i386 -drive file=$<,format=raw

debug: $(ISO) $(BUILD)/kernel.elf
	qemu-system-i386 -s -S -drive file=$<,format=raw &
	i386-elf-gdb $(BUILD)/kernel.elf -ex "target remote localhost:1234"


$(BUILD)/%.o: $(SRC)/kernel/%.c $(HEADERS)
	i386-elf-gcc $(CFLAGS) -c $< -o $@
$(BUILD)/%.o: $(SRC)/kernel/%.asm
	nasm $< $(SFLAGS) -felf -o $@

$(BUILD)/%.bin: $(SRC)/boot/%.asm
	nasm $< -fbin -o $@


$(BUILD)/kernel.elf: $(BUILD)/kernel_entry.o $(OBJS)
	i386-elf-ld -o $@ -Ttext 0x7E00 $^
$(BUILD)/kernel.bin: $(BUILD)/kernel_entry.o $(OBJS)
	i386-elf-ld -o $@ -Ttext 0x7E00 $^ --oformat binary

$(ISO): $(BUILD)/bootsect.bin $(BUILD)/kernel.bin
	dd if=/dev/zero of=$@ bs=512 count=2880
	cat $^ | dd of=$@ conv=notrunc

.PHONY: clean
clean:
	rm -f $(BUILD)/*
