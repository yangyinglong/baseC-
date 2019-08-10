#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <memory.h>
#include <unistd.h>

/**
 * 必须先创建管道 mkfifo s.pipe
 * 命名管道的写
 */
int main(int argc, char const *argv[])
{
	if (argc < 2)
	{
		printf("usage:%s fifo\n", argv[0]);
		exit(1);
	}
	printf("open fifo write...\n");
	int fd = open(argv[1], O_WRONLY);
	if (fd < 0)
	{
		perror("open error");
		exit(1);
	}
	else
	{
		printf("open file success: %d\n", fd);
	}

	// 从命名管道中写入数据
	char *s = "1234567890";
	size_t size = strlen(s);
	if (write(fd, s, size) != size)
	{
		perror("write error");
	}
	close(fd);
	exit(0);
}