# Syscalls

The syscalls are called using the interrupt vector `0x80`. Arguments depend on the syscalls being called. Below the list of available syscalls.

This list is still draft, so it can change at any time.

# Syscalls List

## 0x00 TEST

The first syscall is reserved for test purpose, and it should be never used.

## 0x01

It prints always the string: `Hello from user world (through a syscall...)`

## 0x02 Read from keyboard

Read from keyboard 

Arguments: 

* `rsi`: buffer pointer
* `rdx`: buffer size

## 0x03 Print buffer on screen

Print on screen content of buffer

* `rsi`: buffer pointer
* `rdx`: buffer size

