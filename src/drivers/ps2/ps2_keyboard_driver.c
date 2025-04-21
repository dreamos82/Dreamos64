#include <devices.h>
#include <logging.h>
#include <ps2_keyboard_driver.h>

device_t _ps2_default_driver;

void _ps2_keyboard_driver_init() {

    pretty_log(Info, "Registering ps2 keboard driver");
    _ps2_default_driver.device_id = _device_descriptor++;
    _ps2_default_driver.drivers = NULL;
}


void _ps2_keyboard_read() {
    // TODO: this function will be the read function for the keyboard
}
