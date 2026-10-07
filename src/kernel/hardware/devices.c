#include <devices.h>
#include <ps2_keyboard_driver.h>

unsigned char _device_descriptor;

driver_item_t *_kernel_registered_drivers;

void init_known_drivers() {
    _device_descriptor = 0;
    _ps2_keyboard_driver_init();
    _kernel_registered_drivers = &(_ps2_default_driver.header);
    //TODO: initialize list and add ps2 keyboard driver
}

device_t* get_device(unsigned char _device_item) {    
    //return _device_item - sizeof(driver_item_t);
}
