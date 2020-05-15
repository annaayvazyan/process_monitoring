#include "utils.h"





int compare_cpu_load(collected_data* a, collected_data*b)
{
    return a->cpu_load - b->cpu_load;
}

int compare_avg_load(collected_data* a, collected_data*b)
{
    return a->avg_cpu_load - b->avg_cpu_load;
}

int compare_cpu_load(collected_data* a, collected_data*b)
{
    return a->mem_load - b->mem_load;
}

int compare_pid(collected_data* a, collected_data*b)
{
    return a->task->pid - b->task->pid;
}




void insert_sorted(struct list_head* sortedLst, struct task_node* tsk, int (*compare)(task_node*, task_node*))
{
        struct list_head* current = sortedLst;
        while (compare(tsk, current) < 0)
           current = current->next;
        list_add( &tsk->mylist , &current ) ; 
}



