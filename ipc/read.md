ipc 进程间通讯

管道
1. src/cal_pipe.c 			父子进程通过管道进行数据通讯
2. src/cmd_pipe.c 			父进程创建一个进程扇，两个子进程执行cmd命令
							标准输入输出重定向到管道读端和写端
3. src/add.c 				从标准输入读取数据，将结果输出到标准输入
   src/co_process.c 		创建两个管道，进行双向通讯
4. src/break_pipe_r.c 		读取一个写端关闭的管道
5. src/break_pipe_w.c 		写入一个读端关闭的管道
7. src/popen_rw.c 			标准库中的管道操作
8. src/fifo_read.c 			命名管道的读
9. src/fifo_write.c 		命名管道的写

消息队列
10. src/msq_snd.c 			消息队列的创建和发送消息数据
11. src/msq_rcv.c 			消息队列接受消息

共享内存
12. include/tell.h 			定义通过管道来通知进程读写共享内存
13. src/tell.c 				实现tell.h
14. cal_shm.c 				父进程往共享内存中写入数据，通过管道通知子进程读取数据

15. include/account.h 		定义银行账户结构体和对账户的基本取款存款查看操作
16. src/account.c 			实现account.h

信号量
17. include/pv.h 			信号量的创建、PV操作、销毁定义
18. src/pv.c 				实现pv.h
19. include/account_sem.h 	银行账户绑定信号量集的操作
20. src/account_sem.c 		
21. src/account_test.c 		银行账户操作的main函数

22. src/reader_writer.c 	通过信号量来控制多个进程对共享内存的操作




管道的读写特性：
1. 通过打开两个管道来创建一个双向的管道
2. 管道是阻塞性的，当进程从管道中读取数据，若没有数据进程会阻塞
3. 当一个进程往管道中不断地写入数据但没有进程读取数据，此时只要管道没有满是可以的，但是管道放满数据时则报错
4. 当读一个写端已经被关闭的管道时，在所有的数据被读取后，read()返回0，以表示到达了文件尾部
5. 如果写入一个读端已被关闭的管道，则产生信号SIGPIPE，如果忽略该信号或捕捉该信号并从处理程序返回，则write返回-1，同时errno设置为EPIPE

6. 标准库中的管道操作
	FILE *popen(const char * cmdstring, const char * type);
	int pclose(FILE * fp);


7. 命名管道
	只要有权限，FIFO可用于任何两个没有热河关系的进程之间通信
	本质上是内核中的一块缓存，另在文件系统中以一个特殊的设备we年（管道文件）存在
	在文件系统中只有一个索引块存放文件的路径，没有数据块，所有数据存放在内核中。
	命名管道必须读和写同时打开，否则单独读或者写会引发阻塞
	管道文件用命令mkfifo创建或者调用mkfifo函数


8. System V 引入了三种高级进程间通信机制
	消息队列、共享内存、信号量
	这三种通信机制的对象（IPC对象-进程间通信）存在于内核中，而不是文件系统中，由用户控制释放，不像管道的释放，由内核控制
	IPC对象通过其标识符来引用和访问，所有的IPC对象在内核空间中有唯一性标识ID，在用户空间中的唯一标识成为Key
	Linux IPC继承自System V IPC
	IPC 对象是全局对象，可以用ipcs/ipcrm查看和删除
		ipcs -m 单独查看
		ipcs -q
		ipcs -s
		ipcrm key 删除
	用msgget/shmget/semget创建，创建时必须指定关键字key

9. IPC对象的权限和所有者结构体
struct ipc_perm{
	uid_t uid;	// 拥有者的用户Id
	gid_t gid;	// 所在组的id
	uid_t cuig;	// 创建
	gid_t cgid;
	mode_t mode;	// 访问权限
	.....
}

10. 消息队列
	1）消息队列是内核中的一个链表
	2）用户进程将数据传输到内核后，内核重新添加一些如用户id，组id，读写进程的id和优先级等相关信息后并打成一个数据包成为消息
	3）允许一个或者多个进程往消息队列中读取消息，但是一个消息只能被一个读取一次，读取之后就自动删除
	4）消息队列具有一定的FIFO的特性，消息可以按照顺序发送到队列中，也可以用几种不同的方式从队列中读取
	5）每个消息队列在内核中用一个唯一的IPC标识ID表示
	6）消息队列的实现包括创建和打开队列、发送消息、读取消息和控制控制消息队列四种操作

11. 消息队列属性
struct msqid_ds{
	struct ipc_perm msg_perm;	//IPC对象的权限和所有者结构体属性
	msgqnum_t msg_qnum;	// 消息队列中消息的数量
	msglen_t msg_qbytes;	// 消息队列中最大消息的字节数
	pid_t msg_lspid;	// 最后一次发送消息的进程pid
	pid_t msg_lrpid;	// 最后一次读取消息的进程pid
	time_t msg_stime;	// 最后一次发送消息的时间
	time_t msg_ctime;	// 最后一次改变消息队列的时间
}

12. 打开或者创建消息队列
	#include <sys/msg.h>
	int msgget(key_t key, int flag);
	key: 用户指定的消息队列键值
	flag: IPC_CREAT(创建)、IPC_EXCL(和IPC_CREAT一起使用时，当已经存在队列时，则返回失败（防止关键字重复）)

13. 消息队列的控制
	int msgctl(int msgid, int cmd, struct msqid_ds *buf);
	msgid: 消息队列Id
	buf: 消息队列属性指针
	cmd: 
		1) IPC_STAT 获取消息队列的属性，并将其放在buf指向的结构中
		2) IPC_SET 设置属性，按由buf指向的结构体中的值，设置与此队列相关的结构体中的字段
		3）IPC_RMID 删除队列

14. 发送消息
	int msgsnd(int msgqid, const void *prt, size_t nbytes, int flag);
	msgqid: 消息队列Id
	prt: 消息
		struct mymesg{
			long mtype;	//消息的类型
			char mtext[512]; // 消息
		}
	nbytes: 消息大小，不包括mtype的大小, sizeof(mymesg)-sizeof(long)
	mtype: 指定消息的类型，整数，必须大于0
	mtext: 消息数据本身
	在Linux中，消息最大的长度是4056个字节，其中包括mtype, 占4个字节
	mymesg用户可以自定义，但是第一个成员必须是mtype
	flag: 
		0: 阻塞，若消息队列满，则阻塞到空间可容纳要发送的消息，或者从系统中删除了此队列，或捕捉到一个信号，并从信号处理程序返回
		IPC_NOWAIT: 非阻塞，若消息队列满，则出错

15. 接受消息
	ssize_t msgrcv(int msgqid, void *prt, size_t nbytes, long type, int flag);
	type: 消息类型 type=0，获得消息队列中的第一个消息，type>0获得消息队列中类型为type的第一个消息，type<0,获得消息队列中小于或者等于type绝对值的消息(类型最小的)