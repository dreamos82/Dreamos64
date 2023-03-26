#include <util.h>
#include <bitmap.h>

size_t get_number_of_pages_from_size(size_t length) {
    size_t number_pages = length / PAGE_SIZE_IN_BYTES;
    if (length%PAGE_SIZE_IN_BYTES != 0) {
        number_pages++;
    }
    return number_pages;
}
