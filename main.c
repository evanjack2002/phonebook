#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <assert.h>

#ifdef THD
#include <pthread.h>
#endif

#include IMPL

#define DICT_FILE "./dictionary/words.txt"

#ifdef THD
char buf[400000][MAX_LAST_NAME_SIZE];
//pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t r_mutex = PTHREAD_MUTEX_INITIALIZER;
unsigned int running_threads = 0;
#define NUM_OF_THREADS 4
#endif

static double diff_in_second(struct timespec t1, struct timespec t2)
{
    struct timespec diff;
    if (t2.tv_nsec - t1.tv_nsec < 0) {
        diff.tv_sec  = t2.tv_sec - t1.tv_sec - 1;
        diff.tv_nsec = t2.tv_nsec - t1.tv_nsec + 1000000000;
    } else {
        diff.tv_sec  = t2.tv_sec - t1.tv_sec;
        diff.tv_nsec = t2.tv_nsec - t1.tv_nsec;
    }
    return (diff.tv_sec + diff.tv_nsec / 1000000000.0);
}

void test(entry *pHead)
{
    char test[9][MAX_LAST_NAME_SIZE] = {
        "uninvolved",
        "zyxel",
        "whiteshank",
        "odontomous",
        "pungoteague",
        "reweighted",
        "xiphisternal",
        "aaaaah",
        "yakattalo"
    };

    entry *e;
    for (int i = 0; i < 9; i++) {
        e = findName(test[i], pHead);
        if (e) {
#if 1
            printf("Found ---> input=(%s), lastName=(%s)\n",
                   test[i],
                   e->lastName);
#endif
        } else
            printf("Not Found ---> input=(%s)\n",
                   test[i]);
    }
}

#ifdef THD
struct timespec start_thd, end_thd;
double cpu_time_thd;
#endif

int main(int argc, char *argv[])
{
    FILE *fp;
    int i = 0;
    char line[MAX_LAST_NAME_SIZE];
    struct timespec start, end;
    double cpu_time1, cpu_time2;

#ifdef THD
//    char buf[400000][MAX_LAST_NAME_SIZE];
    unsigned int s = 0;
    pthread_t threads[NUM_OF_THREADS];
    thread_data_t thread_data[NUM_OF_THREADS];
    int remainingWork, amountOfWork;
    int startRange, endRange = -1;
#endif

#ifdef DEBUG
    struct timespec start1, end1;
    struct timespec start2, end2;
    double cpu_time3, cpu_time4;
    clock_gettime(CLOCK_REALTIME, &start1);
#endif

    /* check file opening */
    fp = fopen(DICT_FILE, "r");
    if (fp == NULL) {
        printf("cannot open the file\n");
        return -1;
    }

#if defined(HASH1) || defined(HASH2)
    initHashTable();
#endif

    /* build the entry */
    entry *pHead, *e;
    pHead = (entry *) malloc(sizeof(entry));
    printf("size of entry : %lu bytes\n", sizeof(entry));
    e = pHead;
    e->pNext = NULL;

#if defined(__GNUC__)
    __builtin___clear_cache((char *) pHead, (char *) pHead + sizeof(entry));
#endif
#ifdef THD
    clock_gettime(CLOCK_REALTIME, &start);
    while (fgets(&(buf[s]), MAX_LAST_NAME_SIZE, fp)) {
        while (buf[s][i] != '\0')
            i++;
        buf[s][i - 1] = '\0';
        i = 0;
        s++;
    }

    remainingWork = s;
    for (int i = 0; i < NUM_OF_THREADS; i++) {

        amountOfWork = remainingWork / (NUM_OF_THREADS - i);
        startRange = endRange + 1;
        endRange   = startRange + amountOfWork;

//        thread_data[i].arr   = (char **)buf;
        thread_data[i].start = startRange;
        thread_data[i].end   = endRange;
//        thread_data[i].total = s;

        pthread_mutex_lock(& r_mutex);
        running_threads++;
        pthread_mutex_unlock(& r_mutex);

        pthread_create(&threads[i], NULL, processArray, (void *)&thread_data[i]);

        remainingWork -= amountOfWork;
    }

    while (running_threads > 0)
        usleep(10);

    clock_gettime(CLOCK_REALTIME, &end);
    cpu_time1 = diff_in_second(start, end);

    cpu_time_thd = cpu_time_thd / NUM_OF_THREADS;
#else
    clock_gettime(CLOCK_REALTIME, &start);
    while (fgets(line, sizeof(line), fp)) {
        while (line[i] != '\0')
            i++;
        line[i - 1] = '\0';
        i = 0;
        e = append(line, e);
    }
    clock_gettime(CLOCK_REALTIME, &end);
    cpu_time1 = diff_in_second(start, end);
#endif

    /* close file as soon as possible */
    fclose(fp);

    e = pHead;

    /* the givn last name to find */
    char input[MAX_LAST_NAME_SIZE] = "zyxel";
    e = pHead;

    assert(findName(input, e) &&
           "Did you implement findName() in " IMPL "?");
    assert(0 == strcmp(findName(input, e)->lastName, "zyxel"));

#if defined(__GNUC__)
    __builtin___clear_cache((char *) pHead, (char *) pHead + sizeof(entry));
#endif
    /* compute the execution time */
    clock_gettime(CLOCK_REALTIME, &start);
    findName(input, e);
    clock_gettime(CLOCK_REALTIME, &end);
    cpu_time2 = diff_in_second(start, end);

    FILE *output;
#if defined(OPT)
    output = fopen("opt.txt", "a");
#elif defined(THD)
    output = fopen("opt_thd.txt", "a");
#elif defined(HASH1)
    output = fopen("opt_hash1.txt", "a");
#elif defined(HASH2)
    output = fopen("opt_hash2.txt", "a");
#else
    output = fopen("orig.txt", "a");
#endif
    fprintf(output, "append() findName() %lf %lf\n", cpu_time1, cpu_time2);
    fclose(output);

#ifdef DEBUG
    clock_gettime(CLOCK_REALTIME, &start2);
    test(pHead);
    clock_gettime(CLOCK_REALTIME, &end2);
    cpu_time4 = diff_in_second(start2, end2);
#endif

#ifdef THD
    printf("execution time of pthread : %lf sec\n", cpu_time_thd);
#endif
    printf("execution time of append() : %lf sec\n", cpu_time1);
    printf("execution time of findName() : %lf sec\n", cpu_time2);

    if (pHead->pNext) free(pHead->pNext);
    free(pHead);

#ifdef DEBUG
    clock_gettime(CLOCK_REALTIME, &end1);
    cpu_time3 = diff_in_second(start1, end1);
    printf("execution time of test() : %lf sec\n", cpu_time4);
    printf("execution time of total : %lf sec\n", cpu_time3);
#endif

    return 0;
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

    pthread_mutex_lock(& r_mutex);

#if 0
    printf("pthred_id=%lu, start=%d, end=%d, total=%lu\n",
           pthread_self(),
           start,
           end,
           total);
#endif

    // 1. Wait for a signal to start from the main thread
    clock_gettime(CLOCK_REALTIME, &start_thd);
    for (i = start; i < end; i++) {
#if 0
//        if (strlen(&buf[i]) == 0)
        printf("pthred_id=0x%lx, bSize=%d, slot=%d, arr[%d]=(%s))\n",
               pthread_self(),
               hashTable.bucketSize,
               (hashTable.pEntry + hashFunc(&(buf[i]), &hashTable))->slot,
               i,
               &(buf[i]));
#endif

        e = append(&(buf[i]), e);
    }
    clock_gettime(CLOCK_REALTIME, &end_thd);
    cpu_time_thd += diff_in_second(start_thd, end_thd);

    if (running_threads > 0)
        running_threads--;

    pthread_mutex_unlock(& r_mutex);

    // 2. Signal to the main thread that you're done
    pthread_exit(NULL);
}
#endif