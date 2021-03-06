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
#include "utils.h"



#define BUFSIZE  1000
#define MAXPROCCOUNT 2000
static struct proc_dir_entry *ent;
DECLARE_HASHTABLE(task_hash, 10);  //is used to keep process info for some calculations (e.g. cpu_load)
const char * entry_name = "monitor";


/////////////////////////////////////////////////////////
// options that in further should be passed as argument//
// //////////////////////////////////////////////////////
int (*ARG_SORTING_COMP)(struct task_node*, struct task_node*) = compare_cpu_load; // sorting function, default is cpu_load, further should be user specified 
int ARG_PROCES_COUNT_TO_BE_TRACKED = PRINTED_PROC_COUNT; // how many processes should be printed
pid_t ARG_PID = 0; // pid shoud be tracked




// this function will be used to pass arguments from user
static ssize_t mywrite(struct file *file, const char __user *ubuf, size_t count, loff_t *ppos) 
{


	printk( KERN_DEBUG "write handler\n");
	int num,c,i,m;
	char *buf = kmalloc(BUFSIZE*50, GFP_KERNEL);

	if(*ppos > 0 || count > BUFSIZE)
		return -EFAULT;
	if(raw_copy_from_user(buf,ubuf,count))
		return -EFAULT;
	num = sscanf(buf,"%d %d",&i,&m);
	if(num != 2)
		return -EFAULT;
	//irq = i; 
	//mode = m;
	c = strlen(buf);
	*ppos = c;



	return c;
}


void collect_process_info(void)
{
    printk( KERN_DEBUG "collect_process_info : ");            
    struct task_struct* curr;
	for_each_process(curr) {
		printk( KERN_DEBUG "collect_process_info : pid = %10d, command = %15s\n, utime=%lld, stime=%lld", curr->pid, curr->comm, curr->utime, curr->stime);        
        struct h_struct *task = kmalloc(sizeof(struct h_struct), GFP_KERNEL);
        if (!task) {
            printk( KERN_DEBUG "h_struct *task = kmalloc(sizeof(struct h_struct), GFP_KERNEL)");
            return;            
        }
            
        task->tm_info = kmalloc(sizeof(struct time_info), GFP_KERNEL);
        if (!task->tm_info) {
            printk( KERN_DEBUG "task->tm_info = kmalloc(sizeof(struct time_info), GFP_KERNEL)");
            return;  
        }
        set_time_info(task->tm_info, curr);
        task->tm_info->uptime = ktime_get_coarse_boottime(); // keep for each process to be accurate in cpu_load calculation
        task->state = 1; // alive
        task->pid = curr->pid;
        hash_add(task_hash, &task->node, task->pid);
    }
}

struct cpu_load* compute_cpu_load(struct time_info* time_info_s, struct time_info* time_info_e)
{
    struct cpu_load* a_cpu_load = kmalloc(sizeof(struct cpu_load), GFP_KERNEL);
    if (!a_cpu_load) {
        printk( KERN_DEBUG "struct cpu_load* a_cpu_load = kmalloc(sizeof(struct cpu_load), GFP_KERNEL)");
        return a_cpu_load;
    }
    a_cpu_load->ucpu_load = 0;
    a_cpu_load->scpu_load = 0;

    u64 interval =  time_info_e->uptime - time_info_s->uptime;
	printk( KERN_DEBUG "compute_cpu_load : interval=%lld\n", interval);

    if (interval == 0)
        return a_cpu_load;

    u64 total_utime_s = time_info_s->utime + time_info_s->cutime;
    u64 total_utime_e = time_info_e->utime + time_info_e->cutime;
	printk( KERN_DEBUG "compute_cpu_load : total_utime_s=%lld total_utime_e=%lld\n", total_utime_s, total_utime_e);    
    u64 u_cpu_usage = (total_utime_e - total_utime_s)* 10000/(interval); // actual cpu usage in user space

    u64 total_stime_s = time_info_s->stime + time_info_s->cstime;
    u64 total_stime_e = time_info_e->stime + time_info_e->cstime;
	printk( KERN_DEBUG "compute_cpu_load : total_stime_s=%lld total_stime_e=%lld\n", total_stime_s, total_stime_e);        
    u64 s_cpu_usage = (total_stime_e - total_stime_s)* 10000/(interval); // actual cpu usage in user space
    a_cpu_load->ucpu_load = u_cpu_usage;
    a_cpu_load->scpu_load = s_cpu_usage;
    return a_cpu_load;
}

struct cpu_load* compute_average_cpu_load(struct time_info* tm_info)
{
  
    struct cpu_load* avg_cpu_load = kmalloc(sizeof(struct cpu_load), GFP_KERNEL);
    if (!avg_cpu_load) {
        printk( KERN_DEBUG "struct cpu_load* avg_cpu_load = kmalloc(sizeof(struct cpu_load), GFP_KERNEL)");
        return avg_cpu_load;
    }
    avg_cpu_load->ucpu_load = 0;
    avg_cpu_load->scpu_load = 0;

    u64 interval = tm_info->uptime - tm_info->start_time;
    if (interval == 0)
        return avg_cpu_load;

    u64 total_utime = tm_info->utime + tm_info->cutime;
    u64 total_stime = tm_info->stime + tm_info->cstime;
    u64 u_cpu_usage = (total_utime)* 10000/(interval); // actual cpu usage in user space
    u64 s_cpu_usage = (total_stime)* 10000/(interval); // actual cpu usage in user space
    avg_cpu_load->ucpu_load = u_cpu_usage;
    avg_cpu_load->scpu_load = s_cpu_usage;
    return avg_cpu_load;
}



void compute_cpu_loads(struct collected_data * col_data)
{
	printk( KERN_DEBUG "compute_cpu_loads :\n");
    struct h_struct *entry;
    struct time_info time_info_s;
    struct time_info time_info_e;

	printk( KERN_DEBUG "compute_cpu_loads : hash_for_each_possible\n");
    hash_for_each_possible(task_hash, entry, node, col_data->task->pid) {
        if (entry->pid == col_data->task->pid) {
	        printk( KERN_DEBUG "compute_cpu_loads : hash_for_each_possible loop pid=%lld, utime=%lld, stime=%lld\n", entry->pid, entry->tm_info->utime, entry->tm_info->stime);
            time_info_s = *(entry->tm_info);
            break;
        }
    }
	printk( KERN_DEBUG "compute_cpu_loads : calling set_time_info for e\n");
    set_time_info(&time_info_e, col_data->task);
    time_info_e.uptime = ktime_get_coarse_boottime();
    
	printk( KERN_DEBUG "compute_cpu_loads : calling compute_cpu_load\n");    
    col_data->a_cpu_load = compute_cpu_load(&time_info_s, &time_info_e);
	printk( KERN_DEBUG "compute_cpu_loads : %s calling compute_average_cpu_load\n", cpu_load_to_string(col_data->a_cpu_load));    
    col_data->avg_cpu_load = compute_average_cpu_load(&time_info_e);

}

void compute_mem_usage(struct collected_data * col_data)
{
	printk( KERN_DEBUG "compute_mem_usage :\n");
    u64 mem_used = 0;
    if (col_data->task->mm)
        mem_used = col_data->task->mm->total_vm;

    u64 mem_usage = mem_used*10000 / TOTAL_MEMORY;
    col_data->mem_load = mem_usage;
}

void compute_exec_time(struct collected_data * col_data)
{
    u64 stime = col_data->task->start_time;
    u64 uptime = ktime_get_coarse_boottime();
    col_data->exec_time = ktime_divns(uptime - stime, NSEC_PER_SEC); // converts nsecs to secs 
}

struct collected_data * collect_info_for_process (struct task_struct* tsk)
{
  
	printk( KERN_DEBUG "collect_info_for_process :\n");
    struct collected_data *coll = kmalloc(sizeof(struct collected_data), GFP_KERNEL);
    if (!coll) {
        printk( KERN_DEBUG "struct collected_data *coll = kmalloc(sizeof(struct collected_data), GFP_KERNEL)");
        return 0;
    }

	printk( KERN_DEBUG "collect_info_for_process : calling compute_cpu_loads\n");
    coll->task = tsk;
    compute_cpu_loads(coll);
    compute_mem_usage(coll);
    compute_exec_time(coll);

    return coll; 
}


static ssize_t myread(struct file *file, char __user *ubuf, size_t count, loff_t *ppos) 
{

	printk( KERN_DEBUG "myread :\n");
	char *buf = kmalloc(BUFSIZE*100, GFP_KERNEL);
	if(!buf || *ppos > 0 || count < BUFSIZE)
		return 0;
	
    int len=0;

    printk( KERN_DEBUG "myread: calling collect_process_info\n" );
    // collect data of processes once to use it in further calculations( e.g. cpu_load)
    collect_process_info();

    
    
    msleep(SLEEP); // sleep a second to be able to compute cpu_laod




    printk( KERN_DEBUG "myread: initing sorted_process_list\n" );    
    // this list is used to keep sorted processes using comp function
    LIST_HEAD(sorted_process_list) ;

    printk( KERN_DEBUG "myread: starting main loop\n" );    
	struct task_struct *curr;
    int i = 0;    
    // main loop
	for_each_process(curr) {
    
        printk( KERN_DEBUG "myread: in loop i=%4d, pid=%10d, command=%15s\n", i, curr->pid, curr->comm );            
        // collect/compute data about process (all data printed)
        printk( KERN_DEBUG "myread: calling collect_info_for_process\n" );                    
        struct collected_data *collected = collect_info_for_process(curr);
        
        // init task_node with collected data to insert it in soreted list
        struct  task_node* curr_node = kmalloc(sizeof(struct task_node), GFP_KERNEL);
        if (!curr_node) {
             printk( KERN_DEBUG "struct  task_node* curr_node = kmalloc(sizeof(struct task_node), GFP_KERNEL)");
             return 0;
        }
        curr_node->data = collected;
        INIT_LIST_HEAD( & curr_node->mylist ) ;
       
        printk( KERN_DEBUG "myread: calling insert_sorted\n" );                     
        // insert collected data in right place to make sorted list
        // this will be showed to user
        insert_sorted(&sorted_process_list, curr_node, ARG_SORTING_COMP);
        ++i;
    }

    struct task_node* current_data; 
    int k = 0;
    printk( KERN_DEBUG "myread: list_for_each_entry\n" );        
    len  += sprintf(buf, "\n    PID   USR  PR SPR   CPU*100    AVGCPU    MEM*100     TIME+            NAME\n");        
    list_for_each_entry ( current_data, & sorted_process_list, mylist ) 
    { 
        printk( KERN_DEBUG "myread: list_for_each_entry loop\n" );                             
         if (k == ARG_PROCES_COUNT_TO_BE_TRACKED)
             break;
            
        u64 pid = 0;
        u64 user = 0;
        u64 prio = 0;
        u64 sprio = 0;
        u64 cpu_load = 0;
        u64 avg_cpu_load = 0;
        u64 mem_load = 0;
        char* comm = "";
        char* exec_time = "";

        if (current_data->data && current_data->data->task) {
            pid = current_data->data->task->pid;
            prio = current_data->data->task->prio;
            sprio = current_data->data->task->static_prio;
            mem_load = current_data->data->mem_load;
            comm = current_data->data->task->comm;
            exec_time = exec_time_to_string(current_data->data->exec_time);
            if ( current_data->data->task->cred) {
                user = current_data->data->task->cred->uid.val;
            }
            if (current_data->data->a_cpu_load) {
                cpu_load = current_data->data->a_cpu_load->ucpu_load + current_data->data->a_cpu_load->scpu_load;
            }
            if (current_data->data->avg_cpu_load) {
                avg_cpu_load = current_data->data->avg_cpu_load->ucpu_load + current_data->data->avg_cpu_load->scpu_load;
            }
        }
 
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



         ++k; 
    }



	if(raw_copy_to_user(ubuf,buf,len))
		return -EFAULT;
	*ppos = len;



    list_for_each_entry ( current_data, & sorted_process_list, mylist ){
        if (current_data->data) {
            if (current_data->data->a_cpu_load) {
             	printk( KERN_DEBUG "Free cpu_load\n");                
	            kfree(current_data->data->a_cpu_load);
                current_data->data->a_cpu_load = 0;
            }   
            if (current_data->data->avg_cpu_load) {
             	printk( KERN_DEBUG "Free avg_cpu_load\n");                
	            kfree(current_data->data->avg_cpu_load);
                current_data->data->avg_cpu_load = 0;
            }
            printk( KERN_DEBUG "Free data\n");                
            kfree(current_data->data); 
            current_data->data = 0;
        }

    }

    /*
    struct task_node *tmp;
    struct list_head *pos, *q;

    list_for_each_entry(tmp, &sorted_process_list, mylist){
         tmp= list_entry(pos, struct task_node, mylist);
         list_del(pos);
         kfree(tmp);
    }
    */

	kfree(buf);

	return len;
}

static struct file_operations myops = 
{
	.owner = THIS_MODULE,
	.read = myread,
	.write = mywrite,
};


void ccreate_new_proc_entry(void)
{
   printk(KERN_INFO "ccreate_new_proc_entry : calling proc_create\n");
   ent=proc_create(entry_name, 0666, NULL, &myops);
}


int functn_initi (void) {
    int ret = 0;
    printk(KERN_INFO "functn_initi : HI\n");
    ccreate_new_proc_entry();
    return ret;
}

void functn_cleanup(void) {
    proc_remove(ent);
}

MODULE_LICENSE("GPL");   
module_init(functn_initi);
module_exit(functn_cleanup);
