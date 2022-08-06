#include <task.h>
#include <kheap.h>
#include <string.h>
#include <logging.h>


task_t* create_task(char *name, void (*_entry_point)(void *), void *args) {
    task_t* new_task = (task_t*) kmalloc(sizeof(task_t));
    strcpy(new_task->task_name, name);
    loglinef(Verbose, "Task created with name: %s", new_task->task_name);
    thread_t* thread = create_thread(name, _entry_point, args);
    new_task->threads = thread;
    new_task->parent = NULL;
    return new_task;
}
