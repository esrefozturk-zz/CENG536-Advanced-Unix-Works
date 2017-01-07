#include <linux/syscalls.h>
#include <linux/printk.h>
#include "maplock536.h"
#include <linux/semaphore.h>
#include <linux/mutex.h>
#include <linux/slab.h>




long current_lock_id=1;
long current_block_id=1;

typedef struct asd{
	long id;
	unsigned long xlt;
	unsigned long ylt;
	unsigned long xrb;
	unsigned long yrb;
	short flag;
} lock;

typedef struct qwe{
	long id;
	unsigned long xlt;
	unsigned long ylt;
 	unsigned long xrb;
	unsigned long yrb;
	short flag;
	mutex m;
} block;


lock locks[100000];
block blocks[100000];


static DECLARE_MUTEX(locks_mutex);
static DECLARE_MUTEX(blocks_mutex);

int intersects(lock l, unsigned long  xlt, unsigned long  ylt,unsigned long  xrb,unsigned long  yrb)
{
  return !(   (l.xlt+xrb<xlt)
          &&  (xlt+xrb<l.xlt)
          &&  (l.ylt+yrb<ylt)
          &&  (ylt+yrb<l.ylt));
}


int insert_block(unsigned long  xlt, unsigned long  ylt,unsigned long  xrb,unsigned long  yrb)
{
	int i;
	mutex_lock(blocks_mutex);
	for(i=0;i<100002;i++)
	{
		if( !(blockeds[i].id) )
		{
			blockeds[i].id = current_blocked_id++;
			blockeds[i].xlt = xlt;
			blockeds[i].ylt = ylt;
			blockeds[i].xrb = xrb;
			blockeds[i].yrb = yrb;
			init_mutex( &( blockeds[i].mutex ) );
			mutex_lock( &( blockeds[i].mutex ) )
			break;
		}
	}	
	mutex_unlock(blocks_mutex);
}

int insert_lock(unsigned long  xlt, unsigned long  ylt,unsigned long  xrb,unsigned long  yrb, short flag)
{
	int bindex;
	int fool = 0;
	int i;		

	mutex_lock(locks_mutex);
	for(i=0;i<100000;i++)
	{
		if( locks[i].id && intersects(locks[i],xlt,ylt,xrb,yrb) )
		{
			if( !fool )
			{
				fool = 1;
				bindex = insert_block(xlt,ylt,xrb,yrb,flag);
			}
			mutex_unlock(locks_mutex);
			mutex_lock(blocks[bindex]);
			i=0;
			mutex_lock(locks_mutex);
			mutex_unlock(blocks[bindex]);		
		}
	}
	if( fool )
	{
		delete_block(bindex);
	}
	for(i=0;i<100000;i++)
	{
		if( !(locks[i].id) )
		{
			locks[i].id = locks_current_id++;
			locks[i].xlt=xlt;
                        locks[i].ylt=ylt;
                        locks[i].xrb=xrb;
                        locks[i].yrb=yrb;			
			locks[i].flag=flag;
			break;
		}
	}
	mutex_unlock(locks_mutex);
}




SYSCALL_DEFINE5(map_lock , unsigned long, xlt , unsigned long, ylt , 
		unsigned long , xrb , unsigned long, yrb , short, flags) { 
	
	return insert_lock(xlt,ylt,xrb,yrb,flags);
}

SYSCALL_DEFINE1(map_unlock , int, lockid ) {
	int i;
	
	mutex_lock(locks_mutex);

	for(i=0;i<current_lock_id;i++)
	{
		if(lockid==locks[i].id)
		{
			up_write(&(locks[i].sem));
			break;		
		}
	}

	mutex_unlock(locks_mutex);

	return 0;
}

