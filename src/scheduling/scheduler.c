/*
 * scheduler.c
 *
 *  Created on: May 9, 2025
 *      Author: riginel
 */

#include "scheduling/scheduler.h"
#include "scheduling/timer.h"

void init_task_list() { task_list.curr = 0; }

int push_task(STask task) {

    if (task_list.curr < N_PERIODIC_TASKS) {
        task_list.task_array[task_list.curr] = task;

        return task_list.curr++;
    }

    return 0;
}

int pop_task() {
    if (task_list.curr > 0) {
        task_list.curr -= 1;
        return 1;
    }
    return 0;
}
int enable_task_at(uint32_t index){
    if(task_list.curr <=index){
        return -1;
    }
    disable_timer_interrupt();
    task_list.task_array[index].is_active = true;
    enable_timer_interrupt();
    return 0;
}

int disable_task_at(uint32_t index){
    if(task_list.curr <=index){
        return -1;
    }
    disable_timer_interrupt();
    task_list.task_array[index].is_active = false;
    enable_timer_interrupt();
    return 0;
}

void init_task_queue() {

    task_queue.write_index = 0;
    task_queue.read_index = 0;
}
// enqueues the given task
int enqueue_task(STask *task) {

    if ((task_queue.write_index + 1) % QUEUE_CAPACITY ==
        task_queue.read_index) {
        return 0;
    }
    task_queue.arr[task_queue.write_index] = task->fpointer;
    task_queue.write_index = (task_queue.write_index + 1) % QUEUE_CAPACITY;
    return 1;
}
// dequeues the last task, returning it
TaskFP dequeue_task() {
    if (task_queue.read_index == task_queue.write_index) {
        return 0;
    }
    TaskFP ret = task_queue.arr[task_queue.read_index];
    task_queue.read_index = (task_queue.read_index + 1) % QUEUE_CAPACITY;
    return ret;
}

void scheduler() {
    disable_timer_interrupt();
    TaskFP next = dequeue_task();
    enable_timer_interrupt();
    while (next != 0) {
        next();
        disable_timer_interrupt();
        next = dequeue_task();
        enable_timer_interrupt();
    }
    scheduler_state = SLEEPING;
}

void timer_interrupt(int elapsed) {
    int scheduled_at_least_once = 0;
    disable_timer_interrupt();
    int i;

    for (i = 0; i < task_list.curr; i++) {
        STask *t = &(task_list.task_array[i]);
        if (!t->is_active) {
            continue;
        }
        t->elapsed_time -= elapsed;
        if (t->elapsed_time <= 0) {
            t->elapsed_time = t->max_time;
            // time to schedule
            enqueue_task(t);
            scheduled_at_least_once = 1;
        }
    }
    enable_timer_interrupt();
    if (scheduled_at_least_once && scheduler_state == SLEEPING) {
        scheduler_state = AWAKE;

    }
}

void scheduler_init() {
    init_task_list();
    init_task_queue();
    scheduler_state = SLEEPING;
}
