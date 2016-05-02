//Jordan Lyon & Biggie Emmanuel Assignment pa4: Bank Server -- client
#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<netdb.h>
#include<unistd.h>
#include<errno.h>
#include<string.h>
#include<arpa/inet.h>
#include<regex.h>
#include<pthread.h>
#include<semaphore.h>

struct threadInfo {
    int port;
    struct hostent *hostServerIP;
    int fd;
};

void error(char *msg)
{
    //perror(msg);
    exit(0);
}

void *threadFunc(void *arg)
{
    struct threadInfo* data;
    int port = 0;
    int sockfd = -1;
    struct hostent *hostIP;
    int n = -1;

    data = (struct threadInfo*)arg;
    port = data->port;
    hostIP = data->hostServerIP;
    sockfd = data->fd;
    //establish new connection to listen for server response
    //struct sockaddr_in serverAddr;
    //int sock = -1;
    char recBuffer[256];

    /*
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock < 0)
    {
        error("ERROR creating thread-socket.");
    }
    bzero((char*) &serverAddr, sizeof(serverAddr));

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    bcopy((char *)hostIP->h_addr, (char *)&serverAddr.sin_addr.s_addr, hostIP->h_length);

    if(connect(sock, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
    {
        error("ERROR connecting thread to server");
    }
    */

    //THREAD CONNECTED....
    listen(sockfd, 5);

    bzero(recBuffer, 256);
    n = read(sockfd, recBuffer, 255);

    printf("Server Response: %s\n", recBuffer);

    if(n < 0)
    {
        error("ERROR reading from socket in thread");
    }

    return 0;
}

int main(int argc, char *argv[])
{
    int sockfd = -1;
    int portnum = -1;
    int n = -1;
    char buffer[256];
    struct sockaddr_in serverAddressInfo;
    struct hostent *serverIPAddress;

    if(argc < 3)
    {
            fprintf(stderr, "usage: <%s> <hostname> <port>\n", argv[0]);
        exit(0);
    }
    portnum = atoi(argv[2]);
    serverIPAddress = gethostbyname(argv[1]);
    if(serverIPAddress == NULL)
    {
        fprintf(stderr, "ERROR, host doesn't exist\n");
        exit(0);
    }

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0)
    {
        error("ERROR creating socket");
    }

    bzero((char *) &serverAddressInfo, sizeof(serverAddressInfo));
    serverAddressInfo.sin_family = AF_INET;
    serverAddressInfo.sin_port = htons(portnum);
    bcopy((char *)serverIPAddress->h_addr, (char *)&serverAddressInfo.sin_addr.s_addr, serverIPAddress->h_length);

    if(connect(sockfd, (struct sockaddr *)&serverAddressInfo, sizeof(serverAddressInfo)) < 0)
    {
        error("ERROR connecting to server");
    }

    //ITS MULTI-THREAD TIME!
    struct threadInfo* threadargs = (struct threadInfo*)malloc(sizeof(struct threadInfo*));
    threadargs->port = portnum;
    threadargs->hostServerIP = serverIPAddress;
    threadargs->fd = sockfd;

    void* threadstatus;

    pthread_t thread;
    pthread_t* threadhandle = &thread;

    pthread_attr_t threadattr;
    pthread_attr_init(&threadattr);

    pthread_attr_setscope(&threadattr, PTHREAD_SCOPE_SYSTEM);

    pthread_create(&thread, &threadattr, threadFunc, (void*)threadargs);

    pthread_attr_destroy(&threadattr);


    //CONNECTED....
    printf("Connected to bank server -- Enter command: ");
    bzero(buffer, 256); //zero out message buffer
    fgets(buffer, 255, stdin); //get message from client standard input
    if(*buffer){
        buffer[strlen(buffer) - 1] = (buffer[strlen(buffer) -  1] == '\n') ? '\0' : buffer[strlen(buffer) - 1];
    }

    //Account for valid commands
    regex_t regex;
    int reti; // returns 0 on proper regex match
    int valid = 0;
    regmatch_t matches[1];

    if(!valid)
    {
        //printf("%s", "in first check");
        reti = regcomp(&regex, "(credit|debit) [0-9]+[.][0-9][0-9]", REG_EXTENDED);
        reti = regexec(&regex, buffer, 1, matches, 0);
        //printf("result: %d", reti);
        if(!reti)
        {
            //match successful
            valid = 1;
            //printf("%s", "nailed it");
        }
    }
    if(!valid)
    {
        //printf("%s", "in 2nd check");
        reti = regcomp(&regex, "(open|start) [a-z]{1,100}", REG_EXTENDED);
        reti = regexec(&regex, buffer, 1, matches, 0);
        //printf("result: %d", reti);
        if(!reti)
        {
            //2nd regex match successful
            valid = 1;
            //printf("%s", "nailed it bruh");
        }
    }
    if(valid)
    {
        //printf("%s", "hi");
        if(matches[0].rm_eo == strlen(buffer))
        {
            //printf(matches[0]);
            n = write(sockfd, buffer, strlen(buffer));
            //printf("%s", "wrote to socket dood!");
        }
        printf("eo: %d, len: %d, word: |%s|", matches[0].rm_eo, strlen(buffer), buffer);
    }
    else
    {
        //validate simple commands
        if(strcmp(buffer, "balance") == 0)
        {
            n = write(sockfd, buffer, strlen(buffer));
            //printf("%s", "balance written");
        }
        else if(strcmp(buffer, "finish") == 0)
        {
            n = write(sockfd, buffer, strlen(buffer));
            //printf("%s", "finish written");
        }
        else if(strcmp(buffer, "exit") == 0)
        {
            n = write(sockfd, "finish", 6); // end existing session with bank
            exit(0); // terminate client
        }
    }

    if(n < 0)
    {
        error("ERROR could not write to socket");
    }

    bzero(buffer, 256);

    n = read(sockfd, buffer, 255);

    if(n < 0)
    {
        error("ERROR could not read from socket");
    }
    //printf("%s\n", buffer);
    regfree(&regex);
    return 0;
}
