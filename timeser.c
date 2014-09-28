#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<sys/socket.h>

#include <stdio.h>
#include <sys/types.h>	/* basic system data types */

#include <sys/time.h>	/* timeval{} for select() */
#include <time.h>		/* timespec{} for pselect() */
#define	MAXLINE		4096	/* max text line length */

#define	LISTEN		1024

void err_handling(char* message);
int main(int argc, char **argv)
{
    int sockfd, connfd;
    struct sockaddr_in servaddr;
    char buff[MAXLINE];
    time_t t;
    sockfd = socket(PF_INET, SOCK_STREAM, 0);

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(9997);
 
    if(bind(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) == -1)
	err_handling("bind() error");

    if(listen(sockfd, LISTEN) == -1)
	err_handling("listen() error");

    connfd = accept(sockfd, (struct sockaddr*)NULL, NULL);
    for(;;)
    {      
	
	t = time(NULL);
	snprintf(buff, sizeof(buff), "%.24s\r\n", ctime(&t));
	write(connfd, buff, strlen(buff));
	sleep(5);
    }
    close(connfd);

}

void err_handling(char* message)
{
    printf(message, stderr);
    printf("\n", stderr);
    exit(1);
}
