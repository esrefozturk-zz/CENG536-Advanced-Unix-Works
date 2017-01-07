#include <linux/syscalls.h>
#include <linux/printk.h>
#include "maplock536.h"
#include <linux/semaphore.h>
#include <linux/mutex.h>
#include <linux/slab.h>




#define N 100002
#define UL unsigned long

static DEFINE_MUTEX(locks_mutex);
static DEFINE_MUTEX(blocks_mutex);

static int current_lock_id=1;

typedef struct asd{
	long id;
	UL xlt;
	UL ylt;
	UL xrb;
	UL yrb;
	short flags;
} lock;

typedef struct qwe{
	short exists;
	UL xlt;
	UL ylt;
        UL xrb;
        UL yrb;
	struct semaphore sem;
} block;

static lock locks[N];
static block blocks[N];

int intersection(UL xlt1, UL ylt1, UL xrb1, UL yrb1, UL xlt2, UL ylt2, UL xrb2, UL yrb2)
{
	return 1;	
}


int lock_intersection( lock l, UL xlt, UL ylt, UL xrb, UL yrb)
{
	return intersection(l.xlt,l.ylt,l.xrb,l.yrb,xlt,ylt,xrb,yrb);
}

int block_intersection(block b,UL xlt, UL ylt, UL xrb, UL yrb)
{
	return intersection(b.xlt,b.ylt,b.xrb,b.yrb,xlt,ylt,xrb,yrb);
}

int insert_block(UL xlt, UL ylt, UL xrb, UL yrb)
{
	int i;

        mutex_lock(&blocks_mutex);

	for(i=0;i<N;i++)
	{
		if(!(blocks[i].exists))
		{
			break;
		}
	}        
	blocks[i].exists = 1;
	blocks[i].xlt = xlt;
	blocks[i].ylt = ylt;
	blocks[i].xrb = xrb;
	blocks[i].yrb = yrb;
	sema_init(&(blocks[i].sem),0);        

	mutex_unlock(&blocks_mutex);

	return i;
}

void delete_block(int bindex)
{
	mutex_lock(&blocks_mutex);
	blocks[bindex].exists = 0;
	mutex_unlock(&blocks_mutex);
}

int my_map_lock(UL xlt, UL ylt, UL xrb, UL yrb, short flags)
{
	int lock_id=0;
	int i=0;
	short flag=0;	
	int bindex;

	while(!flag)
	{
		mutex_lock(&locks_mutex);
		for(i=0;i<N;i++)
		{
			if( locks[i].id && lock_intersection(locks[i],xrb,yrb,xlt,ylt) )
			{
				bindex=insert_block(xrb,yrb,xlt,ylt);
				mutex_unlock(&locks_mutex);
				down(&(blocks[bindex].sem));
				delete_block(bindex);
				break;		
			}
		}
		if(i==N)
		{
			flag=1;
		}
	}
	for(i=0;i<N;i++)
	{
		if(!(locks[i].id))
		{
			break;
		}
	}
	locks[i].id = current_lock_id++;
	locks[i].xrb = xrb;
	locks[i].yrb = yrb;
	locks[i].xlt = xlt;
	locks[i].ylt = ylt;
	locks[i].flags = flags;
	lock_id = locks[i].id;

	mutex_unlock(&locks_mutex);
	return lock_id;
}

int my_map_unlock(int lockid)
{
	int i;
	int xrb,yrb,xlt,ylt;

	mutex_lock(&locks_mutex);
        for(i=0;i<N;i++)
        {
                if(locks[i].id==lockid)
                {
                        break;
                }
        }
	if(i==N)
	{
		mutex_unlock(&locks_mutex);
		return -1;
	}
	xrb = locks[i].xrb;
	yrb = locks[i].yrb;
	xlt = locks[i].xlt;
	ylt = locks[i].ylt;
	locks[i].id = 0;
	mutex_unlock(&locks_mutex);
	
	mutex_lock(&(blocks_mutex));

	for(i=0;i<N;i++)
	{
		if( blocks[i].exists && block_intersection(blocks[i],xrb,yrb,xlt,ylt) )
		{
			up(&(blocks[i].sem));
		}
	}

	mutex_unlock(&(blocks_mutex));
	return 0;
}

SYSCALL_DEFINE5(map_lock , unsigned long, xlt , unsigned long, ylt , 
		unsigned long , xrb , unsigned long, yrb , short, flags) { 
	return my_map_lock(xlt, ylt, xrb, yrb, flags);
}

SYSCALL_DEFINE1(map_unlock , int, lockid ) {
	return my_map_unlock(lockid);
}

