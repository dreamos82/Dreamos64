include build/Config.mk
include build/Common.mk

VERSION := 0.0.0

# arch dependant
ARCH_PREFIX := x86_64-elf
ASM_COMPILER := nasm
QEMU_SYSTEM := qemu-system-x86_64

IS_WORKFLOW = 0

ifeq ($(TOOLCHAIN), gcc)
	X_CC = $(ARCH_PREFIX)-gcc
	X_LD = $(ARCH_PREFIX)-ld
else ifeq ($(TOOLCHAIN), clang)
	X_CC = clang
	X_LD = ld.lld
	LD_FLAGS += -z lrodata-after-bss
else
	$(error "Unknown compiler toolchain")
endif

C_DEBUG_FLAGS := -g \
		-DDEBUG=1

ASM_DEBUG_FLAGS := -g \
		-F dwarf
ASM_FLAGS := -f elf64
ASM_FLAGS += $(DEF_FLAGS)

DEBUG := 0

SRC_C_FILES := $(shell find $(PRJ_FOLDERS) -type f -name "*.c")
SRC_H_FILES := $(shell find $(PRJ_FOLDERS) -type f -name "*.h")
SRC_ASM_FILES := $(shell find $(PRJ_FOLDERS) -type f -name "*.s")
SRC_FONT_FILES := $(shell find $(FONT_FOLDER) -type f -name "*.psf")
OBJ_ASM_FILE := $(patsubst src/%.s, $(BUILD_FOLDER)/%.o, $(SRC_ASM_FILES))
OBJ_C_FILE := $(patsubst src/%.c, $(BUILD_FOLDER)/%.o, $(SRC_C_FILES))
OBJ_FONT_FILE := $(patsubst $(FONT_FOLDER)/%.psf, $(BUILD_FOLDER)/%.o, $(SRC_FONT_FILES))

ISO_IMAGE_FILENAME := $(IMAGE_BASE_NAME)-$(ARCH_PREFIX)-$(VERSION).iso

default: build

.PHONY: default build run clean debug tests gdb todolist examples

build: $(BUILD_FOLDER)/$(ISO_IMAGE_FILENAME)

clean:
	-rm -rf $(BUILD_FOLDER)
	-find -name *.o -type f -delete

run: $(BUILD_FOLDER)/$(ISO_IMAGE_FILENAME)
	$(QEMU_SYSTEM) -cdrom $(BUILD_FOLDER)/$(ISO_IMAGE_FILENAME) -serial stdio

examples:
	echo "Building Examples"
	mkdir -p $(BUILD_FOLDER)/examples
	${ASM_COMPILER} -g -felf64 -Fdwarf examples/example_syscall.s -o $(BUILD_FOLDER)/examples/example_syscall.o
	$(X_LD) $(LD_FLAGS) -g $(BUILD_FOLDER)/examples/example_syscall.o -o $(BUILD_FOLDER)/examples/example_syscall.elf -e main -T examples/linker_script_$(SMALL_PAGES).ld -Map $(BUILD_FOLDER)/example_syscall.map

debug: DEBUG=1
debug: CFLAGS += $(C_DEBUG_FLAGS)
debug: ASM_FLAGS += $(ASM_DEBUG_FLAGS)
debug: $(BUILD_FOLDER)/$(ISO_IMAGE_FILENAME)
	# TODO could be useful to use  stdio as both log output and monitor input
	# qemu-system-x86_64 -monitor unix:qemu-monitor-socket,server,nowait -cpu qemu64,+x2apic  -cdrom $(BUILD_FOLDER)/$(ISO_IMAGE_FILENAME) -serial file:dreamos64.log -m 1G -d int -no-reboot -no-shutdown
	$(QEMU_SYSTEM) -monitor unix:qemu-monitor-socket,server,nowait -cpu qemu64,+x2apic  -cdrom $(BUILD_FOLDER)/$(ISO_IMAGE_FILENAME) -serial stdio -m 2G  -no-reboot -no-shutdown

tarfs: examples
	mkdir -p $(BUILD_FOLDER)/tarfs
	cp $(BUILD_FOLDER)/examples/example_syscall.o $(BUILD_FOLDER)/tarfs/
	cp README.md $(BUILD_FOLDER)/tarfs/
	tar cf $(BUILD_FOLDER)/filesystem_module.tar -C $(BUILD_FOLDER)/tarfs README.md example_syscall.o

$(BUILD_FOLDER)/$(ISO_IMAGE_FILENAME): $(BUILD_FOLDER)/kernel.bin grub.cfg examples tarfs
	mkdir -p $(BUILD_FOLDER)/isofiles/boot/grub
	cp grub.cfg $(BUILD_FOLDER)/isofiles/boot/grub
	cp $(BUILD_FOLDER)/kernel.bin $(BUILD_FOLDER)/isofiles/boot
	cp $(BUILD_FOLDER)/kernel.map $(BUILD_FOLDER)/isofiles/boot
	cp $(BUILD_FOLDER)/examples/example_syscall.elf $(BUILD_FOLDER)/isofiles
	cp $(BUILD_FOLDER)/filesystem_module.tar $(BUILD_FOLDER)/isofiles
	grub-mkrescue -o $(BUILD_FOLDER)/$(ISO_IMAGE_FILENAME) $(BUILD_FOLDER)/isofiles

$(BUILD_FOLDER)/%.o: src/%.s
	echo "$(<D)"
	mkdir -p "$(@D)"
	${ASM_COMPILER} ${ASM_FLAGS} "$<" -o "$@"

$(BUILD_FOLDER)/%.o: src/%.c
	echo "$(@D)"
	mkdir -p "$(@D)"
	$(X_CC) ${CFLAGS} -DIS_WORKFLOW=$(IS_WORKFLOW) -c "$<" -o "$@"

$(BUILD_FOLDER)/%.o: $(FONT_FOLDER)/%.psf
	echo "PSF: $(@D)"
	mkdir -p "$(@D)"
	objcopy -O elf64-x86-64 -B i386 -I binary "$<" "$@"

$(BUILD_FOLDER)/kernel.bin: $(OBJ_ASM_FILE) $(OBJ_C_FILE) $(OBJ_FONT_FILE) src/linker.ld
	echo $(OBJ_ASM_FILE)
	echo $(OBJ_FONT_FILE)
	echo $(IS_WORKFLOW)
	$(X_LD) $(LD_FLAGS)  -n -o $(BUILD_FOLDER)/kernel.bin -T src/linker.ld $(OBJ_ASM_FILE) $(OBJ_C_FILE) $(OBJ_FONT_FILE) -Map $(BUILD_FOLDER)/kernel.map

gdb: DEBUG=1
gdb: CFLAGS += $(C_DEBUG_FLAGS)
gdb: ASM_FLAGS += $(ASM_DEBUG_FLAGS)
gdb: $(BUILD_FOLDER)/$(ISO_IMAGE_FILENAME)
	$(QEMU_SYSTEM) -cdrom $(BUILD_FOLDER)/$(ISO_IMAGE_FILENAME) -monitor unix:qemu-monitor-socket,server,nowait -serial file:dreamos64.log -m 1G -d int -no-reboot -no-shutdown -s -S

tests:
	rm -f tests/*.o
	${TOOLCHAIN} ${TESTFLAGS} tests/test_mem.c tests/test_common.c src/kernel/mem/bitmap.c src/kernel/mem/vmm_util.c src/kernel/mem/pmm.c src/kernel/mem/mmap.c -o tests/test_mem.o
	${TOOLCHAIN} ${TESTFLAGS} tests/test_number_conversion.c tests/test_common.c src/base/numbers.c -o tests/test_number_conversion.o
	${TOOLCHAIN} ${TESTFLAGS} tests/test_kheap.c tests/test_common.c src/kernel/mem/kheap.c src/kernel/mem/bitmap.c src/kernel/mem/pmm.c src/kernel/mem/mmap.c src/kernel/mem/vmm_util.c -o tests/test_kheap.o
	${TOOLCHAIN} ${TESTFLAGS} tests/test_vm.c tests/test_common.c src/kernel/arch/x86_64/system/vm.c src/kernel/mem/vmm_util.c  -o tests/test_vm.o
	${TOOLCHAIN} ${TESTFLAGS} tests/test_vfs.c tests/test_common.c src/utils/utils.c src/kernel/loaders/elf.c src/kernel/mem/vmm_util.c src/fs/vfs.c src/drivers/fs/ustar.c -o tests/test_vfs.o
	${TOOLCHAIN} ${TESTFLAGS} tests/test_vmm_utils.c tests/test_common.c  src/kernel/mem/vmm_util.c -o tests/test_vmm_utils.o
	${TOOLCHAIN} ${TESTFLAGS} tests/test_window.c tests/test_common.c  src/kernel/graphics/window.c -o tests/test_window.o
	${TOOLCHAIN} ${TESTFLAGS} tests/test_elf.c tests/test_common.c src/kernel/mem/vmm_util.c src/kernel/loaders/elf.c  -o tests/test_elf.o
	${TOOLCHAIN} ${TESTFLAGS} tests/test_utils.c tests/test_common.c src/kernel/loaders/elf.c src/utils/utils.c src/kernel/mem/vmm_util.c -o tests/test_utils.o
	${TOOLCHAIN} ${TESTFLAGS} tests/test_tar.c tests/test_common.c src/utils/utils.c src/kernel/loaders/elf.c src/drivers/fs/ustar.c src/kernel/mem/vmm_util.c -o test_tar.o
	./tests/test_mem.o && ./tests/test_kheap.o && ./tests/test_number_conversion.o && ./tests/test_vm.o && ./tests/test_vfs.o && ./tests/test_vmm_utils.o && ./tests/test_window.o && ./tests/test_elf.o && tests/test_utils.o && ./test_tar.o

todolist:
	@echo "List of todos and fixme in sources: "
	-@for file in $(SRC_C_FILES:Makefile=); do fgrep -H -e TODO -e FIXME $$file; done; true


