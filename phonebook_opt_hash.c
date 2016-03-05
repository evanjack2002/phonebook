#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#ifdef THREAD
#include <pthread.h>
#endif

#include "phonebook_opt_hash.h"

/* FILL YOUR OWN IMPLEMENTATION HERE! */
hashTable_t hashTable;

#ifdef DEBUG2
int debug_thd = 0;
#endif

#ifdef HASH1
unsigned int hash1(char *key, hashTable_t *ht)
{
    unsigned int hashVal = 0;
    while (*key != '\0') {
        hashVal += *key++;
    }
    return hashVal % ht->tableSize;
}
#define hashFunc(k, h) hash1(k, h)

#elif HASH2
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

#ifdef THREAD
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
#endif

void initHashTable()
{
#ifndef THREAD
#ifdef E_TEST_1
    memset(&hashTable, 0, sizeof(hashTable_t));
#else
    hashTable.bucket = malloc(sizeof(hashEntry_t) * HASH_TABLE_BUCKET);
#endif
#endif
    hashTable.tableSize = HASH_TABLE_BUCKET;
    hashTable.bucketSize = 0;
}

void freeHashTable()
{
#ifndef THREAD
#ifdef E_TEST_1
#else
    free(hashTable.bucket);
#endif
#endif
}

#ifdef THREAD
entry *findName(char lastName[], entry *e)
{
    unsigned int key = 0;
    hashEntry_t *hash;
    int i = 0;
#ifdef DEBUG1
    unsigned int index = 0;
#endif

    key = hashFunc(lastName, &hashTable);

#ifdef DEBUG2
    printf("\r\n(%s:%d) ---> START, input=(%s), key=%u, NUM_OF_THREADS=%d",__FUNCTION__,__LINE__,
           lastName,
           key,
           NUM_OF_THREADS);
#endif

    for (i = 0; i < NUM_OF_THREADS; i++) {
#ifdef DEBUG1
        index = 0;
#endif
        hash = &hashTable.bucket[i][key];
        e = hash->pHead;
        while (e != NULL) {
            if (strcasecmp(lastName, e->lastName) == 0) {
#ifdef DEBUG1
                printf("\r\n(%s:%d) ---> FOUND, thd=%d, bSize=%u, bUse=%u, sSize=%u, input=(%s:%u), key=%u, value=(%s), sIndex=%u",__FUNCTION__,__LINE__,
                       i,
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
    }
#if DEBUG1
    printf("\r\n(%s:%d) ---> NOT FOUND, input=(%s), key=%u",__FUNCTION__,__LINE__,
           lastName,
           key);
#endif
    return NULL;
}

entry *append(char lastName[], entry *e, int thd)
{
    unsigned int key = 0;
    hashEntry_t *hash;

    key = hashFunc(lastName, &hashTable);
    hash = &hashTable.bucket[thd][key] ;

#ifdef DEBUG2
    if (strcasecmp(lastName, "zyxel") == 0) {
        debug_thd = thd;
        printf("\r\n(%s:%d)---> thd=%d, lastName=(%s), key=%u", __FUNCTION__, __LINE__,
               thd,
               lastName,
               key);
    }
#endif

    e = (entry *) malloc(sizeof(entry));
    e->pNext = NULL;
    strcpy(e->lastName, lastName);

    if (hash->pHead == NULL) {
        hash->pHead = e;
#if defined(__GNUC__)
        __builtin___clear_cache((char *) hash->pHead, (char *) hash->pHead + sizeof(entry));
#endif
#ifdef DEBUG1
        pthread_mutex_lock(& mutex);
        hashTable.bucketSize++;
        pthread_mutex_unlock(& mutex);
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

#else /* else of THREAD */
entry *findName(char lastName[], entry *e)
{
    unsigned int key = 0;
    hashEntry_t *hash;

#ifdef DEBUG1
    unsigned int index = 0;
#endif

    key = hashFunc(lastName, &hashTable);
#ifdef E_TEST_1
    hash = &(hashTable.bucket[key]);
#else
    hash = (hashTable.bucket) + key;
#endif

    e = hash->pHead;
    while (e != NULL) {
        if (strcasecmp(lastName, e->lastName) == 0) {
#ifdef DEBUG1
            printf("(%s:%d) ---> FOUND, bSize=%u, bUse=%u, sSize=%u, input=(%s:%u), key=%u, value=(%s), sIndex=%u\n",__FUNCTION__,__LINE__,
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
#if DEBUG1
    printf("(%s:%d) ---> NOT FOUND, input=(%s), key=%u\n",__FUNCTION__,__LINE__,
           lastName,
           key);
#endif
    return NULL;
}

entry *append(char lastName[], entry *e)
{
    unsigned int key = 0;
    hashEntry_t *hash;

    key = hashFunc(lastName, &hashTable);
#ifdef E_TEST_1
    hash = &(hashTable.bucket[key]);
#else
    hash = ((hashTable.bucket) + key);
#endif

    e = (entry *) malloc(sizeof(entry));
    e->pNext = NULL;
    strcpy(e->lastName, lastName);

    if (hash->pHead == NULL) {
        hash->pHead = e;
#ifdef DEBUG1
        hashTable.bucketSize++;
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
#endif /* end of THREAD */

