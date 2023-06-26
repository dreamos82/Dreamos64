include build/Config.mk
include build/Common.mk

VERSION := 0.0

# arch dependant
ARCH_PREFIX := x86_64-elf
ASM_COMPILER := nasm
QEMU_SYSTEM := qemu-system-x86_64

ASM_DEBUG_FLAGS := -g \
					-F dwarf
ASM_FLAGS := -f elf64
ASM_FLAGS += $(DEF_FLAGS)

DEBUG := 0

SRC_C_FILES := $(shell find $(PRJ_FOLDERS) -type f -name "*.c")
SRC_H_FILES := $(shell find $(PRJ_FOLDERS) -type f -name "*.h")
SRC_ASM_FILES := $(shell find $(PRJ_FOLDERS) -type f -name "*.s")
SRC_FONT_FILES := $(shell find $(FONT_FOLDERS) -type f -name "*.psf")
OBJ_ASM_FILE := $(patsubst src/%.s, $(BUILD_FOLDER)/%.o, $(SRC_ASM_FILES))
OBJ_C_FILE := $(patsubst src/%.c, $(BUILD_FOLDER)/%.o, $(SRC_C_FILES))
OBJ_FONT_FILE := $(patsubst fonts/%.psf, $(BUILD_FOLDER)/%.o, $(SRC_FONT_FILES))

default: build

.PHONY: default build run clean debug tests gdb

build: $(BUILD_FOLDER)/$(IMAGE_BASE_NAME)-$(ARCH_PREFIX)-$(VERSION).iso

clean:
	-rm -rf $(BUILD_FOLDER)
	-find -name *.o -type f -delete

run: $(BUILD_FOLDER)/$(IMAGE_BASE_NAME)-$(ARCH_PREFIX)-$(VERSION).iso
	$(QEMU_SYSTEM) -cdrom $(BUILD_FOLDER)/$(IMAGE_BASE_NAME)-$(ARCH_PREFIX)-$(VERSION).iso

debug: DEBUG=1
debug: $(BUILD_FOLDER)/$(IMAGE_BASE_NAME)-$(ARCH_PREFIX)-$(VERSION).iso
# qemu-system-x86_64 -monitor unix:qemu-monitor-socket,server,nowait -cpu qemu64,+x2apic  -cdrom build/DreamOs64.iso -serial file:dreamos64.log -m 1G -d int -no-reboot -no-shutdown
	$(QEMU_SYSTEM) -monitor unix:qemu-monitor-socket,server,nowait -cpu qemu64,+x2apic  -cdrom $(BUILD_FOLDER)/$(IMAGE_BASE_NAME)-$(ARCH_PREFIX)-$(VERSION).iso -serial stdio -m 2G  -no-reboot -no-shutdown

$(BUILD_FOLDER)/$(IMAGE_BASE_NAME)-$(ARCH_PREFIX)-$(VERSION).iso: $(BUILD_FOLDER)/kernel.bin grub.cfg
	mkdir -p $(BUILD_FOLDER)/isofiles/boot/grub
	cp grub.cfg $(BUILD_FOLDER)/isofiles/boot/grub
	cp $(BUILD_FOLDER)/kernel.bin $(BUILD_FOLDER)/isofiles/boot
	cp $(BUILD_FOLDER)/kernel.map $(BUILD_FOLDER)/isofiles/boot
	grub-mkrescue -o $(BUILD_FOLDER)/$(IMAGE_BASE_NAME)-$(ARCH_PREFIX)-$(VERSION).iso $(BUILD_FOLDER)/isofiles

$(BUILD_FOLDER)/%.o: src/%.s
	echo "$(<D)"
	mkdir -p "$(@D)"
	mkdir -p "$(@D)"
ifeq ($(DEBUG),'0')
	nasm ${ASM_FLAGS} "$<" -o "$@"
else
	nasm ${ASM_DEBUG_FLAGS} ${ASM_FLAGS} "$<" -o "$@"
endif

$(BUILD_FOLDER)/%.o: src/%.c
	echo "$(@D)"
	mkdir -p "$(@D)"
ifeq ($(DEBUG),'0')
		$(ARCH_PREFIX)-gcc ${CFLAGS} -c "$<" -o "$@"
else
		@echo "Compiling with DEBUG flag"
		$(ARCH_PREFIX)-gcc ${CFLAGS} ${C_DEBUG_FLAGS} -c "$<" -o "$@"
endif

$(BUILD_FOLDER)/%.o: fonts/%.psf
	echo "PSF: $(@D)"
	mkdir -p "$(@D)"
	objcopy -O elf64-x86-64 -B i386 -I binary "$<" "$@"

$(BUILD_FOLDER)/kernel.bin: $(OBJ_ASM_FILE) $(OBJ_C_FILE) $(OBJ_FONT_FILE) src/linker.ld
	echo $(OBJ_ASM_FILE)
	echo $(OBJ_FONT_FILE)
	$(ARCH_PREFIX)-ld -n -o $(BUILD_FOLDER)/kernel.bin -T src/linker.ld $(OBJ_ASM_FILE) $(OBJ_C_FILE) $(OBJ_FONT_FILE) -Map $(BUILD_FOLDER)/kernel.map

gdb: DEBUG=1
gdb: $(BUILD_FOLDER)/$(IMAGE_BASE_NAME)-$(ARCH_PREFIX)-$(VERSION).iso
	$(QEMU_SYSTEM) -cdrom $(BUILD_FOLDER)/$(IMAGE_BASE_NAME)-$(ARCH_PREFIX)-$(VERSION).iso -monitor unix:qemu-monitor-socket,server,nowait -serial file:dreamos64.log -m 1G -d int -no-reboot -no-shutdown -s -S 

tests:
	gcc ${TESTFLAGS} tests/test_mem.c tests/test_common.c src/kernel/mem/bitmap.c src/kernel/mem/vmm_util.c src/kernel/mem/pmm.c src/kernel/mem/mmap.c -o tests/test_mem.o
	gcc ${TESTFLAGS} tests/test_number_conversion.c tests/test_common.c src/base/numbers.c -o tests/test_number_conversion.o
	gcc ${TESTFLAGS} tests/test_kheap.c tests/test_common.c src/kernel/mem/kheap.c src/kernel/mem/bitmap.c src/kernel/mem/pmm.c src/kernel/mem/mmap.c src/kernel/mem/vmm_util.c -o tests/test_kheap.o
	gcc ${TESTFLAGS} tests/test_vm.c tests/test_common.c src/kernel/arch/x86_64/system/vm.c -o tests/test_vm.o
	gcc ${TESTFLAGS} tests/test_vfs.c tests/test_common.c src/fs/vfs.c src/drivers/fs/ustar.c -o tests/test_vfs.o
	gcc ${TESTFLAGS} tests/test_utils.c src/kernel/mem/vmm_util.c -o tests/test_utils.o
	./tests/test_mem.o && ./tests/test_kheap.o && ./tests/test_number_conversion.o && ./tests/test_vm.o && ./tests/test_vfs.o && ./tests/test_utils.o

