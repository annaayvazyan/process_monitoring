#include<linux/module.h>
#include<linux/kernel.h>
#include <linux/proc_fs.h>
#include<linux/sched/signal.h>
#include <linux/types.h>
#include <linux/ktime.h>
#include <linux/mm.h>

//extern struct ecpu_load;

struct cpu_load
{
   u64 ucpu_load;
   u64 scpu_load; 
};

// this struct is used to keep all data that user will see
struct collected_data
{
    struct task_struct* task;
    struct cpu_load* a_cpu_load;
    struct cpu_load* avg_cpu_load;
    u64 mem_load;
};

struct time_info {
    u64 utime; //time in nanoseconds task spent in user space
    u64 stime; //time in nanoseconds task spent in kernel space
    u64 cutime; //time in nanoseconds task's children spent in user space
    u64 cstime; //time in nanoseconds task's children spent in kernel space
    u64 start_time; // time when the task created
    u64 uptime; // system uptime (used to calculate cpu_load)
};

struct  task_node {

     struct collected_data* data;
     struct list_head mylist;
};

int compare_cpu_load(struct task_node* a, struct task_node*b);
int compare_avg_cpu_load(struct task_node* a, struct task_node*b);
int compare_mem_load(struct task_node* a, struct task_node*b);
int compare_pid(struct task_node* a, struct task_node*b);

void set_time_info(struct time_info* tm_info, struct task_struct* tsk);

void insert_sorted(struct list_head* sortedLst, struct task_node* tsk, int (*compare)(struct task_node*, struct task_node*));

char* cpu_load_to_string(struct cpu_load*c);
