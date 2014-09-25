#include<netinet/in.h>
#include<stdio.h>
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>

#include<unistd.h>
#include<stdlib.h>

#define SERVER_PORT 9999

#define MAXLINE 1024
void str_cli(FILE* fp,int sockfd);

int main(int argc,char* argv[]){
    int fd;
    struct sockaddr_in server_addr;

    if( argc != 2){
	printf("Usage : tcpcli <IP Address>\n");
	exit(0);
    }

    fd = socket(AF_INET,SOCK_STREAM,0);
    bzero(&server_addr,sizeof(struct sockaddr_in));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET,argv[1],&server_addr.sin_addr);

    if( connect(fd, (struct sockaddr*)&server_addr,sizeof(server_addr)) < 0){
	printf ("Connection Error \n");
	exit(0);
    }

    str_cli(stdin,fd);
    return 0;
}


void str_cli(FILE* fp,int sockfd){
    char sendline[MAXLINE],recvline[MAXLINE];

    while(fgets(sendline,MAXLINE,fp) != NULL){
	write(sockfd,sendline,strlen(sendline));

	if(read(sockfd,recvline,MAXLINE) == 0){
	    printf("str_cli: server terminated prematurely");
	    return;
	}

	fputs(recvline,stdout);
    }
}
