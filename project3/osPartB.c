/*******************************************************
 *This is the header container of our project
 * NOTE: please update the credits here.
 * Aldo Tali
 * 21500097
 *******************************************************/

#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/stat.h>
#include <linux/sched.h>
#include <linux/printk.h>
#include <linux/rculist.h>
#include <linux/signal.h>
#include <linux/sched.h>
#include <linux/sched/jobctl.h>
#include <linux/sched/task.h>
#include <linux/cred.h>
#include <linux/mm.h>
#include <linux/const.h>



//As far as I read on the online sources the linux distribution is published under GPL licence.
//since the GPL licence does not allow reuse of code (which at some points in this project we had to)
//unless it is published under the GPL licence.
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Aldo Tali");

// a task refers to a processor or a thread running in the system
// this is a linux source code macro that we used. 
#define next_task(p) \
	list_entry_rcu((p)->tasks.next, struct task_struct, tasks)


//initialize our argument to -27 by default (ideally all process ids should be positive)
int inputProcessID = -27;
int MAX_LOOP_COOUNTER = 50;
int MAXIMUM_LOGGING = 10000;

//declare the use of "inputProcessID" as a paramter to the module that is to be inserted to kernel
module_param(inputProcessID, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
MODULE_PARM_DESC(inputProcessID, "the process id parameter tht will ber searched on the kernel module");


//the following module serves for the first part of the project. 
//It takes the process that needs to be printing the information on the log file 
//as a parameter.  
static void printVMInfoForProcess(struct task_struct *process){
	
    //gets the virtual memory
	struct mm_struct *vm  = process->mm;

    //gets the list of the areas in the virtual memory
	struct vm_area_struct *listOfVMAreas = vm->mmap;

    //start position for the sections of the virtual memory
	struct vm_area_struct *initialStruct = listOfVMAreas;

    int safetycounter = 0;

    //NOTE: this is being printed just for testing purposes. Recheck it before subitshiftMaskerission
	printk(KERN_NOTICE "The proccess id: , PID: %d\n", process->pid);
	printk(KERN_NOTICE "The start of PID: %ld and the end of PID: %ld", listOfVMAreas->vm_start,listOfVMAreas->vm_end);
	

    listOfVMAreas = listOfVMAreas->vm_next;
    
    //go to each section of the virtual memory
    //NOTE: The counter for the loop is only being kept for the assumption that in the case that for some 
    //reason on a specific runsequence it goes to run to infinity the the loop should stop immediately
	while (listOfVMAreas != NULL && listOfVMAreas->vm_mm == vm && safetycounter < MAX_LOOP_COOUNTER){

		printk(KERN_NOTICE "The start and end of vm Section for this PD: %ld %ld", listOfVMAreas->vm_start,listOfVMAreas->vm_end);
		listOfVMAreas = listOfVMAreas->vm_next;
		safetycounter = safetycounter +1;
	}

    //NOTE: this is being printed just for testing purposes. Recheck it before subitshiftMaskerission
	printk(KERN_NOTICE "Areas Counter: %d\n", safetycounter);
	printk(KERN_INFO "The required structures are given below: \n");

	//This is what is required by the assignment
	//printk(KERN_INFO "%ld,%ld,%ld \n",vm->start_code,vm->highest_vm_end,vm->task_size);
	printk(KERN_INFO "VM: %ld,%ld,%ld \n",vm->start_code,vm->highest_vm_end,vm->end_code-vm->start_code);
	printk(KERN_INFO "Data: %ld,%ld,%ld\n",vm->start_data,vm->end_data,vm->end_data - vm->start_data);
	printk(KERN_INFO "Stack: %ld,%ld,%ld \n",vm->start_stack,vm->stack_vm, vm->stack_vm - vm->start_brk);
	printk(KERN_INFO "Heap: %ld,%ld,%ld \n",vm->start_brk,vm->brk,vm->brk - vm->start_brk);
	printk(KERN_INFO "Args: %ld,%ld,%ld \n",vm->arg_start,vm->arg_end, vm->arg_end - vm->arg_start);
	printk(KERN_INFO "Env: %ld,%ld,%ld \n",vm->env_start,vm->env_end, vm->env_end - vm->env_start);
	printk(KERN_INFO "No.Frames: %ld,%ld \n",vm->hiwater_rss, get_mm_rss(vm)*4);
	printk(KERN_INFO "total: %ld \n",vm->total_vm);


	return;
}

//tries to find the process with the given input id in the system
static void findPid(int pidInput){
	
	int process_counter = 0;

    //use this pointer to traverse all the processes in the system
    struct task_struct *traversePointer;

    //makes use of the next_task macro to traverse the prooccesses
    //rather than using the "current" struct makes use of the "init_task" struct
    //either wa the end result shoudl be the same since the pricesses are linked by a doubly linked list
	for (traversePointer = &init_task ; (traversePointer = next_task(traversePointer)) != &init_task ; ) {
        ++process_counter;

        //we are looking way too much, possibility that we are running on infinite loop and the system will block
		if (process_counter > MAXIMUM_LOGGING){
			printk(KERN_NOTICE "The system return from the findPID module since it was taking too long to find the result \n");
			traversePointer = NULL;
			return;
		}

        //if we find the proccess then print its information
		if ( traversePointer->pid == pidInput){
			printk(KERN_NOTICE "The proccess id %d was found ",traversePointer->pid);
			printVMInfoForProcess(traversePointer);
			return;
		}
    }

    //NOTE: this is being printed just for testing purposes. Recheck it before subitshiftMaskerission
	printk(KERN_NOTICE "There are : %d processes", process_counter);
	traversePointer = NULL;
	
    printk(KERN_NOTICE "We were not able to find the process of given id. Aborting");

}

//linux source code definitions
#define PAGE_SHIFT  12
#define PAGE_SIZE   (_AC(1,UL) << PAGE_SHIFT)
#define PAGE_MASK   (~(PAGE_SIZE-1))

// print the page contents of your cache structure
static void printMultiplePageContents(int a){
    

	int PAGE_SIZE = 512

    struct task_struct *myProccess = current;

    //get the cache
    struct mm_struct *myProccessMemory = myProccess->mm;

	int i = 0,j,k,p;
	pgd_t *topLevel = myProccessMemory->pgd;
	pgdval_t pgdValue;
	pud_t *secondLevel;
	pudval_t pudValue;
	pmd_t *thirdLevel;
	pmdval_t pmdValue;
	pte_t *fourthLevel;
	pteval_t pteValue;
	u64 bitshiftMasker = ((1LL<<(40))-1) << 12;

	void *virtualAdressTemp; //virtual addr of page table entry
	phys_addr_t virtualAdressTemp; //physical addr of page table entry

	int maskLastBit =   0b0000000000000001;
	int extract;

	for (i = 0; i < PAGE_SIZE; i++) {
		pgdValue = pgd_val(topLevel[i]);
		int mask =   0b0000111000000000;
		if (pgdValue != 0){
			printk(KERN_INFO "The memory table info for firstLevel: \n\n");
			
			virtualAdressTemp = pgdValue & bitshiftMasker;
			secondLevel = phys_to_virt(virtualAdressTemp);

			for (j = 0; j < PAGE_SIZE; j++){
				pudValue = pud_val(secondLevel[j]);
				if (pudValue != 0){
					printk(KERN_INFO "The memory table info for second: \n\n" );
					
					virtualAdressTemp = pudValue & bitshiftMasker;
					thirdLevel = phys_to_virt(virtualAdressTemp);

					for(k=0; k<PAGE_SIZE; k++){
						pmdValue = pmd_val(thirdLevel[k]);
						if(pmdValue != 0){
							printk(KERN_INFO "The memory table info for third: \n\n" );
							
							virtualAdressTemp = pudValue & bitshiftMasker;
							fourthLevel = phys_to_virt(virtualAdressTemp);

							for(p=0; p < PAGE_SIZE; p++){
								pteValue = pte_val(fourthLevel[p]);
								if (pteValue != 0){
									printk(KERN_INFO "The memory table info for fourth \n\n" );

								}
							}
						}
					}
					
				}
			}
		}
	}
	
    
    

}


static void executePart3(int pid){
	int process_counter = 0;

    //use this pointer to traverse all the processes in the system
    struct task_struct *traversePointer;

    //makes use of the next_task macro to traverse the prooccesses
    //rather than using the "current" struct makes use of the "init_task" struct
    //either wa the end result shoudl be the same since the pricesses are linked by a doubly linked list
	for (traversePointer = &init_task ; (traversePointer = next_task(traversePointer)) != &init_task ; ) {
        ++process_counter;

        //we are looking way too much, possibility that we are running on infinite loop and the system will block
		if (process_counter > MAXIMUM_LOGGING){
			printk(KERN_NOTICE "The system return from the findPID module since it was taking too long to find the result \n");
			traversePointer = NULL;
			return;
		}

        //if we find the proccess then print its information
		if ( traversePointer->pid == pid){
			printk(KERN_NOTICE "The proccess id %d was found ",traversePointer->pid);
			;
			printk(KERN_NOTICE "The total heap is: %ld\n", traversePointer->mm->brk - traversePointer->mm->start_brk );
			return;
		}
    }

    //NOTE: this is being printed just for testing purposes. Recheck it before subitshiftMaskerission
	printk(KERN_NOTICE "There are : %d processes", process_counter);
	traversePointer = NULL;
	
    printk(KERN_NOTICE "We were not able to find the process of given id. Aborting");

}


//the initializer of the module that is inserted
static int __init part3B_init(void)
{

	printk(KERN_INFO "\n=============\nThis is part3B Testing mode");
    //check if we got the parameter input with the loading of the module
    if (inputProcessID < 0){
        printk(KERN_INFO "\n=============\nPlease provide a process id to search for. Abort.");
        return;
    }

	printk(KERN_INFO "inputProcessID is an integer: %d\n", inputProcessID);

    printk("The current process id is %d\n", (int) task_pid_nr(current));
	//findPid(inputProcessID);
    printMultiplePageContents(1);
	//executePart3(inputProcessID);
	return 0;
}

static void __exit part3B_exit(void)
{
	printk(KERN_INFO "Exiting part3B testing \n");
}



module_init(part3B_init);
module_exit(part3B_exit);