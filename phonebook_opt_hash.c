#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "phonebook_opt_hash.h"

/* FILL YOUR OWN IMPLEMENTATION HERE! */

hashTable_t hashTable;

int hash1(char *key)
{
    unsigned int hashVal = 0;
    while (*key != '\0') {
        hashVal += *key++;
    }
    return hashVal & HASH_TABLE_BUCKET;
}

hashEntry_t *appendHashTable(char lastName[], int index, int count,
                             hashEntry_t *h, hashEntry_t *n)
{
    hashEntry_t *e;
    e = (hashEntry_t *) malloc(sizeof(hashEntry_t));
    strcpy(e->lastName, lastName);
    e->key = index;
    e->count = count;
    e->pNext = NULL;
    n->pNext = e;
    return e;
}

hashEntry_t *findTable(char *lastName)
{
    unsigned int index;
    hashEntry_t *e, *pHead;

    index = hash1(lastName);
    e = &(hashTable.hashEntry[index]);

    pHead = e->pHead;
    while (pHead != NULL) {
        if (strcasecmp(lastName, pHead->lastName) == 0) {
            return pHead;
        }
        pHead = pHead->pNext;
    }
    return NULL;
}

int addTable(char *lastName)
{
    unsigned int index;
    hashEntry_t *e;

    index = hash1(lastName);
    e = &(hashTable.hashEntry[index]);
    if (e->hit == 0) {
        e->key = index;
        e->pHead = malloc(sizeof(hashEntry_t));
        e->pHead->pNext = NULL;
        e->pNext = e->pHead;
        strcpy(e->pHead->lastName, lastName);
        e->hit = 1;
        e->count = 0;
    } else {
        e->count++;
        e->pNext = appendHashTable(lastName, index, e->count, e->pHead, e->pNext);
    }
    return 0;
}

entry *findName(char lastName[], entry *pHead)
{
    /* TODO: implement */
    hashEntry_t *e;
    e = findTable(lastName);

    if (e) {
        strcpy(pHead->lastName, e->lastName);
        return pHead;
    }
    return NULL;
}

entry *append(char lastName[], entry *e)
{
    /* allocate memory for the new entry and put lastName */
    addTable(lastName);
    return e;
}
