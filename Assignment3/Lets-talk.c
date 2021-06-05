#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h> //For Sockets
#include <string.h>
#include <netinet/in.h> //For the AF_INET (Address Family)
#include <netdb.h>
#include <arpa/inet.h>
#include <stddef.h>
#include "list.h"
#include "server.h"
#include "client.h"

//global variables
int port,rport;
char *name;
//support for !status
char *ping,*rping;
//Lists
List *reclist, *sendlist;
//stores last message's time
//ping
int callstat = 0;

//mutex lock and condition for receiver list
pthread_cond_t recCond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t recMutex = PTHREAD_MUTEX_INITIALIZER;
//mutex lock and condition for Sender list
pthread_cond_t sendCond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t sendMutex = PTHREAD_MUTEX_INITIALIZER;

//supports 4k+characters
#define buflen 4096
// global threads
pthread_t keyboard, sender, receiver, printer;

//encryption key
int const key = 301386847;

void encryptor(char *text){
    int i = 0;
    while(text[i] != '\0'){
        text[i] = (char)((text[i]+key)%256);
        i++;
    }
}

void decryptor(char *text){
    int i = 0;
    while(text[i] != '\0'){
        text[i] = (char)((text[i]-key)%256);
        i++;
    }
}

void *serverThread() {
    int sockfd = setupServer(port);
    struct sockaddr_in sevrem;
    memset(&sevrem, 0, sizeof(sevrem));
    struct in_addr ipAdd;

    //Checks if argv[2] is a ip address or hostname
    if(inet_pton(AF_INET,name,&(sevrem.sin_addr))==1){
        if(inet_aton(name,&ipAdd)==0) {
            fprintf(stderr, "Invalid IP address\n");
            exit(EXIT_FAILURE);
        }
    }else{
        struct hostent *mach;
        if ((mach = gethostbyname(name)) == NULL) {
            perror("unknown machine\n");
            exit(EXIT_FAILURE);
        }
        ipAdd = *(struct in_addr *) mach->h_addr_list[0];
    }

    //remote server info
    sevrem.sin_family = AF_INET;
    sevrem.sin_addr = ipAdd;
    sevrem.sin_port = htons(rport);

    unsigned int size = sizeof(sevrem);

    while (1) {
        char *buffer = malloc(sizeof(char) * buflen);
        memset(buffer, 0, sizeof(char) * buflen);
        int n = recvfrom(sockfd, buffer, buflen, MSG_WAITALL, (struct sockaddr *) &sevrem, &size);
        if (n < 0) {
            perror("Message not received\n");
            exit(EXIT_FAILURE);
        } else if (n > 0) {
            fflush(stdout);
        }
        buffer[n] = '\0';

        //decrypting the message before adding to the list
        decryptor(buffer);
        if(strcmp(buffer,rping)==0) {
            int csock = setupClient(port);
            struct sockaddr_in clirem;
            memset(&clirem, 0, sizeof(clirem));

            clirem.sin_family = AF_INET;
            clirem.sin_addr = ipAdd;
            clirem.sin_port = htons(rport);
            encryptor(buffer);
            int rem = sendto(csock, buffer, strlen(buffer), MSG_CONFIRM, (struct sockaddr *) &clirem, size);
            if (rem == -1) {
                perror("Error sending message\n");
                exit(EXIT_FAILURE);
            }
        } else if(strcmp(buffer,ping)==0){
            callstat = 0;
            printf(">user is online\n");
        } else {
            pthread_mutex_lock(&recMutex);
            int rec = List_append(reclist, (void *) buffer);
            if (rec != -1) {
                pthread_cond_signal(&recCond);
            } else {
                perror("Message error(server)\n");
                exit(EXIT_FAILURE);
            }
            pthread_mutex_unlock(&recMutex);
        }
        //free(buffer);
    }
    return NULL;
}

void *clientThread() {
    int sockfd = setupClient(port);
    struct sockaddr_in clirem;
    memset(&clirem, 0, sizeof(clirem));
    struct in_addr ipAdd;

    //Checks if argv[2] is a ip address or hostname
    if(inet_pton(AF_INET,name,&(clirem.sin_addr))==1){
        if(inet_aton(name,&ipAdd)==0) {
            fprintf(stderr, "Invalid address\n");
            exit(EXIT_FAILURE);
        }
    }else{
        struct hostent *mach;
        if ((mach = gethostbyname(name)) == NULL) {
            perror("unknown machine\n");
            exit(EXIT_FAILURE);
        }
        ipAdd = *(struct in_addr *) mach->h_addr_list[0];
    }

    clirem.sin_family = AF_INET;
    clirem.sin_addr = ipAdd;
    clirem.sin_port = htons(rport);

    while (1) {
        unsigned int size = sizeof(clirem);
        pthread_mutex_lock(&sendMutex);
        pthread_cond_wait(&sendCond,&sendMutex);
        if (List_count(sendlist) != 0) {
            char *buffer = (char *) List_remove(sendlist);
            if(strcmp(buffer,"!status")==0){
                strcpy(buffer,ping);
                callstat = 1;
            }
            //encrypt before sending
            encryptor(buffer);
            int rem = sendto(sockfd, buffer, strlen(buffer), MSG_CONFIRM, (struct sockaddr *) &clirem, size);
            if (rem == -1) {
                perror("Error sending message\n");
                exit(EXIT_FAILURE);
            }
            //quick decrypt to check if sent message was a exit signal
            decryptor(buffer);
            //support for !exit
            if (strcmp(buffer, "!exit") == 0) {
                pthread_cancel(keyboard);
                pthread_cancel(printer);
                pthread_cancel(receiver);
                pthread_cancel(sender);
                return NULL;
            }
        }
        pthread_mutex_unlock(&sendMutex);
        sleep(2);
        if(callstat == 1){
            printf(">user is offline\n");
        }
    }
    return NULL;
}

void *printThread(){
    while(1) {
        pthread_mutex_lock(&recMutex);
        pthread_cond_wait(&recCond, &recMutex);
        if (List_count(reclist)!=0){
            char *buffer = (char*) List_remove(reclist);

            //support for !exit
            if(strcmp(buffer,"!exit")==0){
                printf("Received exit signal from %s\n",name);
                pthread_cancel(keyboard);
                pthread_cancel(printer);
                pthread_cancel(sender);
                pthread_cancel(receiver);
                return NULL;
            }
            printf(">%s\n",buffer);
            free(buffer);
        }
        pthread_mutex_unlock(&recMutex);
    }
    return NULL;
}

void getinput(char* text) {
    int size = 0;
    int buffsize = buflen;
    char *buf = malloc(sizeof(char)*buflen);
    while (1){
    char ch = getc(stdin);
    if (ch == EOF) {
        break;
    }
    if (ch == '\n') {
        sleep(1);
        if(ftell(stdin) < 0){
            break;
        }
    }
    if(size >= buflen) {
        buffsize += 1024;
        buf = realloc(buf, buffsize);
    }
    buf[size++] = ch;
    }
    buf[size] ='\0';
    strcpy(text,buf);
    free(buf);
}

void *keyboardThread(){
    while(1) {
        char buffer[buflen];
        getinput(buffer);
        pthread_mutex_lock(&sendMutex);
        int app = List_append(sendlist, (void *) &buffer);
        if (app != -1) {
            pthread_cond_signal(&sendCond);
        } else {
            perror("Message error(main)\n");
            exit(1);
        }
        pthread_mutex_unlock(&sendMutex);
    }
    return NULL;
}

int main (int argc, char *argv[]){

  //Error checking number of cmdline args
  if (argc <= 3){
      printf("Missing Arguments\n");
      exit(0);
  }else if (argc >= 5){
      printf("Extra Arguments\n");
      exit(0);
  }
  port = atoi(argv[1]);
  rport = atoi(argv[3]);
  name = argv[2];

  char st[10] = "ping+";
  strcat(st,argv[1]);
  ping = st;
  char str[10] = "ping+";
  strcat(str,argv[3]);
  rping = str;

  reclist = List_create();
  sendlist = List_create();

  pthread_create(&receiver,NULL,serverThread,NULL);
  pthread_create(&printer,NULL,printThread,NULL);
  pthread_create(&sender,NULL,clientThread,NULL);
  pthread_create(&keyboard,NULL,keyboardThread,NULL);
  printf("Welcome to Lets-talk! Please type your message now\n");
  pthread_join(receiver,NULL);
  pthread_join(printer,NULL);
  pthread_join(sender,NULL);
  pthread_join(keyboard,NULL);

  List_free(sendlist,NULL);
  List_free(reclist,NULL);
  printf("Bye!\n");
  return 0;
}