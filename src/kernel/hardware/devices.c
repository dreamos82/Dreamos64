#include <devices.h>

unsigned char _device_descriptor;

driver_item_t *_kernel_registered_drivers;

void init_known_drivers() {

    _device_descriptor = 0;


    //TODO: initialize list and add ps2 keyboard driver

}
