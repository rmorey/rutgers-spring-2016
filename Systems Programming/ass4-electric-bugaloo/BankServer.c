/**************************************************************
 *BankServer.c
 *
 *Server which accepts all incoming requests to the bank and
 *spawns processes to handle each client
 *
 *Authors: Biggie Emmanuel and Jordano Lyono
***************************************************************/

#include "Bank.h"

int spawnedProcesses;
int printBankPID; // pid of bank printing process

void endMethod()
{
    // kill bankPrinting process
    kill(printBankPID, SIGKILL);

    // wait on all spawned processes
    int status;
    int i;
    for(i = 0; i < spawnedProcesses; i++)
    {
        wait(&status);
    }

    printf("%s", "Waited properly");

    exit(EXIT_SUCCESS); 
}


int main(int argc, char *argv[])
{
    /* Init bank and put into shared memory */
    // allocate space for bank in shared memory
    int bankID = shmget(KEY, sizeof(Bank), IPC_CREAT | 0666);   
    Bank *bank = shmat(bankID, NULL, 0); // get bank from shared mem
    createBank(bank); // init bank 

    /* Init bank semaphores (locks) */
    char semaphoreName[100];
    int i;
    // make account locks
    for(i = 0; i < MAXACCOUNTS; i++)
    {
        bzero((void*) semaphoreName, 100); // zero out buffer
        sprintf(semaphoreName, "%d", i); // name for locks
        sem_open(semaphoreName, O_CREAT, 0644, 1); // create account lock
    }
    sem_open(BANKLOCK, O_CREAT, 0644, 1); // create bank lock

    signal(SIGINT, endMethod); // set function to run at program termination;

    // process spawning stuff
    spawnedProcesses = 0; // number of processes spawned
    char *args[] = {"PrintBank", 0}; // args to be passed to child process
    char *env[] = { 0 }; // environment to be passed to child process

    /* Spawn process to print bank contents on an interval*/
    pid_t pid = fork();
    printBankPID = pid;
    spawnedProcesses++;
    if(pid == 0) // this is child process
    {
        execve(
            "/home/biggie/school/rutgers-spring-2016/Systems Programming/ass4-electric-bugaloo/PrintBank",
            args, env);
        perror("Failure to run PrintBank.c");
        return EXIT_FAILURE;
    }
    else if(pid < 0) // fork failed
    {
        printf("%s", "Failure to Fork");
        return EXIT_FAILURE;
    }


    while(1)
    {

    }

    // bind to port and listen

    // while
        // accept
        // give to kid

    return EXIT_SUCCESS;
}
