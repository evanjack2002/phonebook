#ifndef _PHONEBOOK_H
#define _PHONEBOOK_H

#define MAX_LAST_NAME_SIZE 16

/* TODO: After modifying the original version, uncomment the following
 * line to set OPT properly */

#ifdef HASH_1
#define HASH1 1
#endif

#ifdef HASH_2
#define HASH2 1
#endif

#ifdef THD
#define NUM_OF_THREADS 2

extern char buf[400000][MAX_LAST_NAME_SIZE];
extern pthread_mutex_t mutex;

typedef struct thread_data_s {
    int start;
    int end;
    int thd;
} thread_data_t;
#endif

typedef struct phoneBook_s {
    char firstName[16];
    char email[16];
    char phone[10];
    char cell[10];
    char addr1[16];
    char addr2[16];
    char city[16];
    char state[2];
    char zip[5];
} phoneBook_t;

typedef struct __PHONE_BOOK_ENTRY {
    char lastName[MAX_LAST_NAME_SIZE];
    phoneBook_t *pNode;
    struct __PHONE_BOOK_ENTRY *pNext;
} entry;

typedef struct hashEntry_s {
    unsigned int key;
    unsigned int slot;
    entry *pHead;
    entry *pTail;
} hashEntry_t;

#if 1
#define BUCKET_UNIT 42737
#else
#define BUCKET_UNIT 7919
#endif

#ifdef THD
#define HASH_TABLE_BUCKET ((BUCKET_UNIT / NUM_OF_THREADS) + 1)
#else
#define HASH_TABLE_BUCKET BUCKET_UNIT
#endif

typedef struct hashTable_s {
#ifdef E_TEST_1
    hashEntry_t hashEntry[HASH_TABLE_BUCKET];
#else
    hashEntry_t *pEntry;
#endif
    unsigned int bucketSize;
    unsigned int tableSize;
#ifdef THD
    hashEntry_t ht[NUM_OF_THREADS][HASH_TABLE_BUCKET];
#endif
} hashTable_t;

entry *findName(char lastName[], entry *pHead);
#ifdef THD
entry *append(char lastName[], entry *e, int thd);
#else
entry *append(char lastName[], entry *e);
#endif

void initHashTable();

#endif
