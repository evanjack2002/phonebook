#include <stdio.h>
#include <pthread.h>
#include <string.h>

#define DICT_FILE "./dictionary/words.txt"
#define MAX_LAST_NAME_SIZE 16
#define NUM_OF_THREADS 2


typedef struct thread_data_s {
    int start;
    int end;
    long total;
    char **arr;
//    void *aaa;
} thread_data_t;


void print(int *ints, int n);
void *processArray(void *args);
char buf[400000][MAX_LAST_NAME_SIZE];
long ii = 0;

int main(int argc, char *argv[])
{
    FILE *fp;
    char line[MAX_LAST_NAME_SIZE];
    int i = 0;
    int s = 0;

    pthread_t threads[NUM_OF_THREADS];
    thread_data_t thread_data[NUM_OF_THREADS];

    fp = fopen(DICT_FILE, "r");
    if (fp == NULL) {
        printf("cannot open the file\n");
        return -1;
    }

#if 1
    s = 0;
    while (fgets(&(buf[s]), sizeof(line), fp)) {
        while (buf[s][i] != '\0')
            i++;
        buf[s][i - 1] = '\0';
        i = 0;
#if 0
        printf("buf[0]=%s\n", &(buf[s]));
#endif
        s++;
    }
#else
    s = 0;
    while (fgets(line, sizeof(line), fp)) {
        while (line[i] != '\0')
            i++;
        line[i - 1] = '\0';
        i = 0;
        if (s == 0)
            printf("buf[0]=%s\n", line);
        s++;
    }
#endif

    printf("READ File end, s=%d\n", s);

#if 0
    for (i = 0; i < s; i++) {
#if 1
        printf("s[%d]=%s\n", i, &(buf[i]));
#else
        if (strlen(&(buf[i])) == 0 || &(buf[i]) == NULL)
            printf("s[%d]=%s\n", i, &(buf[i]));
#endif
    }
#endif

    int remainingWork = s, amountOfWork;
    int startRange, endRange = -1;

    for (int i = 0; i < NUM_OF_THREADS; i++) {

        amountOfWork = remainingWork / (NUM_OF_THREADS - i);
        startRange = endRange + 1;
        endRange   = startRange + amountOfWork;

        thread_data[i].arr   = &buf;
        thread_data[i].start = startRange;
        thread_data[i].end   = endRange;
        thread_data[i].total   = s;
//       thread_data[i].aaa   = (viod *)buf;


        pthread_create(&threads[i], NULL, processArray, (void *)&thread_data[i]);

        remainingWork -= amountOfWork;
    }

    while (1);


}

void *processArray(void *args)
{
    thread_data_t *data = (thread_data_t *)args;
    char **arr = data->arr;
    int start = data->start;
    int end   = data->end;
    long total = data->total;
#if 0
    printf("pthred_id=0x%lx, start=%d, end=%d, total=%lu\n",
           pthread_self(),
           start,
           end,
           total);
#endif
    // 1. Wait for a signal to start from the main thread

#if 1
    for (int i = start; i < end; i++) {
        printf("pthred_id=%lu, start=%d, end=%d, i=%d, arr[]=(%s)\n",
               pthread_self(),
               start,
               end,
               i,
               &(buf[i]));
    }
#endif

    ii++;

    // 2. Signal to the main thread that you're done

    pthread_exit(NULL);
}

void print(int *ints, int n)
{
    printf("[");
    for (int i = 0; i < n; i++) {
        printf("%d", ints[i]);
        if (i + 1 != n)
            printf(", ");
    }
    printf("]\n");
}