#include <kernel/serial.h>
#include <io.h>

int init_serial(int port){

   outportb(port + 1, 0x00);    // Disable all interrupts
   outportb(port + 3, 0x80);    // Enable DLAB (set baud rate divisor)
   outportb(port + 0, 0x03);    // Set divisor to 3 (lo byte) 38400 baud
   outportb(port + 1, 0x00);    //                  (hi byte)
   outportb(port + 3, 0x03);    // 8 bits, no parity, one stop bit
   outportb(port + 2, 0xC7);    // Enable FIFO, clear them, with 14-byte threshold
   outportb(port + 4, 0x0B);    // IRQs enabled, RTS/DSR set
   outportb(port + 4, 0x1E);    // Set in loopback mode, test the serial chip
   outportb(port + 0, 0xAE);    // Test serial chip (send byte 0xAE and check if serial returns same byte)

   // Check if serial is faulty (i.e: not same byte as sent)
   if(inportb(port + 0) != 0xAE) {
      return 1;
   }

   // If serial is not faulty set it in normal operation mode
   // (not-loopback with IRQs enabled and OUT#1 and OUT#2 bits enabled)
   outportb(port + 4, 0x0F);
   return 0;
}
