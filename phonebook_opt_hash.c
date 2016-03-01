#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "phonebook_opt_hash.h"

/* FILL YOUR OWN IMPLEMENTATION HERE! */

hashTable_t hashTable;
void initHashTable()
{
    hashTable.pEntry = malloc(sizeof(hashEntry_t) * HASH_TABLE_BUCKET);
//    memset(&hashTable, 0, sizeof(hashTable_t));
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
//    entry *e ;

#ifdef DEBUG1
    unsigned int index = 0;
#endif

    key = nameToKey(lastName, &hashTable);
//    hash = &(hashTable.hashEntry[key]);
    hash = (hashTable.pEntry) + key;

    e = hash->pHead;
    while (e != NULL) {
        if (strcasecmp(lastName, e->lastName) == 0) {
#ifdef DEBUG1
            printf("hashEntry_t=%lu, hashTable_t=%lu, bucket=%u, bucktUse=%u, slot=%u, input=(%s), key=%u, slot_index=%u, value=(%s)\n",
                   sizeof(hashEntry_t),
                   sizeof(hashTable_t),
                   hashTable.tableSize,
                   hashTable.bucketSize,
                   hash->slot,
                   lastName,
                   key,
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

    hash = (hashTable.pEntry) + key;

    e = (entry *) malloc(sizeof(entry));
    e->pNext =NULL;
    strcpy(e->lastName, lastName);
    if (hash->pHead == NULL) {
        hash->pHead = e;
        hashTable.bucketSize++;
    } else {
        hash->pTail->pNext = e;
    }
    hash->pTail = e;
    hash->key = key;
    hash->slot++;
    return e;
}
