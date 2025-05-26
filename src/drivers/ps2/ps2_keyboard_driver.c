#include <devices.h>
#include <keyboard.h>
#include <logging.h>
#include <ps2_keyboard_driver.h>

device_t _ps2_default_driver;

void _ps2_keyboard_driver_init() {

    pretty_log(Info, "Registering ps2 keboard driver");
    _ps2_default_driver.device_id = _device_descriptor++;
    //_ps2_default_driver.drivers = NULL;
    _ps2_default_driver.init = NULL;
    _ps2_default_driver.read = &_ps2_keyboard_read;
}


void _ps2_keyboard_read(uint8_t *dst, uint8_t len) {
    // TODO: this function will be the read function for the keyboard
    //keyboard_output = dst;
}

void _ps2_keyboard_shutdown(){

}
