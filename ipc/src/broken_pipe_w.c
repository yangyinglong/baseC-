#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <errno.h>


/**
  *	不完整管道：写一个读端已经被关闭的管道
  */

void sig_handler(int signo)
{
	if (signo == SIGPIPE)
	{
		printf("SIGPIPE occured\n");
	}
}

int main(int argc, char const *argv[])
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
		close(fd[0]);
		if (signal(SIGPIPE, sig_handler) == SIG_ERR)
		{
			perror("signal  sigpipe error");
			exit(1);
		}
		
		char *s  = "1234";
		if (write(fd[1], s, sizeof(s)) != sizeof(s))
		{
			fprintf(stderr, "%s, %s\n", strerror(errno),
				(errno == EPIPE) ? "EPIPE" : "unkonw");
		}
		close(fd[1]);
		wait(0);
	}
	else	// child process
	{
		close(fd[0]);
		close(fd[1]);
	}

	exit(0);
}