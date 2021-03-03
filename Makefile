CFLAGS := -std=gnu99 \
		-ffreestanding \
		-O2 \
		-Wall \
		-Wextra \
		-I src/include \
		-I src/include/kernel

BUILD := build
PRJ_FOLDERS := src

SRC_C_FILES := $(shell find $(PRJ_FOLDERS) -type f -name "*.c")
SRC_H_FILES := $(shell find $(PRJ_FOLDERS) -type f -name "*.h")
SRC_ASM_FILES := $(shell find $(PRJ_FOLDERS) -type f -name "*.s")
OBJ_ASM_FILE := $(patsubst src/%.s, build/%.o, $(SRC_ASM_FILES))
OBJ_C_FILE := $(patsubst src/%.c, build/%.o, $(SRC_C_FILES))

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

build/%.o: src/%.s
	echo "$(<D)"
	mkdir -p "$(@D)"
	nasm -f elf64 "$<" -o "$@"

build/%.o: src/%.c
	echo "$(@D)"
	mkdir -p "$(@D)"
	x86_64-elf-gcc ${CFLAGS} -c "$<" -o "$@"

build/kernel.bin: $(OBJ_ASM_FILE) $(OBJ_C_FILE) src/linker.ld
	echo $(OBJ_ASM_FILE)
	ld -n -o build/kernel.bin -T src/linker.ld $(OBJ_ASM_FILE) $(OBJ_C_FILE) -Map build/kernel.map


