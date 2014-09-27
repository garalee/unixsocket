#include<stdio.h>
#include<netinet/in.h>
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>

#include<unistd.h>
#include<stdlib.h>
#include<pthread.h>
#include<netdb.h>
#include<arpa/inet.h>
#include<errno.h>

#define SERVER_PORT 9999
#define MAX_LINE 1024

#define ECHO_PORT 9998
#define TIME_PORT 9997

void printHost(char* argv);


int main(int argc,char* argv[]){
    int fd;

    struct sockaddr_in server_addr; 
    char buf[MAX_LINE];
    char input[MAX_LINE];
    
    pid_t cp;

    /* Error Check the Number of Arguments */
    if( argc != 2 ){
	printf("Usage: client <hostname or ip address>\n");
	exit(0);
    }

    printHost(argv[1]);		/* Print Out Peer Information */
    

    /* Socket Initialization */
    fd = socket(AF_INET,SOCK_STREAM,0);

    if(fd < 0){
	printf("Socket Error : %s\n",strerror(errno));
    }
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = SERVER_PORT;
    inet_pton(AF_INET,argv[1],&server_addr.sin_addr);

    if(connect(fd,(struct sockaddr*)&server_addr,sizeof(server_addr)) < 0){
	printf("Connection Error : %s\n",strerror(errno));
    }

    while(1){
	printf("command(case-sensitive), \"quit\" for quit:"); // Command line
	fgets(input,MAX_LINE,stdin); /* Blocking IO Model */
	if(strcmp("echo\n",input) == 0){
	    cp = fork();
	    if(cp == 0){	// Child Process
		close(fd);
		execlp("xterm","xterm","-e","./echocli",host,(char*) 0);
	    }else{
		wait();		// Handling SIGCHD signal
	    }
	}else if(strcmp("time\n",input) == 0){
	    cp = fork();
	    if(cp == 0){
		close(fd);
		execlp("xterm","xterm","-e","./timecli",host,(char*)0);
	    }else{
		wait();
	    }
	}else if(strcmp("quit\n",input) == 0){
	    /* Error Handler Required
	     * Expected Error : trying to close socket while few other process is still running
	     *                  trying to close while server is closed already
	     */
	    close(fd);
	}else{
	    char *p = strtok(input,"\n");
	    printf("%s is not valid command\n",p);
	}
    }
	

    printf("Program Exited Normally\n");

    return 0;
}



void printHost(char* host){
    in_addr_t addr;
    struct hostent *hp;
    struct in_addr in_addr;

    char** pptr;
    
    if((int)(addr = inet_addr(host)) == -1){
	hp = gethostbyname(host);
	if( hp != NULL){
	    //in_addr = *(struct in_addr*)(hp->h_addr);
	    printf("Official Host Name :  %s\n",hp->h_name);
	    for(pptr = hp->h_addr_list; *pptr != NULL ; ++pptr){
		memcpy(&in_addr.s_addr,*pptr,sizeof(in_addr.s_addr));
		printf("%s, was resolved to : %s\n",host,inet_ntoa(in_addr));
	    }
	}else{
	    printf("%s was not resolved\n",host);
	}
    }else{
	inet_pton(AF_INET,host,&in_addr);
	hp = gethostbyaddr((void*)&in_addr,sizeof(in_addr),AF_INET);
	if ( hp != NULL){
	    printf("Official Host Name: %s\n",hp->h_name);
	    
	    for(pptr = hp->h_addr_list;*pptr!=NULL;++pptr){
		memcpy(&in_addr.s_addr,*pptr,sizeof(in_addr.s_addr));
		printf("%s\n",inet_ntoa(in_addr));
	    }
	}else{
	    printf("Host Information for %s not found\n",host);
	}
    } 
}
