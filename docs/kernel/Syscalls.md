# Syscalls

The syscalls are called using the interrupt vector `0x80`. Arguments depend on the syscalls being called. Below the list of available syscalls.

This list is still draft, so it can change at any time.

The syscall number is placed in the register `rdi`

# Syscalls List

## 0x00 TEST

The first syscall is reserved for test purpose, and it should be never used.

## 0x01

It prints always the string: `Hello from user world (through a syscall...)`

## 0x02 Read from keyboard

Read from keyboard 

Arguments: 

* `rsi`: file descriptor 
* `rdx`: buffer pointer
* `rcx`: number of charaters to read

Return value: 
* `rax`: number of characters read if success, -1 otherwise.

## 0x03 Print buffer on screen

Print on screen content of buffer

* `rsi`: buffer pointer
* `rdx`: buffer size
* `rcx`: [optional] X coord of the print
* `r8`: [optional] Y coord of screen

If `X` and `Y` are both 0, it will print on the next line available on the screen. Currently, for debug purposes, it also print an extra line, with a constant message.

