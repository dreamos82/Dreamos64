DEF_FLAGS := -D USE_FRAMEBUFFER=$(USE_FRAMEBUFFER)  -D SMALL_PAGES=$(SMALL_PAGES) -D PIN_LOGO=$(PIN_LOGO)

CFLAGS := -std=gnu99 \
        -ffreestanding \
        -O0 \
        -Wall \
        -Wextra \
        -I src/include \
        -I src/include/base \
        -I src/include/userspace \
        -I src/include/kernel \
        -I src/include/kernel/mem \
        -I src/include/kernel/arch/x86_64 \
        -I src/include/kernel/arch/common \
        -I src/include/kernel/arch/common/cpu \
        -I src/include/kernel/arch/common/mem \
        -I src/include/kernel/hardware \
        -I src/include/kernel/graphics \
        -I src/include/kernel/scheduling \
        -I src/include/kernel/loaders \
        -I src/include/utils \
        -I src/include/libc \
        -I src/include/fs \
        -I src/include/drivers/fs \
        -I src/include/sys \
        -mno-red-zone \
        -mno-sse \
        -mcmodel=large \
        -fno-pie \
        -fno-pic \
        -fno-stack-protector

CFLAGS += $(DEF_FLAGS)

TESTFLAGS := -std=gnu99 \
        -I tests/include \
        -I src/include \
        -I src/include/base \
        -I src/include/kernel/mem \
        -I src/include/fs \
        -I src/include/drivers/fs \
        -I src/include/kernel \
        -I src/include/kernel/graphics \
        -I src/include/kernel/arch/x86_64 \
        -I src/include/kernel/arch/common/mem \
        -I src/include/sys \
        -I src/include/utils \
        -DSMALL_PAGES=0 \
        -D_TEST_=1

LD_FLAGS :=

PRJ_FOLDERS := src
TEST_FOLDER := tests
