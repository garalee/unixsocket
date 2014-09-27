#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<sys/socket.h>

#include<errno.h>

#define ECHO_PORT 9998
#define MAX_SIZE 1024

int main(int argc, char *argv[])
{
    int sockfd;
    char message[MAX_SIZE];
    int str_len;
    struct sockaddr_in server_addr;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd == -1){
	printf("socket error : %s\n",strerror(errno));
    }
  
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(ECHO_PORT);
    inet_pton(AF_INET,argv[1],&server_addr.sin_addr);

    if(connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1)
    {
	printf(" Connection Error : %s\n",strerror(errno));
    }


    /* ****************************************************** */
    /* ****************Echo Service ************************* */
    /* ****************************************************** */
    /* TODO in Echo Service
     * Close socket normally when EOF requested
     * Handler that handles reading data from server more than it can recieve.
     * Sending Message that recieved from server to its parent process(Need PIPE). 
     * 
     */
    while(1)
    {
	printf("command(case-sensitive): ");
	fgets(message,MAX_SIZE,stdin);

	if(feof(message)){
	    printf("Sent EOF\n");
	    break;
	}

	write(sockfd, message, strlen(message));
	str_len = read(sockfd, message, MAX_SIZE);
	message[str_len] = 0;
	printf("Message from server: %s\n", message);
    }
    close(sockfd);
    return 0;
}
