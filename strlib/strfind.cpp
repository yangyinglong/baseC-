#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main()
{
	char *s1 = "Happy New Year!";
	char *s2 = "Year";
	char *s3 = "pye HwNa";

	char *p;

	p = strchr(s1, 'a');
	printf("在字符串%s中首次出现字母'a'的位置是:", s1);
	p != NULL ? puts(p) : puts("无");

	p = strrchr(s1, 'a');
	printf("在字符串%s中最后一次出现字母'a'的位置是:", s1);
	p != NULL ? puts(p) : puts("无");

	printf("在字符串%s从首字母开始不包括字符串%s所含字符的分段长度是:%d\n", s1, s2, strcspn(s1, s2));


	return 0;
}