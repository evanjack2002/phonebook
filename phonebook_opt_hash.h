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
typedef struct thread_data_s {
    int start;
    int end;
    long total;
    char **arr;
} thread_data_t;
void *processArray(void *args);
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
#define HASH_TABLE_BUCKET 42737
#else
#define HASH_TABLE_BUCKET 7919
#endif

typedef struct hashTable_s {
#ifdef E_TEST_1
    hashEntry_t hashEntry[HASH_TABLE_BUCKET];
#else
    hashEntry_t *pEntry;
#endif
    unsigned int bucketSize;
    unsigned int tableSize;
} hashTable_t;

entry *findName(char lastName[], entry *pHead);
entry *append(char lastName[], entry *e);

void initHashTable();

#endif
