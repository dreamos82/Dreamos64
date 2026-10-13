#ifndef _LIST_H_
#define _LIST_H_

#define CONTAINER_OF(ptr, type, member) ((type *) ptr - offsetof(type, member))

struct list_item_t{
    struct driver *next;
};


typedef struct list_item_t list_item_t;

#endif
