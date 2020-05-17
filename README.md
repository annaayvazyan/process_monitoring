# Process Monitoring

Write process monitoring tool by writing a kernel module which will add new /proc entry to hold information about processes.

## Getting Started
First clone the repo. Afer run the following commands
```
make   \\this will create monitor.ko file
sudo insmod monitor.ko  \\ insert monitor.ko module to your kernel
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
```
module_init(functn_init);
module_exit(functn_cleanup);
```
This module creates new proc entry with name **monitor**. Catting /proc/monitor file gives you information about running processes.
All information is taken from kernel's task list (actual list of existing processes and threads which holds kernel in memory).
To obtain task_list have used for_each_process macro which is defined in kernel 'linux/sched.h' file.
```
for_each_process(curr) {
    // curr is of type task_struct which is the type of processes and tasks in linux
}
```
In this loop the information about each process is collected using  
```
struct collected_data * collect_info_for_process (struct task_struct* tsk)
```
This function will collect data with the help of other fucntions such as "compute_mem_usage" or "compute_exet_time".
Information is collected and kept in "struct collected_data". After obtaining the info for current process, collected_data object is inserted into the linked_list(kernel data structure). The item is inserted using "insert_sorted" function, which inserts an element in the list by finding the right place of it (with descending order)
```
void insert_sorted(struct list_head* sortedLst, struct task_node* tsk, int (*compare)(struct task_node*, struct task_node*));
```
The comparator function takes two arguments and contains logic to decide their relative order in sorted output. in "utils.c" function there are several comparator functions defined which can be used for sorting results. You can also add you own method.
```
int compare_cpu_load(struct task_node* a, struct task_node*b);
int compare_avg_cpu_load(struct task_node* a, struct task_node*b);
int compare_mem_load(struct task_node* a, struct task_node*b);
int compare_pid(struct task_node* a, struct task_node*b);
```
Then the information is printed by iterating over the sorted linked list.
```c
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


Give an example
And coding style tests
Explain what these tests test and why

Give an example
Deployment
Add additional notes about how to deploy this on a live system

Built With
Dropwizard - The web framework used
Maven - Dependency Management
ROME - Used to generate RSS Feeds
Contributing
Please read CONTRIBUTING.md for details on our code of conduct, and the process for submitting pull requests to us.

Versioning
We use SemVer for versioning. For the versions available, see the tags on this repository.

Authors
Billie Thompson - Initial work - PurpleBooth
See also the list of contributors who participated in this project.

License
This project is licensed under the MIT License - see the LICENSE.md file for details

Acknowledgments
Hat tip to anyone whose code was used
Inspiration
etc
