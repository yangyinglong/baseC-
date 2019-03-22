#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <pthread.h>
 
 
#define PORT 9990
#define SIZE 1024

// gcc 2_server_multithread.cpp -o 2_server_multithread.o -lpthread

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
 
void* hanld_client(void * client)    //信息处理函数,功能是将客户端传过来的小写字母转化为大写字母
{
	int client_socket = (int&)client;
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
				continue;
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
 
int main()
{
	int listen_socket = creat_socket();
	
	while(1)
	{
		int client_socket = wait_client(listen_socket);
		
		pthread_t id;
		pthread_create(&id, NULL, hanld_client, (void*)(client_socket));  //创建一个线程，来处理客户端。
		
		pthread_detach(id);   //把线程分离出去。
	}
	
	close(listen_socket);
	
	return 0;
}