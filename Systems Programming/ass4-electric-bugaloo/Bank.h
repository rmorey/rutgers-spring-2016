/**************************************************************
 *Bank.h
 *
 *Struct definitions for bank
 *
 *Authors: Biggie and Jordano
***************************************************************/

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

/******** Declarations ******************/
/***************************************/
struct Account;
struct Bank;

/******** Account **********************/
/***************************************/

struct Account
{
    char name[101];
    float balance;
    int in_use; // 1 if account is being used, 0 otherwise
};
typedef struct Account Account;

/******** Bank *************************/
/***************************************/

struct Bank
{
    Account accounts[MAXACCOUNTS];
};
typedef struct Bank Bank;


void createBank(Bank *b)
{
    int i;
    for(i = 0; i < MAXACCOUNTS; i++)
    {
        b->accounts[i].balance = 0.0f;
        b->accounts[i].in_use = 0;
        bzero((void*) b->accounts[i].name, 101);
    }

    return;
}
