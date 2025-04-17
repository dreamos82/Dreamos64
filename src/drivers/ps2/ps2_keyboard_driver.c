#include <logging.h>
#include <devices.h>
#include <ps2_keyboard_driver.h>

device_t _ps2_default_driver;

void _ps2_keyboard_driver_init() {

    pretty_log(Info, "Registering ps2 keboard driver");
    _ps2_default_driver.device_id = _device_descriptor++;

}
