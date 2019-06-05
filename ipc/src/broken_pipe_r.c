#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/**
  * 不完整管道：读取一个写端关闭的管道
  */

int	main(int argc, char const *argv[])
{
	int fd[2];
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
		sleep(5);
		close(fd[1]);
		while(1)
		{
			char c;
			if (read(fd[0], &c, 1) == 0)
			{
				printf("\nwrite-end of pipe closed\n");
				break;
			}
			else
			{
				printf("%c", c);
			}
		}
		close(fd[0]);
		wait(0);
	}
	else	// child process
	{
		close(fd[0]);
		char *s  = "1235";
		write(fd[1], s, sizeof(s));
		close(fd[1]);
	}
	exit(0);
}