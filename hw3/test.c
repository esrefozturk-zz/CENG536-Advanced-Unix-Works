#include<stdio.h>
#define _GNU_SOURCE
#include<unistd.h>
#include<sys/syscall.h>
#define SYS_MAP_LOCK 317
#define SYS_MAP_UNLOCK 318

int main() {
	syscall(SYS_MAP_LOCK, 0,0,0,0,0);
	syscall(SYS_MAP_UNLOCK, 0);
}
