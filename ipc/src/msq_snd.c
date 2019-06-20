#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/msg.h>

typedef struct
{
	long type;	//消息类型
	int start;	//消息数据本身(包括start和end)
	int end;	
}MSG;

/**
 * 往消息队列中发送消息
 */
int main(int argc, char const *argv[])
{
	if (argc)
	{
		printf("usage: %s key\n", argv[0]);
		exit(1);
	}

	key_t key = atoi(argv[1]);
	printf("key: %d\n", key);

	// 创建消息队列
	int msq_id;
	if ((msq_id = msgget(key, 
						IPC_CREAT|IPC_EXCL|0777)) < 0)
	{
		perror("msgget error");
		exit(1);
	}
	printf("msq id: %d\n", msq_id);

	// 定义要发送的消息
	MSG m1 = {4, 4. 401};
	MSG m2 = {2, 2. 402};
	MSG m3 = {1, 1. 402};
	MSG m4 = {6, 6. 403};
	MSG m5 = {6, 66. 404};

	// 发送消息到消息队列
	if (msgsnd(msq_id, &m1, sizeof(MSG)-sizeof(long), IPC_NOWAIT) < 0)
	{
		perror("msgsnd error");
		exit(1);
	}
	if (msgsnd(msq_id, &m2, sizeof(MSG)-sizeof(long), IPC_NOWAIT) < 0)
	{
		perror("msgsnd error");
		exit(1);
	}
	if (msgsnd(msq_id, &m3, sizeof(MSG)-sizeof(long), IPC_NOWAIT) < 0)
	{
		perror("msgsnd error");
		exit(1);
	}
	if (msgsnd(msq_id, &m4, sizeof(MSG)-sizeof(long), IPC_NOWAIT) < 0)
	{
		perror("msgsnd error");
		exit(1);
	}
	if (msgsnd(msq_id, &m5, sizeof(MSG)-sizeof(long), IPC_NOWAIT) < 0)
	{
		perror("msgsnd error");
		exit(1);
	}

	// 发送后获取消息队列中消息的总数,msqid_ds 用来放置消息队列的属性
	struct msqid_ds ds;
	if (msgctl(msq_id, IPC_STAT, &ds) < 0)
	{
		perror("msgctl error");
	}
	printf("msg total: %ld\n", ds.msg_qnum);

	exit(0);
}