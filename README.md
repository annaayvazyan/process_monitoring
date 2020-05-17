# Process Monitoring

Process monitoring tool which works by creating a kernel module which adds new /proc entry holding the information about processes.

## Getting Started
First clone the repo. Afer run the following commands
```
make   # this will create monitor.ko file
sudo insmod monitor.ko  # insert monitor.ko module to your kernel
```
**Now you** have monitor module inserted. Inserting monitor.ko module will create /proc/monitor entry.
To ensure you can do
```
lc /proc
```
![image](https://github.com/annaayvazyan/process_monitoring/blob/master/sceenshots/ls_proc.png)

To see process info you can cat /proc/monitor entry 
```
cat /proc/monitor
```
![image](https://github.com/annaayvazyan/process_monitoring/blob/master/sceenshots/cat_proc_monitor.png)
This information will be discibed in the next sections.

To run this programm as top command (to watch updated informations every seconds) run
```
./monitor
```
## Code description
As it is mentioned in the project's description a kernel module was written with name **monitor**. To create a kernel module first were written init and exit functions for module.
```c
module_init(functn_init);
module_exit(functn_cleanup);
```
This module creates new proc entry with name **monitor**. Catting /proc/monitor file gives you information about running processes.
All information is taken from kernel's task list (actual list of existing processes and threads which holds kernel in memory).
To obtain task_list have used for_each_process macro which is defined in kernel 'linux/sched.h' file.
```c
for_each_process(curr) {
    // curr is of type task_struct which is the type of processes and tasks in linux
}
```
In this loop the information about each process is collected using  
```c
struct collected_data * collect_info_for_process (struct task_struct* tsk)
```
This function will collect data with the help of other fucntions such as "compute_mem_usage" or "compute_exet_time".
Information is collected and kept in "struct collected_data". After obtaining the info for current process, collected_data object is inserted into the linked_list(kernel data structure). The item is inserted using "insert_sorted" function, which inserts an element in the list by finding the right place of it (with descending order)
```c
void insert_sorted(struct list_head* sortedLst, struct task_node* tsk, int (*compare)(struct task_node*, struct task_node*));
```
The comparator function takes two arguments and contains logic to decide their relative order in sorted output. in "utils.c" function there are several comparator functions defined which can be used for sorting results. You can also add you own method.
```c
int compare_cpu_load(struct task_node* a, struct task_node*b);
int compare_avg_cpu_load(struct task_node* a, struct task_node*b);
int compare_mem_load(struct task_node* a, struct task_node*b);
int compare_pid(struct task_node* a, struct task_node*b);
```
Then the information is printed by iterating over the sorted linked list.
```c
    len  += sprintf(buf, "\n    PID   USR  PR SPR   CPU*100    AVGCPU    MEM*100     TIME+            NAME\n");        

         len  += sprintf(buf+len, "%7lld %5lld %3lld %3lld %9lld %9lld %10lld %9s %15s\n" 
                                                                          , pid
                                                                          , user
                                                                          , prio
                                                                          , sprio
                                                                          , cpu_load
                                                                          , avg_cpu_load
                                                                          , mem_load
                                                                          , exec_time
                                                                          , comm

                         );
```
- PID - is the PID of the process, is taken from task_struct->pid
- USR - is the user ID of the process owner, is taken form task_struct->cred->uid
- PR - is the priority of the process, is taken from task_stract->prio
- SPR - is the static priority of the process, is taken from task_stract->static_prio
- CPU* 100 - is the CPU load/usage of the process in percents * 100 (to not loose info), is calculated in compute_cpu_load function.
    ```c
        struct cpu_load* compute_cpu_load(struct time_info* time_info_s, struct time_info* time_info_e)

        struct time_info {
            u64 utime; //time in nanoseconds task spent in user space
            u64 stime; //time in nanoseconds task spent in kernel space
            u64 cutime; //time in nanoseconds task's children spent in user space
            u64 cstime; //time in nanoseconds task's children spent in kernel space
            u64 start_time; // time when the task created
            u64 uptime; // system uptime (used to calculate cpu_load)
        };
    ```
    Function takes start and end time_info's (at first time_info_s is taken, then after sleeping a while th time_info_e is taken). With this 2 infos cpu_load can be calculated.
- AVGCPU - is the average CPU load/usage of the process in percents * 100 (to not loose info), is calculated in compute_avg_cpu_load function.
    ```c
        struct cpu_load* compute_average_cpu_load(struct time_info* tm_info)
    ```
    Fucntion takes only one time_info, calculates avg_cpu_load with formula 
    ```c
        avg_load = total_time*100/(uptime - start_time)
    ```
- MEM* 100 - is the MEM load/usage of the process in percents * 100 (to not loose info), is calculated in compute_mem_usage function.
    ```c
        void compute_mem_usage(struct collected_data * col_data)
    ```
    Fucntion computes with this formula: mem_usage = (task_struct->mm->total_vm)* 100 / RAM_SIZE
- TIME+ - is The execution time of the task since it created, is calculated in compute_exec_time function.
    ```c
        void compute_exec_time(struct collected_data * col_data)
    ```
    Fucntion computes using uptime of the system and start_time of the task, exec_time = uptime -start_time
- NAME - is the command name which created the process, is taken form task_struct->comm

## Used Data structures
Will add the descriptions
- doubly linked list (kernel)
- hash table (kernel)


## Demo

![](https://github.com/annaayvazyan/process_monitoring/blob/master/sceenshots/monitor_demo.gif)

## Further steps
Add possibility to pass arguments to the module
- -p - process id to be tracked
- -s - sorting type
- etc

## Authors
Anna Ayvazyan

## Inspiration
Is a course project
