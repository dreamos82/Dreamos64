#include <task.h>
#include <kheap.h>
#include <string.h>
#include <logging.h>


task_t* create_task(char *name, void (*_entry_point)(void *), void *args) {
    task_t* new_task = (task_t*) kmalloc(sizeof(task_t));
    strcpy(new_task->task_name, name);
    new_task->parent = NULL;
    new_task->task_id = next_task_id++;
    loglinef(Verbose, "Task created with name: %s - Task id: %d", new_task->task_name, new_task->task_id);
    thread_t* thread = create_thread(name, _entry_point, args, new_task);
    new_task->threads = thread;
    scheduler_add_task(new_task);
    return new_task;
}

bool add_thread_to_task(thread_t* thread) {

}
