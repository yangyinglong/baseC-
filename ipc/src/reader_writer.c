/* @file: reader_writer.c
 *
 * 目的： 	利用进程信号量的PV操作实现进程间的同步问题
 *			共享内存中读写数据（读者和写者问题）
 *			I(s1, 0)				I(s2, 0)
 *			写者						读者
 *		--------------------------------------------
 *			while(1) {				while(1){
 *	
 *				写入数据...				P(s1);
 *				V(s1);					读取数据...
 *				P(s2);					V(s2);
 *
 *			}						}
 *		--------------------------------------------
 * 读者：首先写者进程写入数据，对写信号量s1做V操作，
 *		然后对读信号量s2做P操作，
 *		由于读者进程还没有有读取完，所以读进程阻塞在P(s2)这一步
 * 写者：读者进程在读取数据之后，对读者信号量s2做V操作，
 *		告诉写者进程数据已经读取完，写者进程的P(s2)不再阻塞
 *		读者进程执行P(s1)，等待写者进程写入数据
 */

#include <sys/shm.h>
#include <sys/sem.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

// 读者和写着的共享资源
typedef struct 
{
	int	val;	// 存储数据 
	int	semid;	// 绑定的信号量id
}Storage;

// 初始化信号量
void init(Storage *s)
{
	assert(s != NULL);
	// 创建信号量集（包含2个信号量）
	if ((s->semid = semget(IPC_PRIVATE, 2, 
				IPC_CREAT | IPC_EXCL | 0777)) < 0)
	{
		perror("semget error");
		exit(1);
	}
	// 对信号量集中的所有信号量初始化
	union semun
	{
		int 				val;
		struct semid_ds 	*ds;
		unsigned short 		*array;
	};
	union semun un;
	// 2个信号量的初始值设置为0
	unsigned short array[2] = {0, 0};
	un.array = array;
	if (semctl(s->semid, 0, SETALL, un) < 0)
	{
		perror("semctl error");
		exit(1);
	}
}

void destroy(Storage *s)
{
	assert(s != NULL);
	if (semctl(s->semid, 0, IPC_RMID, NULL) < 0)
	{
		perror("semctl error");
		exit(1);
	}
}

void write(Storage *s, int val)
{
	assert(s != NULL);

	// 写入数据到 storage
	s->val = val;
	printf("%d write %d\n", getpid(), val);

	// V(s1) 对0号信号量做V(1)
	struct sembuf ops_v[1] = {{0, 1, SEM_UNDO}};
	if (semop(s->semid, ops_v, 1) < 0)
	{
		perror("semop error");
		exit(1);
	}

	// P(s2) 对1号信号量做P(1)
	struct sembuf ops_p[1] = {{1, -1, SEM_UNDO}};
	if (semop(s->semid, ops_p, 1) < 0)
	{
		perror("semop error");
		exit(1);
	}
}

void read(Storage *s)
{
	assert(s != NULL);

	// P(s1)
	struct sembuf ops_p[1] = {{0, -1, SEM_UNDO}};
	if (semop(s->semid, ops_p, 1) < 0)
	{
		perror("semop error");
	}

	// 读取数据
	printf("%d read %d\n", getpid(), s->val);

	// V(s2)
	struct sembuf ops_v[1] = {{1, 1, SEM_UNDO}};
	if (semop(s->semid, ops_v, 1) < 0)
	{
		perror("semop error");
	}
}

int main(void)
{
	// 将共享资源Storage创建在共享内存中
	int shmid;
	if ((shmid =shmget(IPC_PRIVATE, sizeof(Storage),
					IPC_CREAT | IPC_EXCL | 0777)) < 0)
	{
		perror("shmget error");
		exit(1);
	}
	// 父进程进项共享内存映射
	Storage *s  = (Storage*)shmat(shmid, 0, 0);
	if (s == (Storage *)-1)
	{
		perror("shmat error");
		exit(1);
	}

	// 创建信号量集并初始化
	init(s);

	// 创建子进程
	pid_t pid;
	
	if ((pid = fork()) < 0)
	{
		perror("fork error");
		exit(1);
	}
	else if (pid > 0)
	{
		// 父进程
		for (int i = 0; i < 100; ++i)
		{
			write(s, i);
		}
		// 等待子进程回收
		wait(0);
		// 销毁信号量集
		destroy(s);
		// 解除共享内存映射
		shmdt(s);
		// 删除共享内存
		shmctl(shmid, IPC_RMID, NULL);
	}
	else
	{
		// 子进程
		for (int i = 0; i < 100; ++i)
		{
			read(s);
		}
		shmdt(s);
	}
	exit(0);
}