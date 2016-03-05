#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <assert.h>

#ifdef THD
#include <unistd.h>
#include <pthread.h>
#endif

#include IMPL

#define DICT_FILE "./dictionary/words.txt"

#ifdef THD
char buf[MAX_BUFFER_SIZE][MAX_LAST_NAME_SIZE];
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
unsigned int running_threads = 0;
struct timespec start_thd, end_thd;
double cpu_time_thd;
void *processArray(void * args);
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
    for (int i = 0; i < 9; i++) {
        assert(0 == strcmp(findName(test[i], pHead)->lastName, test[i]));
    }
}

int main(int argc, char *argv[])
{
    FILE *fp;
    int i = 0;
#ifdef THD
    unsigned int s = 0;
    pthread_t threads[NUM_OF_THREADS];
    thread_data_t thread_data[NUM_OF_THREADS];
#else
    char line[MAX_LAST_NAME_SIZE];
#endif
    struct timespec start, end;
    double cpu_time1, cpu_time2;

#ifdef DEBUG
    struct timespec start1, end1;
    struct timespec start2, end2;
    double cpu_time3;
    //double cpu_time4;
    clock_gettime(CLOCK_REALTIME, &start1);
#endif

#ifdef THD
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
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
    int ss = 0;
    int dd = 0;
    void *tret;

    clock_gettime(CLOCK_REALTIME, &start);
    while (fgets((char *) &(buf[s]), MAX_LAST_NAME_SIZE, fp)) {
        while (buf[s][i] != '\0')
            i++;
        buf[s][i - 1] = '\0';
        i = 0;
        s++;

        if((s % LINE_E) ==0) {
#if 1
            if(ss >= NUM_OF_THREADS) {
                ss=0;
//                pthread_join(threads[ss], &tret);
                for (int i = 0; i < NUM_OF_THREADS; i++) {
                    pthread_join(threads[i], &tret);
                }
            }
#endif
            thread_data[ss].start = dd*LINE_E;
            thread_data[ss].end   = s-1;
            thread_data[ss].thd   = ss;
            pthread_create(&threads[ss], NULL, processArray, (void *)&thread_data[ss]);
            dd++;
            ss++;
        }
    }


    if((s % LINE_E) !=0) {
#if 1
        if(ss>=NUM_OF_THREADS) {
            ss=0;
            pthread_join(threads[ss], &tret);
        }
#endif
        thread_data[ss].start = dd*LINE_E;
        thread_data[ss].end   = s-1;
        thread_data[ss].thd   = ss;
        pthread_create(&threads[ss], NULL, processArray, (void *)&thread_data[ss]);
    }

#if 0 /* Evan: TEST */
    printf("\r\n(%s:%d)---> ss=%d", __FUNCTION__, __LINE__,
           ss);
#endif
    running_threads = ss;
#if 0
    while(running_threads)
        usleep(10);
#else
#if 0
    pthread_join(threads[ss], &tret);
#else
    for (int i = 0; i <= ss; i++) {
        pthread_join(threads[i], &tret);
#if 0
        printf("(%s:%d)---> NUM_OF_THREADS=%d, tret=%d\n", __FUNCTION__, __LINE__,
               NUM_OF_THREADS,
               tret);
#endif
    }
#endif
#endif

//    pthread_exit(NULL);

#ifdef DEBUG2
    printf("-----------------------------------------------------------\n");
#endif

    clock_gettime(CLOCK_REALTIME, &end);
    cpu_time1 = diff_in_second(start, end);
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

#ifdef DEBUG2
    printf("-----------------------------------------------------------\n");
#endif

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
#elif defined(THD2)
    output = fopen("opt_thd2.txt", "a");
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
//    cpu_time4 = diff_in_second(start2, end2);
#endif

    printf("execution time of append() : %lf sec\n", cpu_time1);
    printf("execution time of findName() : %lf sec\n", cpu_time2);

    if (pHead->pNext) free(pHead->pNext);
    free(pHead);

#ifdef DEBUG
    clock_gettime(CLOCK_REALTIME, &end1);
    cpu_time3 = diff_in_second(start1, end1);
    printf("execution time of total : %lf sec\n", cpu_time3);
//printf("execution time of test() : %lf sec\n", cpu_time4);
#if 0
#ifdef THD
    cpu_time_thd = cpu_time_thd / NUM_OF_THREADS;
//    printf("execution time of pthread : %lf sec\n", cpu_time_thd);
#endif
#endif
#endif

    return 0;
}

#ifdef THD
void *processArray(void *args)
{
    thread_data_t *data = (thread_data_t *)args;
//    char **arr = data->arr;
    int start = data->start;
    int end   = data->end;
    int thd = data->thd;
    int i = 0;
    entry *e = NULL;

#if 1
    printf("(%s:%d) ---> pthred_id=0x%lx, start=%d, end=%d, thd=%d\n",__FUNCTION__,__LINE__,
           pthread_self(),
           start,
           end,
           thd);
#endif
//    pthread_detach(pthread_self());
//    pthread_mutex_lock(& mutex);

    clock_gettime(CLOCK_REALTIME, &start_thd);
    for (i = start; i < end; i++) {
        e = append((char *) & (buf[i]), e, thd);
    }

    clock_gettime(CLOCK_REALTIME, &end_thd);
    cpu_time_thd += diff_in_second(start_thd, end_thd);

//    pthread_mutex_unlock(& mutex);

#if 0
    pthread_mutex_lock(& mutex);
    if(running_threads)
        running_threads--;
    pthread_mutex_unlock(& mutex);
#endif

    pthread_exit(NULL);
}
#endif

