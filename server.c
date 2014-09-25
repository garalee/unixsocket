#include<netinet/in.h>
#include<stdio.h>
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<unistd.h>
#include<stdlib.h>
#define SERVER_PORT 9999

void init_server_addr(struct sockaddr_in * sa);

int main(void){
    int serverfd,clientfd;
    struct sockaddr_in server_addr,client_addr;
    int clilen = 0;

    serverfd = socket(AF_INET,SOCK_STREAM,0);
    
    init_server_addr(&server_addr);

    if (bind(serverfd,(struct sockaddr*)&server_addr,sizeof(struct sockaddr_in)) <0){
	printf("Bind Error \n");
	exit(-1);
    }

    listen(serverfd,1);

    while(1){
	clilen = sizeof(struct sockaddr_in);
	clientfd = accept(serverfd,(struct sockaddr*)&client_addr,&clilen);
	if( fork() == 0){	/* Child Process */
	    close(serverfd);
	    /*  */
	    exit(0);
	}
	close(clientfd);
    }
    return 0;
}


void init_server_addr(struct sockaddr_in * sa){
    bzero(sa,sizeof(struct sockaddr_in));
    sa->sin_family = AF_INET;
    sa->sin_addr.s_addr = htonl(INADDR_ANY);
    sa->sin_port = htons(SERVER_PORT);
}
