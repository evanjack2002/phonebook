#ifndef _PHONEBOOK_H
#define _PHONEBOOK_H

#define MAX_LAST_NAME_SIZE 16

/* TODO: After modifying the original version, uncomment the following
 * line to set OPT properly */
#define HASH 1

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

//#define HASH_TABLE_BUCKET 42737
#define HASH_TABLE_BUCKET 7919

typedef struct hashTable_s {
    hashEntry_t hashEntry[HASH_TABLE_BUCKET];
    unsigned int tableSize;
} hashTable_t;

entry *findName(char lastName[], entry *pHead);
entry *append(char lastName[], entry *e);

void initHashTable();

#endif
