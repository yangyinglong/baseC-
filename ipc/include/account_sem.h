#ifndef __ACCOUNT_H__
#define __ACCOUNT_H__

typedef struct 
{
	int code;
	double balance;
	int semid; // 在共享资源上绑定信号量集
}Account;

// 取款
extern double withdraw(Account *a, double amt);

// 存款
extern double deposit(Account *a, double amt);

// 查看账户余额
extern double get_balance(Account *);


#endif