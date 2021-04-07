USE_FRAMEBUFFER := 1
PAGE_SIZE := 2
CFLAGS := -std=gnu99 \
		-ffreestanding \
		-O2 \
		-Wall \
		-Wextra \
		-I src/include \
		-I src/include/kernel \
		-I src/include/kernel/mem \
		-I src/include/kernel/x86_64 \
		-I src/include/libc \
		-mno-red-zone \
		-mcmodel=large \
		-DUSE_FRAMEBUFFER=$(USE_FRAMEBUFFER) \
		-DPAGE_SIZE=$(PAGE_SIZE)
NASMFLAGS := -f elf64 \
		-D USE_FRAMEBUFFER=$(USE_FRAMEBUFFER) 
BUILD := build
PRJ_FOLDERS := src
FONT_FOLDERS := fonts
DEBUG := 0

SRC_C_FILES := $(shell find $(PRJ_FOLDERS) -type f -name "*.c")
SRC_H_FILES := $(shell find $(PRJ_FOLDERS) -type f -name "*.h")
SRC_ASM_FILES := $(shell find $(PRJ_FOLDERS) -type f -name "*.s")
SRC_FONT_FILES := $(shell find $(FONT_FOLDERS) -type f -name "*.psf")
OBJ_ASM_FILE := $(patsubst src/%.s, build/%.o, $(SRC_ASM_FILES))
OBJ_C_FILE := $(patsubst src/%.c, build/%.o, $(SRC_C_FILES))
OBJ_FONT_FILE := $(patsubst fonts/%.psf, build/%.o, $(SRC_FONT_FILES))
default: build

.PHONY: default build run clean debug

build: build/os.iso

clean:
	rm -rf build
run: build/os.iso
	qemu-system-x86_64 -cdrom build/DreamOs64.iso

debug: DEBUG=1
debug: build/os.iso
	qemu-system-x86_64 -cdrom build/DreamOs64.iso -serial file:dreamos64.log -m 1G -d int -no-reboot

build/os.iso: build/kernel.bin grub.cfg
	mkdir -p build/isofiles/boot/grub
	cp grub.cfg build/isofiles/boot/grub
	cp build/kernel.bin build/isofiles/boot
	grub-mkrescue -o build/DreamOs64.iso build/isofiles

build/%.o: src/%.s
	echo "$(<D)"
	mkdir -p "$(@D)"
	nasm ${NASMFLAGS} "$<" -o "$@"

build/%.o: src/%.c
	echo "$(@D)"
	mkdir -p "$(@D)"
ifeq ($(DEBUG),'0')
		x86_64-elf-gcc ${CFLAGS} -c "$<" -o "$@"
else
		@echo "Compiling with DEBUG flag"
		x86_64-elf-gcc ${CFLAGS} -DDEBUG=1 -c "$<" -o "$@"
endif

build/%.o: fonts/%.psf
	echo "PSF: $(@D)"
	mkdir -p "$(@D)"
	objcopy -O elf64-x86-64 -B i386 -I binary "$<" "$@"

build/kernel.bin: $(OBJ_ASM_FILE) $(OBJ_C_FILE) $(OBJ_FONT_FILE) src/linker.ld
	echo $(OBJ_ASM_FILE)
	echo $(OBJ_FONT_FILE)
	ld -n -o build/kernel.bin -T src/linker.ld $(OBJ_ASM_FILE) $(OBJ_C_FILE) $(OBJ_FONT_FILE) -Map build/kernel.map


