#include <vfs.h>
#include <assert.h>
#include <stdio.h>
#include <test_common.h>
#include <test_vfs.h>

int positions[5] = { 3, 1, 2, 0, 2 };

int main() {
    vfs_init();
    printf("Testing Virtual File System functions\n");
    printf("===============================\n\n");
    test_get_mountpoint_id();
    printf("\n");
}

void test_get_mountpoint_id() {
    printf("Testing vfs functions -\n");
    int last = vfs_get_mountpoint_id("/home/dreamos82");
    pretty_assert(last, positions[0], ==, "(test_get_mountpoint_id): Testing path /home/dreamos82");
    last = vfs_get_mountpoint_id("/home/mount/dreamos82");
    pretty_assert(last, positions[1], ==, "(test_get_mountpoint_id): Testing path /home/mount/dreamos82");
    last = vfs_get_mountpoint_id("/usr");
    pretty_assert(last, positions[2], ==, "(test_get_mountpoint_id): Testing /usr");
    last = vfs_get_mountpoint_id("/");
    pretty_assert(last, positions[3], ==, "(test_get_mountpoint_id): Testing /");
    last = vfs_get_mountpoint_id("/usr/asd");
    pretty_assert(last, positions[4], ==, "(test_get_mountpoint_id): Testing /usr/asd");
}
