USE_FRAMEBUFFER := 1
SMALL_PAGES := 0
CFLAGS := -std=gnu99 \
        -ffreestanding \
        -O2 \
        -Wall \
        -Wextra \
        -I src/include \
        -I src/include/base \
        -I src/include/kernel \
        -I src/include/kernel/mem \
        -I src/include/kernel/x86_64 \
        -I src/include/kernel/hardware \
        -I src/include/libc \
        -mno-red-zone \
        -mcmodel=large \
        -DUSE_FRAMEBUFFER=$(USE_FRAMEBUFFER) \
        -DSMALL_PAGES=$(SMALL_PAGES)

TESTFLAGS := -std=gnu99 \
        -I tests/include \
        -I src/include \
        -I src/include/base \
        -I src/include/kernel/mem \
        -I src/include/kernel \
        -DSMALL_PAGES=$(SMALL_PAGES) \
        -D_TEST_=1

TEST_FOLDER := tests
C_DEBUG_FLAGS := -g \
				-DDEBUG=1
NASM_DEBUG_FLAGS := -g \
					-F dwarf
NASMFLAGS := -f elf64 \
		-D USE_FRAMEBUFFER=$(USE_FRAMEBUFFER) \
		-D SMALL_PAGES=$(SMALL_PAGES)
BUILD := build
PRJ_FOLDERS := src
FONT_FOLDERS := fonts
DEBUG := 0
VERBOSE_OUTPUT :=0

SRC_C_FILES := $(shell find $(PRJ_FOLDERS) -type f -name "*.c")
SRC_H_FILES := $(shell find $(PRJ_FOLDERS) -type f -name "*.h")
SRC_ASM_FILES := $(shell find $(PRJ_FOLDERS) -type f -name "*.s")
SRC_FONT_FILES := $(shell find $(FONT_FOLDERS) -type f -name "*.psf")
OBJ_ASM_FILE := $(patsubst src/%.s, build/%.o, $(SRC_ASM_FILES))
OBJ_C_FILE := $(patsubst src/%.c, build/%.o, $(SRC_C_FILES))
OBJ_FONT_FILE := $(patsubst fonts/%.psf, build/%.o, $(SRC_FONT_FILES))
default: build

.PHONY: default build run clean debug tests

build: build/os.iso

clean:
	rm -rf build
run: build/os.iso
	qemu-system-x86_64 -cdrom build/DreamOs64.iso

debug: DEBUG=1
debug: build/os.iso
	qemu-system-x86_64 -monitor unix:qemu-monitor-socket,server,nowait -cdrom build/DreamOs64.iso -serial file:dreamos64.log -m 1G -d int -no-reboot

build/os.iso: build/kernel.bin grub.cfg
	mkdir -p build/isofiles/boot/grub
	cp grub.cfg build/isofiles/boot/grub
	cp build/kernel.bin build/isofiles/boot
	cp build/kernel.map build/isofiles/boot
	grub-mkrescue -o build/DreamOs64.iso build/isofiles

build/%.o: src/%.s
	echo "$(<D)"
	mkdir -p "$(@D)"
	mkdir -p "$(@D)"
ifeq ($(DEBUG),'0')
	nasm ${NASMFLAGS} "$<" -o "$@"
else
	nasm ${NASM_DEBUG_FLAGS} ${NASMFLAGS} "$<" -o "$@"
endif

build/%.o: src/%.c
	echo "$(@D)"
	mkdir -p "$(@D)"
ifeq ($(DEBUG),'0')
		x86_64-elf-gcc ${CFLAGS} -c "$<" -o "$@"
else
		@echo "Compiling with DEBUG flag"
		x86_64-elf-gcc ${CFLAGS} ${C_DEBUG_FLAGS} -c "$<" -o "$@"
endif

build/%.o: fonts/%.psf
	echo "PSF: $(@D)"
	mkdir -p "$(@D)"
	objcopy -O elf64-x86-64 -B i386 -I binary "$<" "$@"

build/kernel.bin: $(OBJ_ASM_FILE) $(OBJ_C_FILE) $(OBJ_FONT_FILE) src/linker.ld
	echo $(OBJ_ASM_FILE)
	echo $(OBJ_FONT_FILE)
	ld -n -o build/kernel.bin -T src/linker.ld $(OBJ_ASM_FILE) $(OBJ_C_FILE) $(OBJ_FONT_FILE) -Map build/kernel.map

gdb: DEBUG=1
gdb: build/os.iso
	qemu-system-x86_64 -cdrom build/DreamOs64.iso -serial file:dreamos64.log -m 1G -d int -no-reboot -s -S

tests:
	gcc ${TESTFLAGS} tests/test_mem.c tests/test_common.c src/kernel/mem/bitmap.c src/kernel/mem/pmm.c src/kernel/mem/mmap.c -o tests/test_mem.o
	gcc ${TESTFLAGS} tests/test_number_conversion.c tests/test_common.c src//base/numbers.c -o tests/test_number_conversion.o
	gcc ${TESTFLAGS} tests/test_kheap.c src/kernel/mem/kheap.c tests/test_common.c -o tests/test_kheap.o
	./tests/test_mem.o && ./tests/test_kheap.o

