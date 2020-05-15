
#include<linux/module.h>
#include<linux/kernel.h>
#include <linux/proc_fs.h>
#include<linux/sched/signal.h>
#include <linux/types.h>
#include <linux/ktime.h>
#include <asm/param.h>
#include <linux/sched/cputime.h>
#include <linux/jiffies.h>
#include <linux/types.h>
#include <linux/errno.h>
#include <linux/time.h>
#include <linux/kernel.h>
#include <linux/kernel_stat.h>
#include <linux/tty.h>
#include <linux/string.h>
#include <linux/mman.h>
#include <linux/sched/mm.h>
#include <linux/sched/numa_balancing.h>
#include <linux/sched/task_stack.h>
#include <linux/sched/task.h>
#include <linux/sched/cputime.h>
#include <linux/proc_fs.h>
#include <linux/ioport.h>
#include <linux/uaccess.h>
#include <linux/io.h>
#include <linux/mm.h>
#include <linux/hugetlb.h>
#include <linux/pagemap.h>
#include <linux/swap.h>
#include <linux/smp.h>
#include <linux/signal.h>
#include <linux/highmem.h>
#include <linux/file.h>
#include <linux/fdtable.h>
#include <linux/times.h>
#include <linux/cpuset.h>
#include <linux/rcupdate.h>
#include <linux/delayacct.h>
#include <linux/seq_file.h>
#include <linux/pid_namespace.h>
#include <linux/prctl.h>
#include <linux/ptrace.h>
#include <linux/tracehook.h>
#include <linux/string_helpers.h>
#include <linux/user_namespace.h>
#include <linux/fs_struct.h>
#include<linux/syscalls.h> //We're a syscall
#include<linux/sched.h> //Needed for the for_each_process() macro
#include<linux/jiffies.h> //Needed to manage the time
#include<asm/uaccess.h> //Needed to use copy_to_user
#include <linux/tty.h>
#include <linux/linkage.h>
#include <linux/module.h>	/* Needed by all modules */
#include <linux/kernel.h>	/* Needed for KERN_INFO */
#include <linux/init.h>		/* Needed for the macros */
#include <linux/kthread.h>
#include <linux/sched.h>
#include <asm/uaccess.h>
#include <linux/string.h>
#include <linux/jiffies.h>
#include <linux/tty.h>
#include <linux/slab.h>
#include <linux/semaphore.h>
#include <linux/kernel.h>
#include <linux/delay.h>
#include <asm/uaccess.h>
#include <linux/syscalls.h>
static struct proc_dir_entry *ent;

#define BUFSIZE  1000
#define MAXPROCCOUNT 2000
struct task_struct* tasks [MAXPROCCOUNT];

int read_proc(char *buf,char **start,off_t offset,int count,int *eof,void *data )
{
	int len=0;
	struct task_struct *task_list;


	int i = 0;
	for_each_process(task_list) {

		if (i == 7)
			break;
		i++;
       		len  += sprintf(buf+len, "\n %s %d \n",task_list->comm,task_list->pid);
 	}
  
	return len;
}



static ssize_t mywrite(struct file *file, const char __user *ubuf, size_t count, loff_t *ppos) 
{


	printk( KERN_DEBUG "write handler\n");
	int num,c,i,m;
	char buf[BUFSIZE];
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



#include <linux/types.h>

u64 nsec_to_clock_t(u64 x)
{
#if (NSEC_PER_SEC % USER_HZ) == 0
	return div_u64(x, NSEC_PER_SEC / USER_HZ);
#elif (USER_HZ % 512) == 0
	return div_u64(x * USER_HZ / 512, NSEC_PER_SEC / 512);
#else
	/*
         * max relative error 5.7e-8 (1.8s per year) for USER_HZ <= 1024,
         * overflow after 64.99 years.
         * exact for HZ=60, 72, 90, 120, 144, 180, 300, 600, 900, ...
         */
	return div_u64(x * 9, (9ull * NSEC_PER_SEC + (USER_HZ / 2)) / USER_HZ);
#endif
}
static ssize_t myread(struct file *file, char __user *ubuf, size_t count, loff_t *ppos) 
{

	char buf[BUFSIZE];
	printk( KERN_DEBUG "read handler\n");
	if(*ppos > 0 || count < BUFSIZE)
		return 0;
	//len += sprintf(buf,"irq = %d\n",irq);
	//len += sprintf(buf + len,"mode = %d\n",mode);
	//len += sprintf(buf + len,"bufzise = %d\n", BUFSIZE);

	int len=0;
	struct task_struct *task_list;
	int i = 300;
        int j = 0;


		u64 utime_s = 0; 
		u64 stime_s = 0; 

		u64 total_time_s = 0;	
	
		u64 uptime_s = 0; // checked


/*
	for_each_process(task_list) {

		utime_s = task_list->utime;
		stime_s = task_list->stime;

		total_time_s = total_time_s + task_list->signal->cutime + task_list->signal->cstime;
		total_time_s = ktime_divns(total_time_s, NSEC_PER_SEC);

		uptime_s = ktime_divns(ktime_get_coarse_boottime(), NSEC_PER_SEC); // checked

	}
	*/
	int k = 0;
	for_each_process(task_list) {
		if (tasks[k] == 0)
			printk( KERN_DEBUG "Ith proc is null %d\n", k);
		else if (tasks[k]->pid == task_list->pid)
			printk( KERN_DEBUG "Ith %d equal proc pid=%d task_list pid =%d\n", k, tasks[k]->pid, task_list->pid);
		else
			printk( KERN_DEBUG "Ith %d proc pid=%d task_list pid =%d\n", k, tasks[k]->pid, task_list->pid);

		++k;
		
		printk( KERN_DEBUG "read handler1, pComm = %s, pid = %d, user = %d, priority = %d, acct_vm_mem1 = %d\n" ,
				task_list->comm,
				task_list->pid, 
				task_list->cred->uid,
				task_list->prio
				,task_list->acct_vm_mem1
				/*,task_list->mm->total_vm*/
				/*,task_list->active_mm->total_vm*/	);
		if (task_list->mm)
			printk( KERN_DEBUG "total_vm = %d\n", task_list->mm->total_vm);
		if (task_list->active_mm)
			printk( KERN_DEBUG "total_vm = %d\n", task_list->active_mm->total_vm);


		/*
		u64 utime_ = ktime_divns(task_list->utime, MSEC_PER_SEC);
		u64 stime_ = ktime_divns(task_list->stime, MSEC_PER_SEC);
		u64 total_time = utime_ + stime_;

                u64 utime = 0;
		u64 stime = 0;
		//task_cputime(task_list, &utime, &stime);
		cputime_adjust(task_list, &utime, &stime);	
	        task_cputime_adjusted(task_list, &utime, &stime);
		u64 cl_utime = ktime_to_nscputime_to_nsecs(utime);
		u64 cl_stime = ktime_to_nscputime_to_nsecs(stime);
		//u64 cl_utime = nsecs_to_jiffies(utime);
		//u64 cl_stime = nsecs_to_jiffies(stime);
		u64 utime_jiffies = nsecs_to_jiffies(utime_);
		u64 stime_jiffies = nsecs_to_jiffies(stime_);
                //u64 cl_utime = div_u64(utime, NSEC_PER_SEC / USER_HZ);
                //u64 cl_stime = div_u64(stime, NSEC_PER_SEC / USER_HZ);

		ktime_t Utime = 0;
	 	ktime_t Stime = 0;
	 
		struct task_cputime cputime;
	        //task_cputime_adjusted(task_list, &Utime, &Stime);

	        thread_group_cputime(task_list, &cputime);
		Utime = cputime.utime;
		Stime = cputime.stime;
		//thread_group_cputime_adjusted(task_list, &Utime, &Stime);
 		ktime_t jiff = Utime + Stime;
		u64 secs = jiff/HZ;
		*/

                u64 utime = task_list->utime;
		u64 stime = task_list->stime;
		u64 utime_jiffies = nsecs_to_jiffies(utime);
		u64 stime_jiffies = nsecs_to_jiffies(stime);
		u64 total_time = utime + stime;
		u64 utime_ = ktime_divns(task_list->utime, MSEC_PER_SEC);
		u64 stime_ = ktime_divns(task_list->stime, MSEC_PER_SEC);

		total_time = ktime_divns((total_time + task_list->signal->cutime + task_list->signal->cstime), NSEC_PER_SEC);
		//if (task_list->signal)
		 // total_time = total_time + ktime_divns(task_list->signal->cutime, NSEC_PER_SEC) +  ktime_divns(task_list->signal->cstime, NSEC_PER_SEC);
    		
		u64  uptime;
    		uptime = ktime_divns(ktime_get_coarse_boottime(), NSEC_PER_SEC); // checked
		u64 start_ =  task_list->start_time; //ktime_divns(task_list->start_time, NSEC_PER_SEC); // checked
                u64 start = ktime_divns(start_, NSEC_PER_SEC);
		u64 seconds = uptime - start /*(task_list->start_time /(NSEC_PER_SEC * HZ))*/;


                u64 cpu_usage = 0;
	        if (seconds != 0)
		  cpu_usage = ((total_time * 10000) / seconds);
	        printk( KERN_DEBUG "utime = %ld, stime=%ld, utime_ = %ld, stime_=%ld\n", utime, stime, utime_, stime_ /*task_list->vtime->utime, task_list->vtime->stime*/ );

                utime = nsec_to_clock_t(utime);
	        stime = nsec_to_clock_t(stime);



		utime_s = task_list->utime;
		stime_s = task_list->stime;

		total_time_s = utime_s + stime_s;

		total_time_s = total_time_s + task_list->signal->cutime + task_list->signal->cstime;
		total_time_s = ktime_divns(total_time_s, NSEC_PER_MSEC);

		uptime_s = ktime_divns(ktime_get_coarse_boottime(), NSEC_PER_MSEC); // checked
		//uptime_s = ktime_get_coarse_boottime(); // checked


		if (task_list->pid == 3772)
		  msleep(2000); //sleep for 1 sec



		u64 utime_e = task_list->utime;
		u64 stime_e = task_list->stime;
		u64 total_time_e = utime_e + stime_e;

		total_time_e = total_time_e + task_list->signal->cutime + task_list->signal->cstime;
		total_time_e = ktime_divns(total_time_e, NSEC_PER_MSEC);

		u64 uptime_e = ktime_divns(ktime_get_coarse_boottime(), NSEC_PER_MSEC); // checked
		//u64 uptime_e = ktime_get_coarse_boottime(); // checked

		u64 cpuUsage = 0;
 		if (uptime_e - uptime_s != 0)
                	//cpuUsage = (total_time_e - total_time_s)* 10000/(uptime_e - uptime_s); // asctual cpu usage
                	cpuUsage = (total_time_e - total_time_s)* 10000/(uptime_e - uptime_s); // asctual cpu usage


		printk( KERN_DEBUG "command=%s, uptime_s=%d,   uptime_e=%d,   tot_time_s=%d, tot_time_e=%d, CPU usage * 100 =%lld, CPU average usage * 100 = %lld,utime_jiffies =%d, stime_jiffies=%d, utime=%lld, stime=%lld, NSEC_PER_SEC=%d, HZ=%lld, bottomtime=%d, start_time %d, utime=%d, stime=%d, total_time=%lld, uptime=%lld, seconds=%lld\n",task_list->comm,  uptime_s,   uptime_e, total_time_s, total_time_e, cpuUsage, cpu_usage, utime_jiffies,stime_jiffies, utime, stime, NSEC_PER_SEC, HZ,ktime_get_coarse_boottime(),  start, utime, stime, total_time, uptime, seconds);





		if (i > 300 && j <= 10) {
       		  len  += sprintf(buf+len, "\n %s %d \n",task_list->comm,task_list->pid);
 		  j++;
		}
	        i++;
  	}
  



         	printk( KERN_DEBUG "read handler2 count = %d\n", i);
	
	//void * stuff;
	//stuff = kmalloc(BUFSIZE, GFP_KERNEL);
	//char* st = (char*)stuff;
	//*st = 'a'; *(st + 1) = 'r'; *(st + 2) = 'n', *(st + 3) = 'b'; *(st + 4) = '\0';

        //len += sprintf(buf + len, "Allocation finished! %zu, the range is %p-%p\n", ksize(stuff), stuff, stuff + ksize(stuff) - 1);
        //len += sprintf(buf + len, "String written in alocated mem is %s\n", st);

	
 	//kfree(stuff);



	if(raw_copy_to_user(ubuf,buf,len))
		return -EFAULT;
	*ppos = len;

	return len;
}


/*
static ssize_t myread(struct file *file, char __user *ubuf,size_t count, loff_t *ppos) 
{
	printk( KERN_DEBUG "read handler\n");
	return 0;
}
 */

static struct file_operations myops = 
{
	.owner = THIS_MODULE,
	.read = myread,
	.write = mywrite,
};
void ccreate_new_proc_entry(void);
void ccreate_new_proc_entry(void)
{
   printk(KERN_INFO "Hello, It is procss_list init!\n");
  
   ent=proc_create("proci", 0666, NULL, &myops);
  //create_proc_read_entry("ps_list",0,NULL,read_proc,NULL);

}

DECLARE_HASHTABLE(data_hash, 10);
//hash_init(data_hash, 1000);
//

struct struu {

	int a;
	int b;
};


struct h_struct {
    int key;
    struct task_struct* data;
    int state;
    /* other driver specific fields */
    struct hlist_node node;
};

int functn_init (void) {
    int ret = 0;
   
    ccreate_new_proc_entry();
    int i = 0;
    for (; i < MAXPROCCOUNT; ++i)
	    tasks[i] = 0;

    struct struu s;

    i = 0;
    struct task_struct* cur;
    for_each_process(cur) {
	    tasks[i] = cur;
	    ++i;
    }
    struct h_struct *h = kmalloc(sizeof(struct h_struct), 0);
    h->key = 1439;
    h->data = tasks[210];
    h->state = 1;

    hash_add(data_hash, &h->node, h->key);
    struct h_struct *el;
    struct hlist_node* nd;
    int bkt =0;
    hash_for_each(data_hash, bkt, el, node)
      printk(KERN_INFO "hash bkt=%d, key=%d, stat=%d, pid=%lld, comm=%s\n",bkt, el->key, el->state, el->data->pid, el->data->comm);



    return ret;
}

void functn_cleanup(void) {
    proc_remove(ent);
}

MODULE_LICENSE("GPL");   
module_init(functn_init);
module_exit(functn_cleanup);
