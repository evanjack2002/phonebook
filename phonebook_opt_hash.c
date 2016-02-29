#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "phonebook_opt_hash.h"

/* FILL YOUR OWN IMPLEMENTATION HERE! */

hashTable_t hashTable;

void initHashTable()
{
    memset(&hashTable, 0, sizeof(hashTable_t));
    hashTable.tableSize = HASH_TABLE_BUCKET;
}

#ifdef HASH_2
unsigned int hash2(char *key, hashTable_t *ht)
{
    unsigned int hashVal = 0;
    while (*key != '\0') {
        hashVal = (hashVal << 5) + *key++;
    }
    return hashVal % ht->tableSize;
}
#else
unsigned int hash1(char *key, hashTable_t *ht)
{
    unsigned int hashVal = 0;
    while (*key != '\0') {
        hashVal += *key++;
    }
    return hashVal % ht->tableSize;
}
#endif

#ifdef HASH_2
#define nameToKey(k, h) hash2(k, h)
#elif HASH_1
#define nameToKey(k, h) hash1(k, h)
#else
#define nameToKey(k, h) hash1(k, h)
#endif

entry *findName(char lastName[], entry *pHead)
{
    unsigned int key;
    hashEntry_t *hash;
    entry *e ;

    key = nameToKey(lastName, &hashTable);
    hash = &(hashTable.hashEntry[key]);

#ifdef DEBUG1
    unsigned int index = 0;
#endif

    e = hash->pHead;
    while (e != NULL) {
        if (strcasecmp(lastName, e->lastName) == 0) {
#ifdef DEBUG1
            printf("bucket=%u, slot=%u, input=(%s), slot_index=%u, value=(%s)\n",
                   hashTable.tableSize,
                   hash->slot,
                   lastName,
                   index,
                   e->lastName);
#endif
            return e;
        }
        e = e->pNext;
#ifdef DEBUG1
        index++;
#endif
    }
    return NULL;
}

entry *append(char lastName[], entry *e)
{
    unsigned int key;
    hashEntry_t *hash;

    key = nameToKey(lastName, &hashTable);
    hash = &(hashTable.hashEntry[key]);
    if (hash->pHead != NULL) {
        e = hash->pTail;
        e->pNext = (entry *) malloc(sizeof(entry));
        e = e->pNext;
    } else {
        hash->pHead = (entry *) malloc(sizeof(entry));
        e = hash->pHead;
    }
    strcpy(e->lastName, lastName);
    e->pNext = NULL;
    hash->slot++;
    hash->pTail = e;
    return e;
}
