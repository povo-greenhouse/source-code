/*
 * scheduler.h
 *
 *  Created on: May 10, 2025
 *      Author: riginel
 */

#ifndef SCHEDULER_H_
#define SCHEDULER_H_
#include "msp.h"
#include "stdint.h"
#include "stdbool.h"
/*
    pointer to routines
 */
typedef void (*TaskFP)(void);
/*
    struct representing a task
    fields:
    - fpointer: pointer to the routine
    - max_time: period of the task in milliseconds
    - elapsed_time: time elapsed since last time in milliseconds
    - is_active: indicates whether or not this task is active
*/
typedef struct{
    TaskFP fpointer;
    int32_t max_time;
    int32_t elapsed_time;
    bool is_active;
} STask;


#define N_PERIODIC_TASKS 10
/*
    List of the tasks that are periodically executed, used as a stack
    fields:
    - task_array: the underlying array
    - curr: index of next available slot;

*/
typedef struct {
    STask task_array[N_PERIODIC_TASKS];
    int32_t curr;
}STaskList;

// global task list
STaskList task_list;


/*
    initializes the task list
*/
void init_task_list();


/*
pushes the given task on the list
arguments:
    - task: the task to be pushed
returns:
    - -1 if there's no space
    - the index of the newly placed task in the array

 */
int push_task(STask task);
/*
removes the last task on the list
*/
int pop_task();






#define QUEUE_CAPACITY 50
/*
    scheduler's task queue, as a circular buffer
    fields:
    - arr: underlying array
    - write_index: index where to write
    - read_index: index where to read
*/
typedef struct {
    TaskFP arr[QUEUE_CAPACITY];
    int write_index;
    int read_index;
}STaskQueue;

//global task queue
STaskQueue task_queue;


// initializes the task queue
void init_task_queue();


//enqueues the given task
int enqueue_task(STask * task);
//dequeues the last task, returning it
TaskFP dequeue_task();
/*
    enum representing the state of the scheduler
    SLEEPING: no tasks scheduled
    AWAKE: currently scheduling tasks
*/
typedef enum {
    SLEEPING,
    AWAKE
} SState;

SState scheduler_state;

void scheduler();

/*
    function called when timer sets off
    subtracts specified milliseconds from tasks' elapsed field and schedules them when it gets to 0

    arguments:
    - elapsed: elapsed milliseconds since last timer interrupt
 */
void timer_interrupt(int elapsed);

void scheduler_init();



#endif /* SCHEDULER_H_ */
