# The Kernel

## The Kernel status structure

The `kernel_status_t` struct, is used by the kernel code to hold execution status information about the current running kernel. It is defined in the `kernel.h` file.

It is used to get the current status of the following components:

* _Apic Timer_: It contains the calibrated initial value for the apic timer counter, and the divisor used by the timer.
* _Keyboard Status_: Used to get the current keyboard status, what keys are currently being pressed
* _Kernel uptime_: simply how long the kernel has been running
* _Paging_: It contains the status information about paging: the kernel root pml4 address. hhdm root address and the page generation. This field is used as a master copy of kernel higher half page tables to be copied when a process is created. The page generation is used by processes to sync their local pml4 with the.
* _Use x2 apic_ this field is true if the x2Apic is used.



