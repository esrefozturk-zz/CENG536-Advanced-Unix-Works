#include <linux/syscalls.h>
#include <linux/printk.h>
#include "maplock536.h"
#include <linux/semaphore.h>
#include <linux/rwsem.h>
#include <linux/slab.h>

struct rw_semaphore *sem;



SYSCALL_DEFINE5(map_lock , unsigned long, xlt , unsigned long, ylt , 
		unsigned long , xrb , unsigned long, yrb , short, flags) { 

down_write(sem);

	return 0;
}

SYSCALL_DEFINE1(map_unlock , int, lockid ) {
	

	return 0;
}

int maplock_init(void)
{
	sem = kmalloc(sizeof(struct rw_semaphore),GFP_KERNEL);
	init_rwsem(sem);
	return 0;
}

void maplock_deinit(void)
{	
	return;
}

module_init(maplock_init);
module_exit(maplock_deinit);
