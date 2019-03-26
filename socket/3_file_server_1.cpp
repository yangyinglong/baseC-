#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <errno.h>

#define PORT 9999
#define SIZE 1024

int create_socket()
{
	int listen_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_socket == -1)
	{
		 perror("create socket error!");
		 return -1;
	}
	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(PORT);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);

	int ret = bind(listen_socket, (struct sockaddr *)&addr, sizeof(addr));
	if (ret == -1)
	{
		perror("bind error!");
		return -1;
	}

	ret = listen(listen_socket, 5);
	if (ret == -1)
	{
		perror("listen error!");
		return -1;
	}
	return listen_socket;
}

int wait_client(int & listen_socket)
{
	struct sockaddr_in cliaddr;
	socklen_t addrlen;
	printf("等待客户端连接......\n");
	int client_socket = accept(listen_socket, (struct sockaddr *)&cliaddr, &addrlen);
	if (client_socket == -1)
	{
		perror("accept error!");
		// close(listen_socket);
		return -1;
	}
	printf("成功连接一个客户端\n");
	return client_socket;
}

void hanld_client(int & client_socket, const char * path) 
{
	FILE * fp = fopen(path, "r");
	if (fp == NULL)
	{
		perror("file open error");
		close(client_socket);
		exit(1);
	}

	fseek(fp, 0, SEEK_END);
	unsigned int fileSize = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	char buf[10];
	unsigned char fileBuf[SIZE];

	if (write(client_socket, (unsigned char *)&fileSize, 4) != 4)
	{
		perror("sent file size to client error!");
		close(client_socket);
		exit(1);
	}
	if (read(client_socket, buf, 2) != 2)
	{
		perror("read mess from clinet error!");
		close(client_socket);
		exit(1);	
	}

	int size = 0, netSize = 0;
	while((size = fread(fileBuf, 1, SIZE, fp)) > 0) 
	{
		unsigned int size2 = 0;
		while(size2 < size)
		{
			if ((netSize = write(client_socket, fileBuf+size2, size-size2)) < 0)
			{
				perror("send file to client error!");
				close(client_socket);
				exit(1);
			}
			size2 += netSize;
		}
	}
	close(client_socket);
	fclose(fp);
}

int main()
{
	int listen_socket = create_socket();
	int client_socket = wait_client(listen_socket);
	hanld_client(client_socket, "TCP.jpeg");
	close(listen_socket);
	return 0;
}