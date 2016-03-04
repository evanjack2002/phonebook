#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#ifdef THD
#include <pthread.h>
#endif

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

void freeHashTable()
{
#ifdef E_TEST_1
#else
    free(hashTable.pEntry);
#endif
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
#define hashFunc(k, h) hash1(k, h)

#elif HASH_2
unsigned int hash2(char *key, hashTable_t *ht)
{
    unsigned int hashVal = 0;
    while (*key != '\0') {
        hashVal = (hashVal << 5) + *key++;
    }
    return hashVal % ht->tableSize;
}
#define hashFunc(k, h) hash2(k, h)

#elif
#error
#endif

entry *findName(char lastName[], entry *e)
{
    unsigned int key = 0;
    hashEntry_t *hash;

#ifdef DEBUG1
    unsigned int index = 0;
#endif

    key = hashFunc(lastName, &hashTable);
#ifdef E_TEST_1
    hash = &(hashTable.hashEntry[key]);
#else
    hash = (hashTable.pEntry) + key;
#endif

#ifdef THD
    for (int i = 0; i < NUM_OF_THREADS; i++) {
        hash = &hashTable.ht[i][key] ;
#endif
        e = hash->pHead;
        while (e != NULL) {
            if (strcasecmp(lastName, e->lastName) == 0) {
#ifdef DEBUG1
#ifdef THD
                printf("thd=%d, bSize=%u, bUse=%u, sSize=%u, input=(%s:%u), key=%u, value=(%s), sIndex=%u\n",
                       i,
                       hashTable.tableSize,
                       hashTable.bucketSize,
#else
                printf("bSize=%u, bUse=%u, sSize=%u, input=(%s:%u), key=%u, value=(%s), sIndex=%u\n",
                       hashTable.tableSize,
                       hashTable.bucketSize,
#endif
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
#ifdef THD
    }
#endif
    return NULL;
}

#ifdef THD
entry *append(char lastName[], entry *e, int thd)
#else
entry *append(char lastName[], entry *e)
#endif
{
    unsigned int key = 0;
    hashEntry_t *hash;

    e = (entry *) malloc(sizeof(entry));
    e->pNext = NULL;
    key = hashFunc(lastName, &hashTable);
#ifdef E_TEST_1
    hash = &(hashTable.hashEntry[key]);
#else
    hash = ((hashTable.pEntry) + key);
#endif

#ifdef THD
    hash = &hashTable.ht[thd][key] ;
#endif

    strcpy(e->lastName, lastName);

    if (hash->pHead == NULL) {
        hash->pHead = e;
#ifdef DEBUG1
#if 1
        hashTable.bucketSize++;
#else
#ifdef THD
        pthread_mutex_lock(& mutex);
        hashTable.bucketSize++;
        pthread_mutex_unlock(& mutex);
#endif
#endif
#endif
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
