#ifndef _PS2_H_
#define _PS2_H_

#include <stdint.h>

#define PS2_TARGET_DEVICE   1
#define PS2_TARGET_CONTROLLER   2

#define PS2_DATA_REGISTER   0x60    // This is the same as the keyboard data port
#define PS2_STATUS_REGISTER 0x64
#define PS2_COMMAND_REGISTER PS2_STATUS_REGISTER

#define PS2_READ_CONFIGURATION_COMMAND 0x20

#endif
