#include <vfs.h>
#include <test_common.c>

int main() {
    int last = get_mountpoint_id("/home/dreamos82");
    printf("Filesystem /home/dreamos82 found at position: %d", last);
    last = get_mountpoint_id("/home/mount/dreamos82");
    printf("Filesystem /home/mount/dreamos82 found at position: %d", last);
    last = get_mountpoint_id("/usr");
    printf("Filesystem /usr found at position: %d", last);
    last = get_mountpoint_id("/");
    printf("Filesystem / found at position: %d", last);
    last = get_mountpoint_id("/usr/asd");
    printf("Filesystem /usr/asd found at position: %d", last);
}
