CFLAGS := -std=gnu99 \
		-ffreestanding \
		-O2 \
		-Wall \
		-Wextra \
		-I src/include \
		-I src/include/kernel

BUILD := build
PRJ_FOLDERS := src

SRC_C_FILES := $(shell find $(PRJ_FOLDERS) -type f -name "\*.c")
SRC_H_FILES := $(shell find $(PRJ_FOLDERS) -type f -name "\*.h")
SRC_ASM_FILES := $(shell find $(PRJ_FOLDERS) -type f -name "\*.")

default: build

.PHONY: default build run clean debug

build: build/os.iso

clean:
	rm -rf build
run: build/os.iso
	qemu-system-x86_64 -cdrom build/DreamOs64.iso

debug: build/os.iso
	qemu-system-x86_64 -cdrom build/DreamOs64.iso -serial file:dreamos64.log

build/os.iso: build/kernel.bin grub.cfg
	mkdir -p build/isofiles/boot/grub
	cp grub.cfg build/isofiles/boot/grub
	cp build/kernel.bin build/isofiles/boot
	grub-mkrescue -o build/DreamOs64.iso build/isofiles

build/multiboot_header.o: src/asm/multiboot_header.s
	mkdir -p build
	nasm -f elf64 src/asm/multiboot_header.s -o build/multiboot_header.o

build/boot.o: src/asm/boot.s
	mkdir -p build
	nasm -f elf64 src/asm/boot.s -o build/boot.o

build/main.o: src/asm/main.s
	mkdir -p build
	nasm -f elf64 src/asm/main.s -o build/main.o

build/%.o: src/%.c
	echo "$(@D)"
	mkdir -p "$(@D)"
	x86_64-elf-gcc ${CFLAGS} -c "$<" -o "$@"

build/kernel.bin: build/multiboot_header.o build/boot.o build/kernel/cpu/idt.o build/kernel/debug/qemu.o build/kernel/io/serial.o build/main.o build/kernel/main.o build/kernel/io/video.o build/kernel/io/io.o src/linker.ld
	ld -n -o build/kernel.bin -T src/linker.ld build/multiboot_header.o build/kernel/cpu/idt.o build/boot.o build/main.o build/kernel/main.o build/kernel/io/io.o build/kernel/io/video.o build/kernel/io/serial.o build/kernel/debug/qemu.o -Map build/kernel.map

