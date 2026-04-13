#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include "benchmark.h"

typedef struct account {
    unsigned int account;
    int balance;
    struct account *next;
    pthread_mutex_t lock;
} account_t;

typedef struct ledger {
    account_t *head;
    account_t *tail;
    pthread_rwlock_t lock;
} ledger_t;

int create_account(unsigned int account, ledger_t *l) {

    pthread_rwlock_rdlock(&l->lock);
    account_t *curr = l->head;
    while (curr != NULL) {
        if (curr->account == account ) {
            pthread_rwlock_unlock(&l->lock);
            return -1;
        }
        curr = curr->next;
    }
    pthread_rwlock_unlock(&l->lock);

    account_t *new_acct = (account_t *)malloc(sizeof(account_t));
    if (new_acct == NULL) {
        pthread_rwlock_unlock(&l->lock);
        return -1;
    }

    new_acct->account = account;
    new_acct->balance = 0;
    new_acct->next    = NULL;
    pthread_mutex_init(&new_acct->lock, NULL);

    pthread_rwlock_wrlock(&l->lock);
    curr = l->head;
    while (curr != NULL) {
        if (curr->account == account) {
            pthread_rwlock_unlock(&l->lock);
            pthread_mutex_destroy(&new_acct->lock);
            free(new_acct);
            return -1;
        }
        curr = curr->next;
    }


    if (l->tail == NULL) {
        l->head = new_acct;
        l->tail = new_acct;
    } else {
        l->tail->next = new_acct;
        l->tail = new_acct;
    }

    pthread_rwlock_unlock(&l->lock);
    return 0;

}

void list_accounts(void *ledger) {
    ledger_t *l = (ledger_t *) ledger;

    pthread_rwlock_rdlock(&l->lock);

    account_t *curr = l->head;
    while (curr != NULL) {
        printf("Account: %u, Balance: %d\n", curr->account, curr->balance);
        curr = curr->next;
    }
    pthread_rwlock_unlock(&l->lock);

}

int modify_balance(unsigned int account, int amount, ledger_t *l) {
    pthread_rwlock_rdlock(&l->lock);

    account_t *curr = l->head;
    while (curr != NULL) {
        if (curr->account == account) {
            pthread_mutex_lock(&curr->lock);
            curr->balance += amount;
            int updated = curr->balance;
            pthread_mutex_unlock(&curr->lock);
            pthread_rwlock_unlock(&l->lock);
            return updated;
        }
        curr = curr->next;
    }
    pthread_rwlock_unlock(&l->lock);
    return -1;
}



int main() {
    ledger_t ledger;
    ledger.head = NULL;
    ledger.tail = NULL;
    pthread_rwlock_init(&ledger.lock, NULL);


    double time_taken;
    int accounts = 100;
    printf("accounts: %d\n", accounts);
    time_taken = benchmark_driver(1, 10000, accounts, &ledger);
    printf("Threads: 1 Time: %f\n", time_taken);

    time_taken = benchmark_driver(10, 10000, accounts, &ledger);
    printf("Threads: 10 Time: %f\n", time_taken);

    time_taken = benchmark_driver(100, 10000, accounts, &ledger);
    printf("Threads: 100 Time: %f\n", time_taken);

    time_taken = benchmark_driver(1000, 10000, accounts, &ledger);
    printf("Threads: 1000 Time: %f\n", time_taken);

    return 0;
}