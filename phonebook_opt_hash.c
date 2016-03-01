#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "phonebook_opt_hash.h"

/* FILL YOUR OWN IMPLEMENTATION HERE! */

hashTable_t hashTable;
void initHashTable()
{
#ifdef E_TEST_1
    memset(&hashTable, 0, sizeof(hashTable_t));
#else
    hashTable.pEntry = malloc(sizeof(hashEntry_t) * HASH_TABLE_BUCKET);
#endif
    hashTable.tableSize = HASH_TABLE_BUCKET;
    hashTable.bucketSize = 0;
}

#ifdef HASH_1
unsigned int hash1(char *key, hashTable_t *ht)
{
    unsigned int hashVal = 0;
    while (*key != '\0') {
        hashVal += *key++;
    }
    return hashVal % ht->tableSize;
}
#define nameToKey(k, h) hash1(k, h)

#elif HASH_2
unsigned int hash2(char *key, hashTable_t *ht)
{
    unsigned int hashVal = 0;
    while (*key != '\0') {
        hashVal = (hashVal << 5) + *key++;
    }
    return hashVal % ht->tableSize;
}
#define nameToKey(k, h) hash2(k, h)

#elif
#error
#endif

entry *findName(char lastName[], entry *e)
{
    unsigned int key;
    hashEntry_t *hash;

#ifdef DEBUG1
    unsigned int index = 0;
#endif

    key = nameToKey(lastName, &hashTable);
#ifdef E_TEST_1
    hash = &(hashTable.hashEntry[key]);
#else
    hash = (hashTable.pEntry) + key;
#endif

    e = hash->pHead;
    while (e != NULL) {
        if (strcasecmp(lastName, e->lastName) == 0) {
#ifdef DEBUG1
            printf("buckSize=%u, buckUse=%u, slotSize=%u, input=(%s:%u), key=%u, value=(%s), slotIndex=%u\n",
                   hashTable.tableSize,
                   hashTable.bucketSize,
                   hash->slot,
                   lastName,
                   key,
                   hash->key,
                   e->lastName,
                   index);
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

    e = (entry *) malloc(sizeof(entry));
    e->pNext =NULL;
    key = nameToKey(lastName, &hashTable);
#ifdef E_TEST_1
    hash = &(hashTable.hashEntry[key]);
#else
    hash = ((hashTable.pEntry) + key);
#endif
    strcpy(e->lastName, lastName);
    if (hash->pHead == NULL) {
        hash->pHead = e;
        hashTable.bucketSize++;
    } else {
        hash->pTail->pNext = e;
    }
    hash->pTail = e;
#ifdef DEBUG1
    hash->key = key;
    hash->slot++;
#endif
    return e;
}
