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
#ifndef THD
#ifdef E_TEST_1
    memset(&hashTable, 0, sizeof(hashTable_t));
#else
    hashTable.pEntry = malloc(sizeof(hashEntry_t) * HASH_TABLE_BUCKET);
#endif
#endif
    hashTable.tableSize = HASH_TABLE_BUCKET;
    hashTable.bucketSize = 0;
}

void freeHashTable()
{
#ifndef THD
#ifdef E_TEST_1
#else
    free(hashTable.pEntry);
#endif
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

#ifdef THD
extern unsigned int running_threads;
#endif

entry *findName(char lastName[], entry *e)
{
    unsigned int key = 0;
    hashEntry_t *hash;

#ifdef DEBUG1
    unsigned int index = 0;
#endif

    key = hashFunc(lastName, &hashTable);

#if defined(THD) && defined(DEBUG2)
    printf("(%s:%d) ---> START, input=(%s), key=%u, NUM_OF_THREADS=%d, running_threads=%d\n",__FUNCTION__,__LINE__,
           lastName,
           key,
           NUM_OF_THREADS,
           running_threads);
#endif

#ifndef THD
#ifdef E_TEST_1
    hash = &(hashTable.hashEntry[key]);
#else
    hash = (hashTable.pEntry) + key;
#endif
#endif

#ifdef THD
    for (int i = 0; i < NUM_OF_THREADS; i++) {
        hash = &hashTable.ht[i][key] ;
#ifdef DEBUG2
        printf("(%s:%d) ---> thd=%d, bSize=%u, bUse=%u, sSize=%u, input=(%s:%u), key=%u, value=(%s), sIndex=%u\n", __FUNCTION__,__LINE__,
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
#endif
        index = 0;
        e = hash->pHead;
        while (e != NULL) {
            if (strcasecmp(lastName, e->lastName) == 0) {
#if 0
#ifdef THD
                printf("(%s:%d) ---> END, FOUND, thd=%d, bSize=%u, bUse=%u, sSize=%u, input=(%s:%u), key=%u, value=(%s), sIndex=%u\n",__FUNCTION__,__LINE__,
                       i,
                       hashTable.tableSize,
                       hashTable.bucketSize,
#else
                printf("(%s:%d) ---> bSize=%u, bUse=%u, sSize=%u, input=(%s:%u), key=%u, value=(%s), sIndex=%u\n",__FUNCTION__,__LINE__,
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

#if 1
    printf("(%s:%d) ---> END, NOT FOUND, input=(%s), key=%u\n",__FUNCTION__,__LINE__,
           lastName,
           key);
#endif

    return NULL;
}

#ifdef THD
extern pthread_mutex_t mutex;
entry *append(char lastName[], entry *e, int thd)
#else
entry *append(char lastName[], entry *e)
#endif
{
    unsigned int key = 0;
    hashEntry_t *hash;

#ifdef THD
//    pthread_mutex_lock(&mutex);
#endif

    e = (entry *) malloc(sizeof(entry));
    e->pNext = NULL;
    key = hashFunc(lastName, &hashTable);
#ifdef THD
    hash = &hashTable.ht[thd][key] ;
#else
#ifdef E_TEST_1
    hash = &(hashTable.hashEntry[key]);
#else
    hash = ((hashTable.pEntry) + key);
#endif
#endif

#if defined(THD) && defined(DEBUG2)
    if (strcasecmp(lastName, "uninvolved") == 0)
        printf("\r\n(%s:%d)---> thd=%d, lastName=(%s), key=%u\n", __FUNCTION__, __LINE__,
               thd,
               lastName,
               key);
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
#if 0
    pthread_mutex_lock(& mutex);
    hash->key = key;
    hash->slot++;
    pthread_mutex_unlock(& mutex);
#else
    hash->key = key;
    hash->slot++;
#endif
#endif

#ifdef THD
//    pthread_mutex_unlock(& mutex);
#endif

    return e;
}
