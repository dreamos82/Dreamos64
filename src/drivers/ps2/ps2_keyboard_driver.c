#include <devices.h>
#include <keyboard.h>
#include <kheap.h>
#include <logging.h>
#include <ps2_keyboard_driver.h>

device_t _ps2_default_driver;
pending_operation_t *_ps2_op_head;
pending_operation_t *_ps2_op_tail;

void _ps2_keyboard_driver_init() {

    pretty_log(Info, "Registering ps2 keboard driver");
    _ps2_default_driver.device_id = _device_descriptor++;
    //_ps2_default_driver.drivers = NULL;
    _ps2_default_driver.init = NULL;
    _ps2_default_driver.read = &_ps2_keyboard_read;
    _ps2_op_head = NULL;
    _ps2_op_tail = NULL;
}


void _ps2_keyboard_shutdown(){

}

void _ps2_keyboard_read(uint8_t *dst, uint8_t len) {

}


pending_operation_t* _ps2_get_next_operation() {
    return _ps2_op_head;
}

void _ps2_keyboard_add_operation(pending_operation_t *new_op) {
    if ( _ps2_op_head == NULL) {
        pretty_logf(Verbose, "New op: nbytes: %d - read: %d", new_op->nbytes, new_op->read);
        _ps2_op_head = new_op;
        _ps2_op_tail = _ps2_op_head;
    } else {
        _ps2_op_tail->next = new_op;
        _ps2_op_tail = new_op;
    }
}

void _ps2_keyboard_del_operation(pending_operation_t *op_to_delete) {
    pending_operation_t *elmement_to_delete = _ps2_op_head;
    _ps2_op_head = _ps2_op_head->next;
    kfree(elmement_to_delete);
}
