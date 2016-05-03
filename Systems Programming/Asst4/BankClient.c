//Jordan Lyon & Biggie Emmanuel Assignment pa4: Bank Server -- client
#include "Bank.h"

struct threadInfo {
    int fd;
};

/* thread to print server responses */
void *threadFunc(void *arg)
{
    int sockfd = ((struct threadInfo*)arg)->fd; // get socket file descriptor

    listen(sockfd, 10); // listen on socket

    // print responses
    char recBuffer[256];
    while(1)
    {
        bzero(recBuffer, 256);
        if((read(sockfd, recBuffer, 255)) > 0) // successful read
        {
            printf("\nServer Response: %s\n", recBuffer);
        }
        else{
            exit(EXIT_SUCCESS);
        }
    }

    return 0;
}

int main(int argc, char *argv[])
{
    struct sockaddr_in serverAddressInfo;
    struct hostent *serverIPAddress;

    if(argc < 3)
    {
            fprintf(stderr, "usage: <%s> <hostname> <port>\n", argv[0]);
        exit(0);
    }

    int portnum = atoi(argv[2]); // get port #

    if((serverIPAddress = gethostbyname(argv[1])) == NULL)
    {
        fprintf(stderr, "ERROR, host doesn't exist\n");
        exit(0);
    }

    int sockfd = -1;
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) // init socket
    {
        //error("ERROR creating socket");
        exit(EXIT_FAILURE);
    }

    // Server setup stuff
    bzero((char *) &serverAddressInfo, sizeof(serverAddressInfo));
    serverAddressInfo.sin_family = AF_INET;
    serverAddressInfo.sin_port = htons(portnum);
    bcopy((char *)serverIPAddress->h_addr, (char *)&serverAddressInfo.sin_addr.s_addr, serverIPAddress->h_length);

    time_t timeBeforeConnect = time(NULL);
    while(connect(sockfd, (struct sockaddr *)&serverAddressInfo, sizeof(serverAddressInfo)) < 0)
    {
        printf("%s\n", "Attempting to connect...\n");
        sleep(3);
        if((time(NULL) - timeBeforeConnect) > 20)
        {
            printf("%s\n", "More than 20 seconds elapsed while connecting; terminating.\n");
            exit(EXIT_FAILURE);
        }
    }

    //ITS MULTI-THREAD TIME!
    struct threadInfo* threadargs = (struct threadInfo*)malloc(sizeof(struct threadInfo*));
    threadargs->fd = sockfd;

    // start thread, which prints repsonses from server
    pthread_t thread;
    pthread_attr_t threadattr;
    pthread_attr_init(&threadattr);
    pthread_attr_setscope(&threadattr, PTHREAD_SCOPE_SYSTEM);
    pthread_create(&thread, &threadattr, threadFunc, (void*)threadargs);
    pthread_attr_destroy(&threadattr);

    //CONNECTED....
    regex_t regex;
    char buffer[256];
    while(1)
    {
        printf("\nConnected to bank server -- Enter command: \n");
        bzero(buffer, 256); //zero out message buffer
        fgets(buffer, 255, stdin); //get message from client standard input
        if(buffer[0] != '\0'){ // remove '\n' from user input
            buffer[strlen(buffer) - 1] = (buffer[strlen(buffer) -  1] == '\n') ? '\0' : buffer[strlen(buffer) - 1];
        }

        //Account for valid commands
        int reti; // returns 0 on proper regex match
        int valid = 0;
        regmatch_t matches[1];
        int n = -1;

        if(!valid)
        {
            reti = regcomp(&regex, "(credit|debit) [0-9]+[.][0-9][0-9]", REG_EXTENDED);
            reti = regexec(&regex, buffer, 1, matches, 0);
            if(!reti)
            {
                valid = 1;
            }
        }
        if(!valid)
        {
            reti = regcomp(&regex, "(open|start) [a-z]{1,100}", REG_EXTENDED);
            reti = regexec(&regex, buffer, 1, matches, 0);
            if(!reti)
            {
                valid = 1;
            }
        }
        if(valid)
        {
            if(matches[0].rm_eo == (int) strlen(buffer))
            {
                n = write(sockfd, buffer, strlen(buffer));
            }
        }
        else
        {
            //validate simple commands
            if(strcmp(buffer, "balance") == 0)
            {
                n = write(sockfd, buffer, strlen(buffer));
            }
            else if(strcmp(buffer, "finish") == 0)
            {
                n = write(sockfd, buffer, strlen(buffer));
            }
            else if(strcmp(buffer, "exit") == 0)
            {
                n = write(sockfd, "finish", 6); // end existing session with bank
                exit(0); // terminate client
            }
        }

        if(n < 0){
            printf("\n%s\n", "Invalid command");
        }

        sleep(2);
    }

    regfree(&regex);

    return 0;
}
