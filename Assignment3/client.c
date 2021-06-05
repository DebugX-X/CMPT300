#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<netdb.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>



int setupClient(int myport){
    int sockfd;
    struct sockaddr_in clt;

    memset(&clt,0,sizeof(clt));

    clt.sin_family = AF_INET;
    clt.sin_addr.s_addr = htonl(INADDR_ANY);
    clt.sin_port = htons(myport);

    //printf("Establishing a client\n");
    if((sockfd = socket(PF_INET,SOCK_DGRAM,0)) < 0){
        perror("client setup failure\n");
        exit(EXIT_FAILURE);
    }
    return sockfd;
}