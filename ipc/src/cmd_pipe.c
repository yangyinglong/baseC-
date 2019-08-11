#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

// char *cmd1[3] = {"/bin/cat", "/etc/passwd", NULL};
// char *cmd2[3] = {"/bin/grep", "root", NULL};

char *cmd1[3] = {"/bin/cat", "/etc/passwd", NULL};
char *cmd2[3] = {"awk", "-F:", "{print $7}"};
// awk -F: '{print $7}'
int main(int argc, char const *argv[])
{
	int fd[2];
	if (pipe(fd) < 0)
	{
		perror("pipe error");
		exit(1);
	}
	pid_t pid;
	// 创建一个进程扇
	for (int i = 0; i < 2; ++i)
	{
		pid = fork();
		if (pid < 0)
		{
			perror("fork error");
			exit(1);
		} 
		else if (pid == 0)	// child process
		{
			if (i == 0)	// 第一个子进程，负责写入数据
			{
				// 关闭管道读取端口
				close(fd[0]);
				/*
				 * 将标准输出重定向到管道的写端
				 */
				if (dup2(fd[1], STDOUT_FILENO) != STDOUT_FILENO)
				{
					perror("dup2 error");
				}
				close(fd[1]);
				// 调用exec函数执行cat命令
				if (execvp(cmd1[0], cmd1) < 0)
				{
					perror("execvp error");
					exit(1);
				}
				break;
			}
			if (i == 1)	// 第二个子进程，负责读取数据
			{
				sleep(3);
				// 关闭管道写入端口
				close(fd[1]);
				// 将标准输入重定向到管道的读取
				if (dup2(fd[0], STDIN_FILENO) != STDIN_FILENO)
				{
					perror("dup2 error");
				}
				close(fd[0]);
				// 调用exec函数执行grep命令
				if (execvp(cmd2[0], cmd2) < 0)
				{
					perror("execvp error");
				}
				break;
			}
		}
		else	// parent process
		{
			if (i == 1)	// i == 1 说明创建了两个子进程，进程扇
			{
				close(fd[0]);
				close(fd[1]);
				wait(0);
				wait(0);
			}
		}
	}
	exit(0);
}