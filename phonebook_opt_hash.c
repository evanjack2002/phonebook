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

    e = hash->pHead;
    while (e != NULL) {
        if (strcasecmp(lastName, e->lastName) == 0) {
#ifdef DEBUG1
            printf("bSize=%u, bUse=%u, sSize=%u, input=(%s:%u), key=%u, value=(%s), sIndex=%u\n",
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

#ifdef THD
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
#endif

entry *append(char lastName[], entry *e)
{
    unsigned int key = 0;
    hashEntry_t *hash;

#ifdef THD
    pthread_mutex_lock(&mutex);
#endif

    e = (entry *) malloc(sizeof(entry));
    e->pNext = NULL;
    key = hashFunc(lastName, &hashTable);
#ifdef E_TEST_1
    hash = &(hashTable.hashEntry[key]);
#else
    hash = ((hashTable.pEntry) + key);
#endif
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

#ifdef THD
    pthread_mutex_unlock(&mutex);
#endif

    return e;
}

#ifdef THD
void *processArray(void *args)
{
    thread_data_t *data = (thread_data_t *)args;
    char **arr = data->arr;
    int start = data->start;
    int end   = data->end;
    long total = data->total;
    int i = 0;
    entry *e = NULL;

#if 0
    printf("pthred_id=%lu, start=%d, end=%d, total=%lu\n",
           pthread_self(),
           start,
           end,
           total);
#endif

    // 1. Wait for a signal to start from the main thread
    for (i = start; i < end; i++) {
#if 1
        if (strlen(&arr[i]) == 0)
            printf("pthred_id=%lu, bSize=%d, slot=%d, arr[%d]=(%s))\n",
                   pthread_self(),
                   hashTable.bucketSize,
                   (hashTable.pEntry + hashFunc(&arr[i], &hashTable))->slot,
                   i,
                   &arr[i]);
#endif
        e = append(&arr[i], e);
    }

    // 2. Signal to the main thread that you're done
    pthread_exit(NULL);
}
#endif