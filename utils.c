#include "utils.h"

#include<linux/module.h>
#include <linux/proc_fs.h>
#include<linux/sched/signal.h>
#include <linux/types.h>
#include <linux/ktime.h>
#include <asm/param.h>
#include <linux/sched/cputime.h>
#include <linux/types.h>
#include <linux/errno.h>
#include <linux/time.h>
#include <linux/kernel_stat.h>
#include <linux/string.h>
#include <linux/mman.h>
#include <linux/sched/mm.h>
#include <linux/sched/cputime.h>
#include <linux/io.h>
#include <linux/mm.h>
#include <linux/signal.h>
#include <linux/highmem.h>
#include <linux/file.h>
#include <linux/fdtable.h>
#include <linux/times.h>
#include <linux/cpuset.h>
#include <linux/rcupdate.h>
#include <linux/tracehook.h>
#include <linux/fs_struct.h>
#include <linux/sched.h> //Needed for the for_each_process() macro
#include <linux/jiffies.h> //Needed to manage the time
#include <asm/uaccess.h> //Needed to use copy_to_user
#include <linux/tty.h>
#include <linux/linkage.h>
#include <linux/init.h>		/* Needed for the macros */
#include <linux/kthread.h>
#include <linux/slab.h>
#include <linux/semaphore.h>
#include <linux/kernel.h>
#include <linux/delay.h>

void set_time_info(struct time_info* tm_info, struct task_struct* tsk)
{
    tm_info->utime = tsk->utime;
    tm_info->stime = tsk->stime;
    tm_info->start_time = tsk->start_time;
    tm_info->cutime = 0;
    tm_info->cstime = 0;
    if (tsk->signal)
    {
        tm_info->cutime = tsk->signal->cutime;
        tm_info->cstime = tsk->signal->cstime;
    }
}


int compare_cpu_load(struct task_node* a, struct task_node*b)
{

    printk( KERN_DEBUG "compare_cpu_load:\n" );
    /*
    if (a == 0 )
       printk( KERN_DEBUG "a is null\n");
    if  (b == 0)
       printk( KERN_DEBUG "b is null\n");
    if ( a->data == 0)
       printk( KERN_DEBUG "a->data is null\n");
    if ( b->data == 0)
       printk( KERN_DEBUG "b->data is null\n");
    if (  a->data->a_cpu_load == 0)
       printk( KERN_DEBUG "a->data->a_cpu_load is null\n");
    if ( b->data->a_cpu_load == 0)
       printk( KERN_DEBUG "b->data->a_cpu_load is null\n");
    if ( a->data->task == 0)
       printk( KERN_DEBUG "a->data->task is null\n");
    if ( b->data->task == 0)
       printk( KERN_DEBUG "b->data->task is null\n");
    */


     if (   a == 0 || b == 0 
         || a->data == 0 || b->data == 0 
         || a->data->a_cpu_load == 0 || b->data->a_cpu_load == 0 
         || a->data->task == 0 || b->data->task == 0 ) {
         printk( KERN_DEBUG "compare_cpu_load : returning 0\n" );
        return 0; 
     }

    printk( KERN_DEBUG "compare_cpu_load: apid=%d bpid=%d\n", a->data->task->pid, b->data->task->pid );
    printk( KERN_DEBUG "compare_cpu_load: a=%d b=%d\n", a->data->a_cpu_load->ucpu_load + a->data->a_cpu_load->scpu_load, b->data->a_cpu_load->ucpu_load + b->data->a_cpu_load->scpu_load );
    return (a->data->a_cpu_load->ucpu_load + a->data->a_cpu_load->scpu_load) 
         - (b->data->a_cpu_load->ucpu_load + b->data->a_cpu_load->scpu_load);
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
        curr = sortedLst->next;

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
        list_add( &tsk->mylist, curr->prev ) ; 
}

char* cpu_load_to_string(struct cpu_load*c)
{
   char *buff = kmalloc(1024, GFP_NOWAIT);
   int len = 0;
   len = sprintf(buff+len, "%d %d", c->ucpu_load, c->scpu_load);   
   return buff;
}

char* exec_time_to_string(u64 exec_time) 
{
    char *buff = kmalloc(1024, GFP_NOWAIT);    
    u64 minutes = exec_time / 60;
    u64 hours =  minutes / 60;
    u64 secs = exec_time % 60;
    minutes = minutes % 60;
    sprintf(buff, "%3d:%02d:%02d", hours, minutes, secs);
    return buff;
}

