#include <util.h>
#include <bitmap.h>

size_t get_number_of_pages_from_size(size_t length) {
    return align_value_to_page(length) / PAGE_SIZE_IN_BYTES;
}

size_t align_value_to_page(size_t value) {
    return ((value + PAGE_SIZE_IN_BYTES - 1) /PAGE_SIZE_IN_BYTES) * PAGE_SIZE_IN_BYTES;
}

size_t align_up(size_t value, size_t alignment) {
    return ((value + alignment - 1) / alignment) * alignment;
}

bool is_address_aligned(size_t value, size_t alignment) {
    if (value % alignment == 0) {
        return true;
    }
    return false;
}

