#include <vfs.h>
#include <assert.h>
#include <stdio.h>
#include <test_common.h>
#include <test_vfs.h>

int positions[5] = { 3, 1, 2, 0, 2 };

int main() {
    vfs_init();    
    test_get_mountpoint_id();
}

void test_get_mountpoint_id() {
    printf("Testing vfs functions -\n");
    int last = get_mountpoint_id("/home/dreamos82");    
    printf("\t [test_vfs](test_get_mountpoint_id): Testing path /home/dreamos82: found at position: %d should be: %d\n", last, positions[0]);
    assert(last == positions[0]);
    last = get_mountpoint_id("/home/mount/dreamos82");
    printf("\t [test_vfs](test_get_mountpoint_id): Testing /home/mount/dreamos82: found at position: %d should be %d\n", last, positions[1]);
    assert(last == positions[1]);
    last = get_mountpoint_id("/usr");
    printf("\t [test_vfs](test_get_mountpoint_id): Testing /usr: found at position: %d should be %d\n", last, positions[2]);
    assert(last == positions[2]);
    last = get_mountpoint_id("/");
    printf("\t [test_vfs](test_get_mountpoint_id): Testing /: found at position: %d should be: %d\n", last, positions[3]);
    assert(last == positions[3]);
    last = get_mountpoint_id("/usr/asd");
    printf("\t [test_vfs](test_get_mountpoint_id): Testing /usr/asd: found at position: %d found at: %d\n", last, positions[4]);
    assert(last == positions[4]);
}
