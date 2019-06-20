#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

/**
 * 标准库中的管道操作
 */
int main(int argc, char const *argv[])
{
	FILE *fp1, *fp2;
	// 命令执行的结果放置在fp指向的结构体缓存中
	// type = "r"
	// 1. 子进程执行 exec(cmd)，
	// 将标准输出重定向到管道的写端，
	// 将命令执行的结果写入到管道中
	// 2. 父进程从管道中读取命令执行的结果，
	// 并将其放置到FILE*类型的文件指针指向的结构体缓存中
	fp1 = popen("cat /etc/passwd", "r");
	// type = "w"
	// 1. 将标准输入重定向到管道的读端，
	// 2. 父进程将文件结构体缓存中的数据写入到管道
	// 3. 子进程从管道中读取数据作为命令执行的输入
	fp2 = popen("wc -l", "w");
	char buff[512];
	memset(buff, 0, sizeof(buff));
	while (fgets(buff, sizeof(buff), fp1) != NULL)
	{
		printf("%s", buff);
		fprintf(fp2, buff);
	}
	pclose(fp1);
	pclose(fp2);
	exit(0);
}