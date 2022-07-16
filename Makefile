SRC = src
BUILD = build
ISO = $(BUILD)/os.iso

ASM_SOURCES = $(wildcard $(SRC)/kernel/*.asm)
C_SOURCES = $(wildcard $(SRC)/kernel/*.c)
HEADERS = $(wildcard $(SRC)/kernel/*.h)
OBJS = $(patsubst $(SRC)/kernel/%.c,$(BUILD)/%.o,$(C_SOURCES)) $(patsubst $(SRC)/kernel/%.asm,$(BUILD)/%.o,$(ASM_SOURCES))
CCFLAGS = -std=gnu11 -g -O2 -m32 -mno-red-zone -fno-stack-protector -fno-pie \
		 -ffreestanding -fno-builtin -nostdlib -nostdinc -nodefaultlibs \
		 -Wall -Wextra -Werror
ASFLAGS = -g
LDFLAGS = 


iso: $(ISO)

run: $(ISO)
	qemu-system-i386 -drive file=$<,format=raw

debug: $(ISO) $(BUILD)/kernel.elf
	qemu-system-i386 -s -S -drive file=$<,format=raw &
	i386-elf-gdb $(BUILD)/kernel.elf -ex "target remote localhost:1234"


$(BUILD)/%.o: $(SRC)/kernel/%.c $(HEADERS)
	i386-elf-gcc $(CCFLAGS) -c $< -o $@
$(BUILD)/%.o: $(SRC)/kernel/%.asm
	nasm $< $(ASFLAGS) -felf -o $@

$(BUILD)/%.bin: $(SRC)/boot/%.asm
	nasm $< -fbin -o $@


$(BUILD)/kernel.elf: $(BUILD)/kernel_entry.o $(OBJS)
	i386-elf-ld -o $@ -Ttext 0x7E00 $^ $(LDFLAGS)
$(BUILD)/kernel.bin: $(BUILD)/kernel_entry.o $(OBJS)
	i386-elf-ld -o $@ -Ttext 0x7E00 $^ $(LDFLAGS) --oformat binary

$(ISO): $(BUILD)/bootsect.bin $(BUILD)/kernel.bin
	dd if=/dev/zero of=$@ bs=512 count=2880
	cat $^ | dd of=$@ conv=notrunc

.PHONY: clean
clean:
	rm -f $(BUILD)/*
