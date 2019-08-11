/** 
 *			cmd_pipe_1.c
 * 功能：	通过多进程和管道实现命令 
 *			cat /etc/passwd | awk -F: '{print $7}' | grep sh | wc -l
 * 描述：	有四条命令，所以需要四个进程，三个管道
 					parent-----------------------------------------------------------
						|				|						|					|
 				frist child	==FD1== second child ==FD2== third child ==FD3== forth child				
 *
 * 输出：	yangyl@ubuntu:~/projects/cpp/baseC-/ipc$ ./bin/cmd_pipe_1 
 *			parent process pid: 1561
 *			forth child process pid: 1565
 *			third child process pid: 1564
 *			second child process pid: 1563
 *			frist child process pid: 1562
 *			2
 *
 * 备注：	一开始由父进程创建了三个子进程，由父进程和三个子进程之间相互通讯实现命令，
 *			但是发现父进程不能等待子进程执行结束，在执行上有错误，所以由父进程创建四个子进程来执行
 */

#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

char *cmd1[3] = {"/bin/cat", "/etc/passwd", NULL};
char *cmd2[3] = {"awk", "-F:", "{print $7}"};
char *cmd3[3] = {"/bin/grep", "sh", NULL};
char *cmd4[3] = {"wc", "-l", NULL};

int main(void)
{

	pid_t pid;
	int fd1[2];
	int fd2[2];
	int fd3[2];

	// 创建三个管道
	if (pipe(fd1) < 0 || pipe(fd2) < 0 || pipe(fd3) < 0)
	{
		perror("pipe error");
		exit(1);
	}

	// 创建三个子进程
	for (int i = 0; i < 4; ++i)
	{
		pid = fork();
		if (pid < 0)
		{
			perror("fork error");
			exit(1);
		}
		else if (0 == pid)
		{
			if (0 == i)
			{
				// frist child procese
				printf("frist child process pid: %d\n", getpid());

				// 关闭管道fd1的写端，管道fd2的读端，管道fd3
				close(fd1[1]);
				close(fd2[0]);
				close(fd3[0]);
				close(fd3[1]);

				// 将标准输入重定向到管道fd1的读取
				if (dup2(fd1[0], STDIN_FILENO) != STDIN_FILENO)
				{
					perror("dup2 error");
					exit(1);
				}
				// 将标准输出重定向到管道fd2的写端
				if (dup2(fd2[1], STDOUT_FILENO) != STDOUT_FILENO)
				{
					perror("dup2 error");
					exit(1);
				}
				// 关闭管道fd1的读端和fd2的写端
				close(fd1[0]);
				close(fd2[1]);
				// 第一个子进程执行命令 cmd2: awk -F: '{print $7}'
				if (execvp(cmd2[0], cmd2) < 0)
				{
					perror("execvp error 2");
					exit(1);
				}
				break;
			}
			if (1 == i)
			{
				// second child procese
				printf("second child process pid: %d\n", getpid());

				// 关闭管道fd1，管道fd2的写端，管道fd3的读端
				close(fd1[0]);
				close(fd1[1]);
				close(fd2[1]);
				close(fd3[0]);

				// 将标准输入重定向到管道fd1的读取
				if (dup2(fd2[0], STDIN_FILENO) != STDIN_FILENO)
				{
					perror("dup2 error");
					exit(1);
				}
				// 将标准输出重定向到管道fd2的写端
				if (dup2(fd3[1], STDOUT_FILENO) != STDOUT_FILENO)
				{
					perror("dup2 error");
					exit(1);
				}
				// 关闭管道fd2的读端和fd3的写端
				close(fd2[0]);
				close(fd3[1]);
				// 第二个子进程执行命令 cmd3: grep sh
				if (execvp(cmd3[0], cmd3) < 0)
				{
					perror("execvp error 3");
					exit(1);
				}
				break;
			}
			if (2 == i)
			{
				// third child procese
				printf("third child process pid: %d\n", getpid());

				// 关闭管道fd1，管道fd2，管道fd3的写端
				close(fd1[0]);
				close(fd1[1]);
				close(fd2[0]);
				close(fd2[1]);
				close(fd3[1]);

				// 将标准输入重定向到管道fd3的读取
				if (dup2(fd3[0], STDIN_FILENO) != STDIN_FILENO)
				{
					perror("dup2 error");
					exit(1);
				}

				// 关闭管道fd3的读端
				close(fd3[0]);

				// 第三个子进程执行命令 cmd4: wc -l
				if (execvp(cmd4[0], cmd4) < 0)
				{
					perror("execvp error 4");
					exit(1);
				}
				break;
			}
			if (3 == i)
			{
				// forth child procese
				printf("forth child process pid: %d\n", getpid());

				// // 关闭fd1的读端以及管道fd2和fd3
				close(fd1[0]);
				close(fd2[0]);
				close(fd2[1]);
				close(fd3[0]);
				close(fd3[1]);

				// 将标准输出重定向到fd1的写端，然后关闭fd1[1]
				if (dup2(fd1[1], STDOUT_FILENO) != STDOUT_FILENO)
				{
					perror("dup2 error");
					exit(1);
				}
				// 执行命令 cmd1: cat /etc/passwd
				if (execvp(cmd1[0], cmd1) < 0)
				{
					perror("execvp error 1");
					exit(1);
				}
				break;
			}
		}
		else
		{
			if (3 == i)
			{
				printf("parent process pid: %d\n", getpid());
				close(fd1[0]);
				close(fd1[1]);
				close(fd2[0]);
				close(fd2[1]);
				close(fd3[0]);
				close(fd3[1]);
				
				wait(0);
				wait(0);
				wait(0);
				wait(0);
			}
			
		}
	}
	exit(0);
}