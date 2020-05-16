#include<linux/module.h>
#include<linux/kernel.h>
#include <linux/proc_fs.h>
#include<linux/sched/signal.h>
#include <linux/types.h>
#include <linux/ktime.h>

struct collected_data
{
    struct task_struct* task;
    u64 cpu_load;
    u64 avg_cpu_load;
    u64 mem_load;
};

struct  task_node {

     struct collected_data* data;
     struct list_head mylist;
};

int compare_cpu_load(struct task_node* a, struct task_node*b);
int compare_avg_cpu_load(struct task_node* a, struct task_node*b);
int compare_mem_load(struct task_node* a, struct task_node*b);
int compare_pid(struct task_node* a, struct task_node*b);




void insert_sorted(struct list_head* sortedLst, struct task_node* tsk, int (*compare)(struct task_node*, struct task_node*));

