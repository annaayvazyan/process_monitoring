#include<linux/module.h>
#include<linux/kernel.h>
#include <linux/proc_fs.h>
#include<linux/sched/signal.h>
#include <linux/types.h>
#include <linux/ktime.h>

int compare_cpu_load(collected_data* a, collected_data*b);
int compare_avg_load(collected_data* a, collected_data*b);
int compare_cpu_load(collected_data* a, collected_data*b);
int compare_pid(collected_data* a, collected_data*b);

struct collected_data
{
    struct task_struct* task;
    u64 cpu_load;
    u64 avg_cpu_load;
    u64 mem_load;
}



struct  task_node {

     struct collected_data* data;
     struct list_head mylist;
};


void insert_sorted(struct list_head* sortedLst, struct task_node* tsk, int (*compare)(task_node*, task_node*));

