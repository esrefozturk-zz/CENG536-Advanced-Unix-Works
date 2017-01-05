#include <linux/syscalls.h>
#include <linux/printk.h>
#include "maplock536.h"

SYSCALL_DEFINE5(map_lock , unsigned long, xlt , unsigned long, ylt , 
		unsigned long , xrb , unsigned long, yrb , short, flags) { 
	printk("in map_lock flags: %d\n", flags );
	return 0;
}

SYSCALL_DEFINE1(map_unlock , int, lockid ) {
	printk("in map_unlock for %d\n", lockid );
	return 0;
}
