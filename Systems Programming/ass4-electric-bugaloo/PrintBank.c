/**************************************************************
 *PrintBank.c
 *
 *Print the contents of the bank on an interval
 *
 *Authors: Biggie and Jordano
***************************************************************/

#include "Bank.h"

#define INTERVAL 20 // interval, in seconds, on which bank contents are printed

int main(int argc, char *argv[])
{
    int bankID = shmget(KEY, sizeof(Bank), 0666);
    Bank *bank = (Bank*) shmat(bankID, NULL, 0);

    printf("%s\n", "hiya from printBank!");
    while(1)
    {
        sleep(INTERVAL);

        /* print bank */

        sem_t *bankLock = sem_open(BANKLOCK, 0);
        sem_wait(bankLock); // lock bank

        int i;
        char lockName[100];
        for (i = 0; i < 20; i++)
        {
            // get lock for account
            bzero((void*) lockName, 100); // clear buffer
            sprintf(lockName, "%d", i);
            sem_t *accountLock = sem_open(lockName, 0);
            printf("%s\n", "HA");
            sem_wait(accountLock); // lock account

            printf("Bank #%d:\tbalance: %f\t%s\tname: %s\n",
                    i, bank->accounts[i].balance,
                    (bank->accounts[i].in_use) ? "IN SERVICE" : "NOT IN SERVICE",
                    bank->accounts[i].name);

            sem_post(accountLock); // unlock account
        }

        sem_post(bankLock); // unlock bank
    }

    return EXIT_SUCCESS;
}
