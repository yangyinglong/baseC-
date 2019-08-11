/**
 * 功能：	由一个父进程fork出两个子进程（进程扇）进行数据传递
 * 输出：	yangyl@ubuntu:~/projects/cpp/baseC-/ipc$ ./bin/process_pipe 
 *			parent process pid: 1528
 *			frist child process pid: 1529 write 0
 *			second child process pid: 1530 read 0
 *			frist child process pid: 1529 write 1
 *			second child process pid: 1530 read 1
 *			frist child process pid: 1529 write 2
 *			second child process pid: 1530 read 2
 *			frist child process pid: 1529 write 3
 *			second child process pid: 1530 read 3
 */

#include <stdio.h>
#include <unistd.h>

int main(void)
{
	pid_t pid;
	int fd[2];

	// 创建管道
	if (pipe(fd) < 0)
	{
		perror("pipe error");
		exit(1);
	}

	// 创建进程扇
	for (int i = 0; i < 2; ++i)
	{
		pid = fork();
		if (pid < 0)
		{
			perror("fork error");
			exit(1);
		}
		else if(0 == pid)
		{
			// child process
			if (0 == i)
			{
				// frist child process: write data
				// 关闭管道的读端
				close(fd[0]);
				for (int j = 0; j < 4; ++j)
				{
					if (write(fd[1], &j, sizeof(int)) != sizeof(int))
					{
						perror("write error");
						exit(1);
					}
					printf("frist child process pid: %d write %d\n", getpid(), j);
				}
				close(fd[1]);
				break;
			}
			if (1 == i)
			{
				// second child process: read data
				// 关闭管道的写端
				close(fd[1]);
				int num = -1;
				for (int j = 0; j < 4; ++j)
				{
					if (read(fd[0], &num, sizeof(int)) < sizeof(int))
					{
						perror("read error");
						exit(1);
					}
					printf("second child process pid: %d read %d\n", getpid(), num);
				}
				

				close(fd[0]);
				break;
			}
		}
		else
		{
			// parent process
			if (1 == i)
			{
				close(fd[0]);
				close(fd[1]);
				printf("parent process pid: %d\n", getpid());
				wait(0);
				wait(0);
			}		
		}
	}

	exit(0);
}