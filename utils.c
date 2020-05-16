#include "utils.h"





int compare_cpu_load(struct task_node* a, struct task_node*b)
{
    printk( KERN_DEBUG "compare_cpu_load:\n" );
    if (a == 0 || b == 0 || a->data == 0 || b->data == 0)
    return 0;    
    return a->data->cpu_load - b->data->cpu_load;
}

int compare_avg_cpu_load(struct task_node* a, struct task_node*b)
{
    return a->data->avg_cpu_load - b->data->avg_cpu_load;
}

int compare_mem_load(struct task_node* a, struct task_node*b)
{
    return a->data->mem_load - b->data->mem_load;
}

int compare_pid(struct task_node* a, struct task_node*b)
{
    return a->data->task->pid - b->data->task->pid;
}


void insert_sorted(struct list_head* sortedLst, struct task_node* tsk, int (*compare)(struct task_node*, struct task_node*))
{
    printk( KERN_DEBUG "insert_sorted: start\n" );    

        struct list_head* curr;
        curr = sortedLst;

        struct task_node  *curr_data  = NULL ; 
        curr_data = list_entry ( curr, struct task_node, mylist); 
        //struct task_node  *tsk_data  = NULL ; 
        //tsk_data = list_entry ( tsk, struct task_node, mylist);   
    printk( KERN_DEBUG "insert_sorted: calling comp\n" );    

        while (compare(tsk, curr_data) < 0 && curr != sortedLst )
        {
           curr = curr->next;
           curr_data = list_entry ( curr, struct task_node, mylist);
    printk( KERN_DEBUG "insert_sorted: in while loop\n" );    

        }
    printk( KERN_DEBUG "insert_sorted: adding in list\n" );    
        list_add( &tsk->mylist, curr ) ; 
}



