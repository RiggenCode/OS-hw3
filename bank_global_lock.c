#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include "bank_interface.h"
#include "benchmark.h"

typedef struct account {
    unsigned int account;
    int balance;
    struct account *next;
} account_t;

typedef struct ledger {
    account_t *head;
    account_t *tail;
    pthread_mutex_t lock;
} ledger_t;

int create_account(unsigned int account, void *ledger) {
    ledger_t *l = (ledger_t *) ledger;

    pthread_mutex_lock(&l->lock);
    account_t *curr = l->head;
    while (curr != NULL) {
        if (curr->account == account) {
            pthread_mutex_unlock(&l->lock);
            return -1;
        }
        curr = curr->next;
    }
    account_t *new_acc = (account_t *)malloc(sizeof(account_t));

    if (new_acc == NULL) {
        pthread_mutex_unlock(&l->lock);
        return -1;
    }

    new_acc->account = account;
    new_acc->balance = 0;
    new_acc->next = NULL;


    if (l->tail == NULL) {
        l->head = new_acc;
        l->tail = new_acc;
    } else {
        l->tail->next = new_acc;
        l->tail = new_acc;
    }

    pthread_mutex_unlock(&l->lock);
    return 0;

}

void list_accounts(void *ledger) {
    ledger_t *l = (ledger_t *) ledger;

    pthread_mutex_lock(&l->lock);
    account_t *curr = l->head;
    while (curr != NULL) {
        printf("Account: %u, Balance: %d\n", curr->account, curr->balance);
        curr = curr->next;
    }
    pthread_mutex_unlock(&l->lock);
}

int modify_balance(unsigned int account, int amount, void *ledger) {
    ledger_t *l = (ledger_t *) ledger;

    pthread_mutex_lock(&l->lock);

    account_t *curr = l->head;
    while (curr != NULL) {
        if (curr->account == account) {
            curr->balance += amount;
            int updated = curr->balance;
            pthread_mutex_unlock(&l->lock);
            return updated;

        }
        curr = curr->next;
    }
    pthread_mutex_unlock(&l->lock);
    return -1;
}

int main() {
    ledger_t ledger;
    ledger.head = NULL;
    ledger.tail = NULL;
    pthread_mutex_init(&ledger.lock, NULL);
    
    double time_taken;
    int accounts = 100;
    printf("accounts: %d\n", accounts);
    time_taken = benchmark_driver(1, 10000, accounts, &ledger);
    printf("%f\n", time_taken);

    time_taken = benchmark_driver(10, 10000, accounts, &ledger);
    printf("%f\n", time_taken);

    time_taken = benchmark_driver(100, 10000, accounts, &ledger);
    printf("%f\n", time_taken);

    time_taken = benchmark_driver(1000, 10000, accounts, &ledger);
    printf("%f\n", time_taken);

    return 0;
}