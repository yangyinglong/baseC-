#include <sys/shm.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "tell.h"

// gcc -o bin/cal_shm -Iinclude src/tell.c src/cal_shm.c 
// src/cal_shm.c
/**
 * 利用共享内存通信
 * 首先创建共享内存，父进程映射共享内存，在共享内存中写入数据，然后解除映射，通知子进程，数据已经写好
 * 子进程映射共享内存，读取共享内存中的数据，然后解除映射，销毁共享内存（因为共享内存不能继承，所以父子进程中只有一块共享内存）
 * 先在tell.h中定义管道的基本操作，通过管道来通知数据是否写入
 */
int main(void)
{
	// 创建共享内存
	int shmid;
	if ((shmid = shmget(IPC_PRIVATE, 1024, 
			IPC_CREAT | IPC_EXCL | 0777)) < 0)
	{
		perror("shmget error");
		exit(1);
	}
	pid_t pid;

	// 初始化管道
	init();

	if ((pid = fork()) < 0)
	{
		perror("fork error");
		exit(1);
	} 
	else if (pid > 0) // 父进程
	{
		// 父进程进行共享内存的映射
		// void * shmat(int shmid, char * shmaddr, int shmflg);
		// 成功返回共享内存映射到进程虚拟内存空间中的地址，失败返回-1
		// 所以根据写入到共享内存中的数据类型，来强制转换成相应的数据类型
		// 可以是int char 结构体等等
		int *pi = (int*)shmat(shmid, 0, 0);
		// 判断是否成功映射，如果成功映射，返回进程虚拟内存地址
		// 因为pi是一个指针,存放的是共享内存中数据的地址
		// 所以吧-1转成指针
		if (pi == (int*)-1)
		{
			perror("shmat error");
			exit(1);
		}
		// 往共享内存中写入数据（通过操作映射的地址）
		*pi = 100;
		*(pi + 1) = 200;
		// 操作完毕后解除共享内存的映射
		shmdt(pi);

		// 通过管道通知子进程
		notify_pipe();
		// 销毁管道
		destroy_pipe();
		wait(0);
	}
	else // 子进程
	{
		// 子进程阻塞，等待父进程先往共享内存中写入数据
		wait_pipe();
		// 子进程从共享内存中读取数据
		// 子进程进行共享内存的映射
		int *pi = (int*)shmat(shmid, 0, 0);
		if (pi == (int*)-1)
		{
			perror("shmat error");
			exit(1);
		}
		printf("start: %d, end: %d\n", *pi, *(pi+1));
		// 读取完毕后解除映射
		shmdt(pi);
		// 删除共享内存
		shmctl(shmid, IPC_RMID, NULL);
		destroy_pipe();
	}

	exit(0);
}