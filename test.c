#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>

int main(void){
    execlp("xterm","xterm","-e","./echocli","127.0.0.1","4",(char*) 0);
    exit(0);
}
