#include <stdio.h>
#include <stdlib.h>
#include "bank_interface.h"

typedef struct account {
    unsigned int account;
    int balance;
    struct account *next;
} account_t;

typedef struct ledger {
    account_t *head;
    account_t *tail;
} ledger_t;

int create_account(unsigned int account, void *ledger) {
	// turn ledger into ledger_t
	ledger_t *l = (ledger_t *) ledger;

	// check if account already exists
	account_t *curr = l->head;
	while (curr != NULL) {
		if (curr->account == account) {
			return -1;
		}
		curr = curr->next;
	}

	/* create new account and add to ledger */
	account_t *new_account = (account_t *) malloc(sizeof(account_t));
	if (new_account == NULL) {
        return -1; /* failed */
    }

    new_account->account = account;
    new_account->balance = 0;
    new_account->next = NULL;

    if (l->tail == NULL) {
        l->head = new_account;
        l->tail = new_account;
    } else {
        l->tail->next = new_account;
        l->tail = new_account;
    }



    return 0; /* success */
}

void list_accounts(void *ledger) {
    // turn ledger into ledger_t
    ledger_t *l = (ledger_t *) ledger;

    account_t *curr = l->head;
    while (curr != NULL) {
        printf("Account: %u, Balance: %d\n", curr->account, curr->balance);
        curr = curr->next;
    }
}

int modify_balance(unsigned int account, int balance, void *ledger) {
    ledger_t *l = (ledger_t *) ledger;

    account_t *curr = l->head;
    while (curr != NULL) {
        if (curr->account == account) {
            curr->balance += balance;
            return 0; /* success */
        }
        curr = curr->next;
    }
    return -1; /* account not found */
}

int main() {
    ledger_t ledger;
    ledger.head = NULL;
    ledger.tail = NULL;

    create_account(0, &ledger);
    create_account(1, &ledger);
    create_account(2, &ledger);

    list_accounts(&ledger);
    
    modify_balance(1, 10, &ledger);
    modify_balance(0, -100, &ledger);
    modify_balance(123, -100, &ledger);

    list_accounts(&ledger);

    return 0;
}