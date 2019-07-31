#include "account_sem.h"
#include "pv.h"
#include <unistd.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


/*
功能：	利用进程信号量的PV操作实现多进程对银行账户操作的互斥。
I(1):	在银行账户上绑定信号量/信号灯(初值为1)
	进程1 取款					进程2 取款
（任何操作都可以）			（任何操作都可以）
----------------------------------------------
	P(1)						P(1)
	withdraw()					withdraw()
	V(1)						V(1)
*/

int main(void)
{
	// 在共享内存中创建银行账户
	int shmid;
	if ((shmid = shmget(IPC_PRIVATE, sizeof(Account), IPC_CREAT | IPC_EXCL | 0777)) < 0)
	{
		perror("shmget error");
		exit(1);
	}
	// 进行共享内存的映射
	Account *a = (Account*)shmat(shmid, 0, 0);
	if (a == (Account*)-1)
	{
		perror("shmat error");
		exit(1);
	}
	a->code = 100001;
	a->balance = 10000;
	// 创建信号量集并初始化，初值为1
	a->semid = I(1, 1);
	if (a->semid < 0)
	{
		perror("I(1,1) init error");
		exit(1);
	}
	printf("balance: %f\n", a->balance);

	pid_t pid;
	if ((pid = fork()) < 0)
	{
		perror("fock error");
		exit(1);
	}
	else if (pid > 0)
	{
		// 父进程执行取款操作
		double amt = withdraw(a, 10000);
		printf("pid %d withdraw %f from code %d\n", getpid(), amt, a->code);
		wait(0);

		// 对共享内存的操作要在解除映射之前
		printf("balance: %f\n", a->balance);
		// 销毁信号量
		D(a->semid);
		shmdt(a);
		// 删除共享内存
		shmctl(shmid, IPC_RMID, NULL);
	}
	else
	{
		// 子进程执行取款操作
		double amt = withdraw(a, 10000);
		printf("pid %d withdraw %f from code %d\n", getpid(), amt, a->code);
		shmdt(a);
	}


	exit(0);
}	
