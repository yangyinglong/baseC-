// 遍历目录以及目录的子目录

#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>

void scan_dir(char *dir, int depth)
{
	DIR *dp;
	struct dirent *entry;
	struct stat statbuf;
	if ((dp = opendir(dir)) == NULL)
	{
		puts("无法打开该目录");
		return;
	}
	chdir(dir);
	while ((entry = readdir(dp)) != NULL) 
	{
		lstat(entry->d_name, &statbuf);
		if (S_IFDIR & statbuf.st_mode)
		{
			if (strcmp(".", entry->d_name) == 0 || strcmp("..", entry->d_name) == 0)
			{
				continue;
			}
			printf("%*s%s/\n", depth, "", entry->d_name );
			scan_dir(entry->d_name, depth+4);
		}
		else
		{
			printf("%*s%s\n", depth, "", entry->d_name);
		}
	}
	chdir("..");
	closedir(dp);
}

int main(int argc, char const *argv[])
{
	puts("开始扫面!");
	scan_dir("/home/yangyl/projects/c/baseC-", 0);
	puts("扫描结束!");
	return 0;
}