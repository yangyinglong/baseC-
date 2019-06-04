#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>


/*
 * 父进程通过管道传输两个数据给子进程
 * 由子进程负责从管道中读取并输出
 */
int main(int argc, char const *argv[])
{
	int fd[2];
	// 创建管道
	if (pipe(fd) < 0)
	{
		perror("pipe error");
		exit(1);
	}
	pid_t pid;
	if ((pid = fork()) < 0)
	{
		perror("fork error");
		exit(1);
	}
	else if (pid > 0)	// parent process
	{
		close(fd[0]);	// 父进程用来写入数据，所以关闭读端口
		int start = 1, end = 100;
		if (write(fd[1], &start, sizeof(int)) != sizeof(int))
		{
			perror("write error");
			exit(1);
		}
		if (write(fd[1], &end, sizeof(int)) != sizeof(int))
		{
			perror("write error");
			exit(1);
		}
		close(fd[1]);	// 进程结束，关闭写端口
	}
	else	// child process
	{
		close(fd[1]);	// 子进程用来读取数据，关闭写端口
		int start, end;
		if (read(fd[0], &start, sizeof(int)) < 0)
		{
			perror("read error");
			exit(1);
		}
		if (read(fd[0], &end, sizeof(int)) < 0)
		{
			perror("read error");
			exit(1);
		}
		close(fd[0]);
		printf("start: %d; end: %d\n", start, end);
	}
	exit(0);
}