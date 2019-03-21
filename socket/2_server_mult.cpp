#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <signal.h>
#include <sys/wait.h>
 
#define PORT 9990
#define SIZE 1024
 
int creat_socket()         //创建套接字和初始化以及监听函数
{
	int listen_socket = socket(AF_INET, SOCK_STREAM, 0);      //创建一个负责监听的套接字  
	if(listen_socket == -1)
	{
		perror("socket");
		return -1;
	}
	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	
	addr.sin_family = AF_INET;  /* Internet地址族 */
    addr.sin_port = htons(PORT);  /* 端口号 */
    addr.sin_addr.s_addr = htonl(INADDR_ANY);   /* IP地址 */
	
	int ret = bind(listen_socket, (struct sockaddr *)&addr, sizeof(addr));    //连接
	if(ret == -1)
	{
		perror("bind");
		return -1;
	}
	
	ret = listen(listen_socket, 5);   //监听
	if(ret == -1)
	{
		perror("listen");
		return -1;
	}
	return listen_socket;
}
 
int wait_client(int listen_socket)
{
	struct sockaddr_in cliaddr;
	socklen_t addrlen;
	printf("等待客户端连接。。。。\n");
	int client_socket = accept(listen_socket, (struct sockaddr *)&cliaddr, &addrlen);     //创建一个和客户端交流的套接字
	if(client_socket == -1)
	{
		perror("accept");
		return -1;
	}
	
	printf("成功接收到一个客户端：%s\n", inet_ntoa(cliaddr.sin_addr));
	
	return client_socket;
}
 
void hanld_client(int listen_socket, int client_socket)    //信息处理函数,功能是将客户端传过来的小写字母转化为大写字母
{
	char buf[SIZE];
	while(1)
	{
		int ret = read(client_socket, buf, SIZE-1);
		if(ret == -1)
		{
			perror("read");
			break;
		}
		if(ret == 0)
		{
			break;
		}
		buf[ret] = '\0';
		int i;
		for(i = 0; i < ret; i++)
		{
			if (buf[i] >= 'a' && buf[i] <= 'z')
			{
				buf[i] = buf[i] + 'A' - 'a';
			}
		}
		
		printf("%s\n", buf);
		write(client_socket, buf, ret);
		
		if(strncmp(buf, "end", 3) == 0)
		{
			break;
		}
	}
	close(client_socket);
}
 
void handler(int sig)
{
	
	while (waitpid(-1,  NULL,   WNOHANG) > 0)
	{
		printf ("成功处理一个子进程的退出\n");
	}
}
 
int main()
{
	int listen_socket = creat_socket();
	
 
	signal(SIGCHLD,  handler);    //处理子进程，防止僵尸进程的产生
	while(1)
	{
		int client_socket = wait_client(listen_socket);   //多进程服务器，可以创建子进程来处理，父进程负责监听。
		// 每次有客户端请求连接之后，创建一个子进程，复制父进程中的所有资源，
		// 包括已经建立连接的socket和正在监听的socket，
		// 这个时候，父进程和子进程中都有同一个客户端的连接socket和监听socket，
		// 然后在父进程中，关闭客户端socket，在子进程中，关闭监听socket
		int pid = fork();
		if(pid == -1)
		{
			perror("fork");
			break;
		}
		if(pid > 0)
		{
			close(client_socket);
			continue;
		}
		if(pid == 0)
		{
			close(listen_socket);
			hanld_client(listen_socket, client_socket);
			break;
		}
	}
	
	close(listen_socket);
	
	return 0;
}