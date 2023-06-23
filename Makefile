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
        -I src/include/kernel/scheduling \
        -I src/include/libc \
        -I src/include/fs \
        -I src/include/drivers/fs \
        -I src/include/sys \
        -mno-red-zone \
        -mno-sse \
        -mcmodel=large \
        -DUSE_FRAMEBUFFER=$(USE_FRAMEBUFFER) \
        -DSMALL_PAGES=$(SMALL_PAGES)

TESTFLAGS := -std=gnu99 \
        -I tests/include \
        -I src/include \
        -I src/include/base \
        -I src/include/kernel/mem \
        -I src/include/fs \
        -I src/include/drivers/fs \
        -I src/include/kernel \
        -I src/include/kernel/x86_64 \
        -I src/include/sys \
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
BUILD := dist
PRJ_FOLDERS := src
FONT_FOLDERS := fonts
DEBUG := 0
VERBOSE_OUTPUT :=0

SRC_C_FILES := $(shell find $(PRJ_FOLDERS) -type f -name "*.c")
SRC_H_FILES := $(shell find $(PRJ_FOLDERS) -type f -name "*.h")
SRC_ASM_FILES := $(shell find $(PRJ_FOLDERS) -type f -name "*.s")
SRC_FONT_FILES := $(shell find $(FONT_FOLDERS) -type f -name "*.psf")
OBJ_ASM_FILE := $(patsubst src/%.s, $(BUILD)/%.o, $(SRC_ASM_FILES))
OBJ_C_FILE := $(patsubst src/%.c, $(BUILD)/%.o, $(SRC_C_FILES))
OBJ_FONT_FILE := $(patsubst fonts/%.psf, $(BUILD)/%.o, $(SRC_FONT_FILES))
default: build

.PHONY: default build run clean debug tests gdb

build: $(BUILD)/os.iso

clean:
	rm -rf $(BUILD)
	find -name *.o -type f -delete

run: $(BUILD)/os.iso
	qemu-system-x86_64 -cdrom $(BUILD)/DreamOs64.iso

debug: DEBUG=1
debug: $(BUILD)/os.iso
# qemu-system-x86_64 -monitor unix:qemu-monitor-socket,server,nowait -cpu qemu64,+x2apic  -cdrom build/DreamOs64.iso -serial file:dreamos64.log -m 1G -d int -no-reboot -no-shutdown
	qemu-system-x86_64 -monitor unix:qemu-monitor-socket,server,nowait -cpu qemu64,+x2apic  -cdrom $(BUILD)/DreamOs64.iso -serial stdio -m 2G  -no-reboot -no-shutdown

$(BUILD)/os.iso: $(BUILD)/kernel.bin grub.cfg
	mkdir -p $(BUILD)/isofiles/boot/grub
	cp grub.cfg $(BUILD)/isofiles/boot/grub
	cp $(BUILD)/kernel.bin $(BUILD)/isofiles/boot
	cp $(BUILD)/kernel.map $(BUILD)/isofiles/boot
	grub-mkrescue -o $(BUILD)/DreamOs64.iso $(BUILD)/isofiles

$(BUILD)/%.o: src/%.s
	echo "$(<D)"
	mkdir -p "$(@D)"
	mkdir -p "$(@D)"
ifeq ($(DEBUG),'0')
	nasm ${NASMFLAGS} "$<" -o "$@"
else
	nasm ${NASM_DEBUG_FLAGS} ${NASMFLAGS} "$<" -o "$@"
endif

$(BUILD)/%.o: src/%.c
	echo "$(@D)"
	mkdir -p "$(@D)"
ifeq ($(DEBUG),'0')
		x86_64-elf-gcc ${CFLAGS} -c "$<" -o "$@"
else
		@echo "Compiling with DEBUG flag"
		x86_64-elf-gcc ${CFLAGS} ${C_DEBUG_FLAGS} -c "$<" -o "$@"
endif

$(BUILD)/%.o: fonts/%.psf
	echo "PSF: $(@D)"
	mkdir -p "$(@D)"
	objcopy -O elf64-x86-64 -B i386 -I binary "$<" "$@"

$(BUILD)/kernel.bin: $(OBJ_ASM_FILE) $(OBJ_C_FILE) $(OBJ_FONT_FILE) src/linker.ld
	echo $(OBJ_ASM_FILE)
	echo $(OBJ_FONT_FILE)
	ld -n -o $(BUILD)/kernel.bin -T src/linker.ld $(OBJ_ASM_FILE) $(OBJ_C_FILE) $(OBJ_FONT_FILE) -Map $(BUILD)/kernel.map

gdb: DEBUG=1
gdb: $(BUILD)/os.iso
	qemu-system-x86_64 -cdrom $(BUILD)/DreamOs64.iso -monitor unix:qemu-monitor-socket,server,nowait -serial file:dreamos64.log -m 1G -d int -no-reboot -no-shutdown -s -S 

tests:
	gcc ${TESTFLAGS} tests/test_mem.c tests/test_common.c src/kernel/mem/bitmap.c src/kernel/mem/vmm_util.c src/kernel/mem/pmm.c src/kernel/mem/mmap.c -o tests/test_mem.o
	gcc ${TESTFLAGS} tests/test_number_conversion.c tests/test_common.c src/base/numbers.c -o tests/test_number_conversion.o
	gcc ${TESTFLAGS} tests/test_kheap.c tests/test_common.c src/kernel/mem/kheap.c src/kernel/mem/bitmap.c src/kernel/mem/pmm.c src/kernel/mem/mmap.c src/kernel/mem/vmm_util.c -o tests/test_kheap.o
	gcc ${TESTFLAGS} tests/test_vm.c tests/test_common.c src/kernel/arch/x86_64/system/vm.c -o tests/test_vm.o
	gcc ${TESTFLAGS} tests/test_vfs.c tests/test_common.c src/fs/vfs.c src/drivers/fs/ustar.c -o tests/test_vfs.o
	gcc ${TESTFLAGS} tests/test_utils.c src/kernel/mem/vmm_util.c -o tests/test_utils.o
	./tests/test_mem.o && ./tests/test_kheap.o && ./tests/test_number_conversion.o && ./tests/test_vm.o && ./tests/test_vfs.o && ./tests/test_utils.o

