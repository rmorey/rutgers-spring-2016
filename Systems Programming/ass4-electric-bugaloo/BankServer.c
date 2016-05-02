/**************************************************************
 *BankServer.c
 *
 *Server which accepts all incoming requests to the bank and
 *spawns processes to handle each client
 *
 *Authors: Biggie Emmanuel and Jordano Lyono
***************************************************************/

#include "Bank.h"

// temp
#define KEY 666 // key used to access bank in shared mem
#define MAXACCOUNTS 20 // max # of accounts in bank
// Name for bank Semaphore
#define BANKLOCK "bank"
#include <stdlib.h>
#include <stdio.h>
#include <strings.h>
#include <string.h>
#include <unistd.h>
#include  <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/prctl.h>
// end temp

void endMethod(int signal);
void suicide(int signal);
void clientHandler(int clientSocket);
void openAccount(int clientSocket, char *commandArg, int *in_session);
void startAccount(int clientSocket, char *commandArg, int *in_session);
void finishSession(int clientSocket, int *in_session);
void getBalance(int clientSocket, int *in_session);

int spawnedProcesses;
int printBankPID; // pid of bank printing process
Bank *bank;

int main(int argc, char *argv[])
{
    if(argc < 2){
        printf("%s", "ERROR: No Port Number Provided");
        exit(EXIT_FAILURE);
    }

    spawnedProcesses = 0;

    signal(SIGINT, endMethod); // set function to run at program termination;

    /* Init bank and put into shared memory */

    // allocate space for bank in shared memory
    int bankID = shmget(KEY, sizeof(Bank), IPC_CREAT | 0666);

    bank = (Bank*) shmat(bankID, NULL, 0); // get bank from shared mem
    createBank(bank); // init bank

    /* Init bank semaphores (locks) */

    char semaphoreName[100];
    int i;
    // make account locks
    for(i = 0; i < MAXACCOUNTS; i++)
    {
        bzero((void*) semaphoreName, 100); // zero out buffer
        sprintf(semaphoreName, "%d", i); // create name for lock
        sem_open(semaphoreName, O_CREAT, 0644, 1); // create account lock
    }
    sem_open(BANKLOCK, O_CREAT, 0644, 1); // create bank lock

    /* Spawn process to print bank contents on an interval*/

    char *args[] = {"PrintBank", 0}; // args to be passed to child process
    char *env[] = { 0 }; // environment to be passed to child process
    pid_t pid = fork();
    printBankPID = pid;
    spawnedProcesses++;
    if(pid == 0) // this is child process
    {
        execve(
            "/home/biggie/school/rutgers-spring-2016/Systems Programming/ass4-electric-bugaloo/PrintBank",
            args, env);
        perror("Failure to run PrintBank.c");
        exit(EXIT_FAILURE);
    }
    else if(pid < 0) // fork failed
    {
        printf("%s", "Failure to Fork");
        exit(EXIT_FAILURE);
    }

    /* Create socket and bind to specified server port */

    int port = atoi(argv[1]); // port number to bind socket to

    // build socket
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if(serverSocket < 0){
        printf("%s", "Error opening socket.");
        exit(EXIT_FAILURE);
    }

    // setup server address struct
    struct sockaddr_in serverAddressInfo;
    bzero((void*) &serverAddressInfo, sizeof(serverAddressInfo));
    serverAddressInfo.sin_port = htons(port);
    serverAddressInfo.sin_family = AF_INET;
    serverAddressInfo.sin_addr.s_addr = INADDR_ANY;

    // bind socket to port
    if( (bind(serverSocket, (struct sockaddr *) &serverAddressInfo,
        sizeof(serverAddressInfo))) < 0)
    {
        printf("%s", "Error binding to port");
        exit(EXIT_FAILURE);
    }

    // set socket to listen for client connects
    listen(serverSocket, 10);

    // listen and accept incoming requests
    int clientSocket; // file descriptor for client socket
    struct sockaddr_in clientAddressInfo;
    socklen_t len = sizeof(clientAddressInfo);
    while(1)
    {
        clientSocket = accept(serverSocket,
                (struct sockaddr *) &clientAddressInfo,
                &len); // accept incoming connection

        //spawn process to handle connection
        pid_t prev_ppid = getpid();
        pid = fork();
        if(pid == 0) // child process
        {
            // set SIGTERM signal to emit when parent dies so process can kill itself
            prctl(PR_SET_PDEATHSIG, SIGTERM);
            signal(SIGTERM, suicide);

            // check if parent is already dead
            if(getppid() != prev_ppid){
                suicide(-1);
            }

            clientHandler(clientSocket); // run client handler
        }
        else if(pid < 0){ // fork failed
            printf("%s", "Error spawning clientHandler process");
            exit(EXIT_FAILURE);
        }
        spawnedProcesses++;
    }

    return EXIT_SUCCESS;
}

void clientHandler(int clientSocket)
{
    // connection error
    if(clientSocket < 0){
        printf("%s", "Error connecting to clientSocket.");
        exit(EXIT_FAILURE);
    }

    int in_session = 0;

    // create buffer for socket messages
    char buffer[256];
    bzero((void*) buffer, 256);

    // read from client
    while(1){
        if((read(clientSocket, buffer, 255)) <= 0){
            printf("%s", "No client message or error, not sure lol");
            exit(EXIT_SUCCESS);
        }

        // display message for debug purposed
        printf("Client message: %s", buffer);

        // parse command
        char *command = strtok(buffer, " \n");
        char *commandArg = strtok(NULL, " \n");

        /* handle client operation */

        if(strcmp(command, "open") == 0){ // open account
           openAccount(clientSocket, commandArg, &in_session);
        }
        else if(strcmp(command, "start") == 0){ // start account session
            startAccount(clientSocket, commandArg, &in_session);
        }
        else if(buffer[0] == 'c'){ // credit account

        }
        else if(buffer[0] == 'd'){ // debit account

        }
        else if(strcmp(command, "balance") == 0){ // get balance
            getBalance(clientSocket, &in_session);
        }
        else if(strcmp(command, "finish") == 0){ // finish
            finishSession(clientSocket, &in_session);
        }
        else{
            printf("%s", "Client done goofed, abort mission.");
            exit(EXIT_FAILURE);
        }
    }

}

void openAccount(int clientSocket, char *commandArg, int *in_session)
{
    sem_t *bankLock = sem_open(BANKLOCK, 0);
    sem_wait(bankLock);

    if(*in_session >= 0){
        write(clientSocket, "Cannot create account, you're in a session.", 43);
        return;
    }

    // find free account spot and check if it already exits
    int i;
    int exists = 0;
    for(i = 0; i < MAXACCOUNTS; i++)
    {
        if(bank->accounts[i].name){ // taken
            // check if desired account to open already exists
            exists = (exists == 0) ? (strcmp(bank->accounts[i].name, commandArg) == 0) : 1;
            continue;
        }
    }

    if(i < MAXACCOUNTS) // found free spot
    {
        strcpy(bank->accounts[i].name, commandArg);
        write(clientSocket, "Account created!", 16);

    }
    else
    {
        write(clientSocket, "Error: Bank full", 16);
    }

    sem_post(bankLock);

    return;
}

void startAccount(int clientSocket, char *commandArg, int *in_session)
{
    if(*in_session >= 0){
        write(clientSocket, "Cannot create account, you're in a session.", 43);
        return;
    }

    sem_t *bankLock = sem_open(BANKLOCK, 0);
    sem_wait(bankLock);

    // find specified account
    int i;
    for(i = 0; i < MAXACCOUNTS; i++)
    {
        if(bank->accounts[i].name){
            break;
        }

        if(strcmp(bank->accounts[i].name, commandArg) == 0) // found bank
        {
            // wait till account is free to use
            while(bank->accounts[i].in_use){
                write(clientSocket, "Account currently in use, waiting to connect", 44);
                sleep(2); // wait 2 seconds
            }

            // start session
            bank->accounts[i].in_use = 1;
            *in_session = i;
            write(clientSocket, "Connected successfully!", 23);
        }
    }

    write(clientSocket, "Error: bank with specified account name not found.", 50); // bank not found

    sem_post(bankLock);

    return;
}

void finishSession(int clientSocket, int *in_session)
{
    // end the account session if they are in one
    if(*in_session >= 0){
        bank->accounts[*in_session].in_use = 0;
    }

    write(clientSocket, "Successful exit!", 16);

    return;
}

void getBalance(int clientSocket, int *in_session)
{
    if(*in_session < 0){
        write(clientSocket, "Can't get balance, not in session", 33);
        return;
    }

    // get lock for account
    char lockName[100];
    bzero((void*) lockName, 100);
    sprintf(lockName, "%d", *in_session);
    sem_t *accountLock = sem_open(lockName, 0);
    sem_wait(accountLock);

    // send balance to client
    char response[256];
    bzero((void*) response, 256);
    sprintf(response, "Current balance is: %f", bank->accounts[*in_session].balance);
    write(clientSocket, response, strlen(response));

    sem_post(accountLock);
}

void endMethod(int signal)
{
    // kill bankPrinting process
    kill(printBankPID, SIGKILL);

    // wait on all spawned processes
    int status;
    int i;
    for(i = 0; i < spawnedProcesses; i++)
    {
        wait(&status);
        if(status){
            // should do somthing along these lines
        }
    }

    printf("%s", "Waited properly");

    exit(EXIT_SUCCESS);
}

void suicide(int signal)
{
    printf("%s", "goodbye cruel world.");
    exit(EXIT_SUCCESS);
}
