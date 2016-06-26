/**************************************************************
 *BankServer.c
 *
 *Server which accepts all incoming requests to the bank and
 *spawns processes to handle each client
 *
 *Authors: Biggie Emmanuel and Jordano Lyono
***************************************************************/

#include "Bank.h"

void endMethod(int signal);
void suicide(int signal);
void clientHandler(int clientSocket);
void openAccount(int clientSocket, char *commandArg, int *in_session);
void startAccount(int clientSocket, char *commandArg, int *in_session);
void finishSession(int clientSocket, int *in_session);
void getBalance(int clientSocket, int *in_session);
void debitAccount(int clientSocket, char *commandArg, int *in_session);
void creditAccount(int clientSocket, char *commandArg, int *in_session);

int spawnedProcesses; // number of processes spawned
int printBankPID; // pid of bank printing process
Bank *bank; // ptr to bank
int bankID; // shmid for bank
int originalParentPPID;

int main(int argc, char *argv[])
{
    if(argc < 2){
        printf("%s", "ERROR: No Port Number Provided");
        exit(EXIT_FAILURE);
    }

    spawnedProcesses = 0;
    originalParentPPID = getppid();

    signal(SIGINT, endMethod); // set function to run at program termination

    // allocate space for bank in shared memory
    bankID = shmget(KEY, sizeof(Bank), IPC_CREAT | 0666);

    bank = (Bank*) shmat(bankID, NULL, 0); // get bank from shared mem
    createBank(bank); // init bank

    // make account locks
    char semaphoreName[100];
    int i;
    for(i = 0; i < MAXACCOUNTS; i++)
    {
        bzero((void*) semaphoreName, 100); // zero out buffer
        sprintf(semaphoreName, "%d", i); // create name for lock
        sem_unlink(semaphoreName); // in case it already exists
        sem_open(semaphoreName, O_CREAT, 0644, 1); // create account lock
    }

    sem_unlink(BANKLOCK); // in case it already exists
    sem_open(BANKLOCK, O_CREAT, 0644, 1); // create bank lock

    // args and env to be passed to child process
    char *args[] = {"PrintBank", 0};
    char *env[] = { 0 };
    // get path name for exe to be run in new process
    char filePath[PATH_MAX];
    realpath("PrintBank", filePath);

    // spawn bank-printing process
    spawnedProcesses++;
    pid_t pid = fork();
    printBankPID = pid;
    if(pid == 0) // this is child process
    {
        execve(
            filePath,
            args, env);
        perror("Failure to run PrintBank.c");
        exit(EXIT_FAILURE);
    }
    else if(pid < 0) // fork failed
    {
        printf("%s", "Failure to Fork");
        exit(EXIT_FAILURE);
    }

    int port = atoi(argv[1]); // port number to bind socket to
    // build socket
    int serverSocket;
    if((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0){
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

    listen(serverSocket, 10);
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
            printf("%s", "Error spawning clientHandler process\n");
            exit(EXIT_FAILURE);
        }
        printf("%s", "Recieved a client connection.\n");
        spawnedProcesses++;
    }

    return EXIT_SUCCESS;
}

void clientHandler(int clientSocket)
{
    // connection error
    if(clientSocket < 0){
        printf("%s", "Error connecting to clientSocket.\n");
        exit(EXIT_FAILURE);
    }

    int in_session = -1;

    // read from client
    char buffer[256]; // buffer for socket messages
    while(1){
        bzero((void*) buffer, 256);
        if((read(clientSocket, buffer, 255)) <= 0){ // no response
            exit(EXIT_SUCCESS);
        }

        // parse command
        char *command = strtok(buffer, " \n");
        char *commandArg = strtok(NULL, " \n");

        if(strcmp(command, "open") == 0){ // open account
           openAccount(clientSocket, commandArg, &in_session);
        }
        else if(strcmp(command, "start") == 0){ // start account session
            startAccount(clientSocket, commandArg, &in_session);
        }
        else if(strcmp(command, "credit") == 0){ // credit account
            creditAccount(clientSocket, commandArg, &in_session);
        }
        else if(strcmp(command, "debit") == 0){ // debit account
            debitAccount(clientSocket, commandArg, &in_session);
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

    return;
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
        if(bank->accounts[i].name[0] != '\0'){ // taken
            // check if desired account to open already exists
            exists = (exists == 0) ? (strcmp(bank->accounts[i].name, commandArg) == 0) : 1;
            continue;
        }
        else{
            break;
        }
    }

    if(exists){
        write(clientSocket, "Account already exists", 22);
    }
    else if(i < MAXACCOUNTS)
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
        if(bank->accounts[i].name[0] == '\0'){ // free account; end of list
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
            break;
        }
    }

    if(i >= MAXACCOUNTS){ // bank not found
        write(clientSocket, "Error: bank with specified account name not found.", 50);
    }

    sem_post(bankLock);

    return;
}

void finishSession(int clientSocket, int *in_session)
{
    // end the account session if they are in one
    if(*in_session >= 0){
        bank->accounts[*in_session].in_use = 0;
        *in_session = -1;
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

    return;
}

void debitAccount(int clientSocket, char *commandArg, int *in_session)
{
    if(*in_session < 0){
        write(clientSocket, "Can't debit account, not in session", 36);
        return;
    }

    // get lock for account
    char lockName[100];
    bzero((void*) lockName, 100);
    sprintf(lockName, "%d", *in_session);
    sem_t *accountLock = sem_open(lockName, 0);
    sem_wait(accountLock);

    // check for sufficient funds
    float debitAmount = atof(commandArg);
    if(bank->accounts[*in_session].balance < debitAmount) // insufficient
    {
        write(clientSocket, "Can't debit account, insufficient funds.", 40);
    }
    else // sufficient
    {
        bank->accounts[*in_session].balance -= debitAmount;
        write(clientSocket, "Debit successful!", 17);
    }

    sem_post(accountLock);

    return;
}

void creditAccount(int clientSocket, char *commandArg, int *in_session)
{
    if(*in_session < 0){
        write(clientSocket, "Can't credit account, not in session", 36);
        return;
    }

    // get lock for account
    char lockName[100];
    bzero((void*) lockName, 100);
    sprintf(lockName, "%d", *in_session);
    sem_t *accountLock = sem_open(lockName, 0);
    sem_wait(accountLock);

    // credit account
    bank->accounts[*in_session].balance += atof(commandArg);

    sem_post(accountLock);

    write(clientSocket, "Account credited!", 17);

    return;
}

void endMethod(int signal)
{
    if(getppid() != originalParentPPID){ // not parent
        exit(EXIT_SUCCESS);
    }

    kill(printBankPID, SIGKILL); // kill bankPrinting process

    // remove shared memory
    shmdt((void*) bank);
    shmctl(bankID, IPC_RMID, NULL);
    printf("%s\n", "\nRemoved Shared Memory\n");

    exit(EXIT_SUCCESS);
}

void suicide(int signal)
{
    exit(EXIT_SUCCESS);
}
