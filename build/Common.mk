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

C_DEBUG_FLAGS := -g \
				-DDEBUG=1
				
PRJ_FOLDERS := src
TEST_FOLDER := tests
