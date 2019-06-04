#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int	main(int argc, char const *argv[])
{
	int fda[2], fdb[2];

	if ((pipe(fda) < 0) || pipe(fdb) < 0)
	{
		perror("pipe error");
		exit(1);
	}

	pid_t pid;
	pid = fork();
	if (pid < 0)
	{
		perror("pid error");
		exit(1);
	}
	else if (pid == 0)	// child process
	{
		/**
		 * 1) 子进程负责从管道a中读取父进程写入的累加参数x和y
		 * 2) 通过exec函数去调用bin/add程序进行累加
		 * 3) 将累加的结果写入到管道b
		 */
		close(fda[1]);
		close(fdb[0]);
		// 标准输入和输出重定向到管道输入输出
		if (dup2(fda[0], STDIN_FILENO) != STDIN_FILENO)
		{
			perror("dup2 error");
			exit(1);
		}
		if (dup2(fdb[1], STDOUT_FILENO) != STDOUT_FILENO)
		{
			perror("dup2 error");
			exit(1);
		}

		close(fda[0]);
		close(fdb[1]);

		if (execlp("bin/add", "bin/add", NULL) < 0)
		{
			perror("execlp error");
			exit(1);
		}

	}
	else	// parent process
	{
		/**
		 * 1) 父进程从标准输入上读取累加参数x和y
		 * 2) 将x和y写入管道a
		 * 3) 从管道b中读取累加的结果并输出
		 */
		close(fda[0]);
		close(fdb[1]);
		int x, y;
		printf("please input x and y: ");
		scanf("%d %d", &x, &y);
		if (write(fda[1], &x, sizeof(int)) != sizeof(int))
		{
			perror("write error");
			exit(1);
		}
		if (write(fda[1], &y, sizeof(int)) != sizeof(int))
		{
			perror("write error");
			exit(1);
		}
		int result = 0;
		if (read(fdb[0], &result, sizeof(int)) != sizeof(int))
		{
			perror("read error");
			exit(1);
		}
		else
		{
			printf("add result is %d\n", result);
		}
		close(fda[1]);
		close(fdb[0]);
		wait(0);
	}
	return 0;
}