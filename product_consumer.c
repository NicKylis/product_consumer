#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <math.h>

#define QUEUESIZE 10
#define LOOP 100000 // Large number of iterations

typedef struct {
    void * (*work)(void *);
    void *arg;
    struct timeval timestamp;
    int task_id; // Added for tracking tasks
} workFunction;

typedef struct {
    workFunction *buf[QUEUESIZE];
    long head, tail;
    int full, empty;
    pthread_mutex_t *mut;
    pthread_cond_t *notFull, *notEmpty;
} queue;

queue *queueInit(void);
void queueDelete(queue *q);
void queueAdd(queue *q, workFunction *in);
void queueDel(queue *q, workFunction **out);
void *producer(void *args);
void *consumer(void *args);
void *computeSin(void *arg);

// Statistics
pthread_mutex_t stats_mut = PTHREAD_MUTEX_INITIALIZER;
long total_waiting_time = 0;
int count = 0;

// Task ID counter
pthread_mutex_t task_id_mut = PTHREAD_MUTEX_INITIALIZER;
int next_task_id = 1;

int main() {
    queue *fifo;
    int p = 1; // Number of producers
    int q = 1000; // Number of consumers
    pthread_t producers[p];
    pthread_t consumers[q];

    fifo = queueInit();
    if (fifo == NULL) {
        fprintf(stderr, "main: Queue Init failed.\n");
        exit(1);
    }

    // Create producer threads
    for (int i = 0; i < p; i++) {
        pthread_create(&producers[i], NULL, producer, fifo);
    }
    // Create consumer threads
    for (int i = 0; i < q; i++) {
        pthread_create(&consumers[i], NULL, consumer, fifo);
    }

    // Wait for producers to finish
    for (int i = 0; i < p; i++) {
        pthread_join(producers[i], NULL);
    }

    // Send termination signals to consumers
    for (int i = 0; i < q; i++) {
        workFunction *wf = malloc(sizeof(workFunction));
        wf->work = NULL; // Termination signal
        wf->task_id = -1; // Invalid task ID for termination
        pthread_mutex_lock(fifo->mut);
        while (fifo->full) {
            pthread_cond_wait(fifo->notFull, fifo->mut);
        }
        queueAdd(fifo, wf);
        pthread_mutex_unlock(fifo->mut);
        pthread_cond_signal(fifo->notEmpty);
    }

    // Wait for consumers to finish
    for (int i = 0; i < q; i++) {
        pthread_join(consumers[i], NULL);
    }

    // Print statistics
    if (count > 0) {
        long average_waiting_time = total_waiting_time / count;
        printf("Average waiting time: %ld microseconds\n", average_waiting_time);
    }

    queueDelete(fifo);
    return 0;
}

void *producer(void *q) {
    queue *fifo = (queue *)q;
    for (int i = 0; i < LOOP; i++) {
        double *angles = malloc(10 * sizeof(double));
        for (int j = 0; j < 10; j++) {
            angles[j] = (double)(rand() % 360) * 3.14 / 180.0;
        }
        workFunction *wf = malloc(sizeof(workFunction));
        wf->work = computeSin;
        wf->arg = angles;
        gettimeofday(&wf->timestamp, NULL);

        // Assign unique task ID
        pthread_mutex_lock(&task_id_mut);
        wf->task_id = next_task_id++;
        pthread_mutex_unlock(&task_id_mut);

        pthread_mutex_lock(fifo->mut);
        while (fifo->full) {
            pthread_cond_wait(fifo->notFull, fifo->mut);
        }
        queueAdd(fifo, wf);
        // Print when producer adds to queue
        printf("Producer added task ID %d to queue\n", wf->task_id);
        pthread_mutex_unlock(fifo->mut);
        pthread_cond_signal(fifo->notEmpty);
    }
    return NULL;
}

void *consumer(void *q) {
    queue *fifo = (queue *)q;
    while (1) {
        pthread_mutex_lock(fifo->mut);
        while (fifo->empty) {
            pthread_cond_wait(fifo->notEmpty, fifo->mut);
        }
        workFunction *wf;
        queueDel(fifo, &wf);
        // Print when consumer removes from queue
        printf("Consumer removed task ID %d from queue\n", wf->task_id);
        pthread_mutex_unlock(fifo->mut);
        pthread_cond_signal(fifo->notFull);

        if (wf->work == NULL) {
            free(wf);
            break; // Exit on termination signal
        }

        // Calculate waiting time only for valid tasks
        struct timeval now;
        gettimeofday(&now, NULL);
        long waiting_time = (now.tv_sec - wf->timestamp.tv_sec) * 1000000 + (now.tv_usec - wf->timestamp.tv_usec);

        // Ensure waiting_time is non-negative
        if (waiting_time >= 0) {
            pthread_mutex_lock(&stats_mut);
            total_waiting_time += waiting_time;
            count++;
            pthread_mutex_unlock(&stats_mut);
        } else {
            printf("Warning: Negative waiting time for task ID %d\n", wf->task_id);
        }

        // Execute the function
        wf->work(wf->arg);
        free(wf->arg);
        free(wf);
    }
    return NULL;
}

void *computeSin(void *arg) {
    double *angles = (double *)arg;
    for (int i = 0; i < 10; i++) {
        double result = sin(angles[i]);
        // Simple computation, no output to avoid clutter
    }
    return NULL;
}

queue *queueInit(void) {
    queue *q = (queue *)malloc(sizeof(queue));
    if (q == NULL) return NULL;
    q->empty = 1;
    q->full = 0;
    q->head = 0;
    q->tail = 0;
    q->mut = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t));
    pthread_mutex_init(q->mut, NULL);
    q->notFull = (pthread_cond_t *)malloc(sizeof(pthread_cond_t));
    pthread_cond_init(q->notFull, NULL);
    q->notEmpty = (pthread_cond_t *)malloc(sizeof(pthread_cond_t));
    pthread_cond_init(q->notEmpty, NULL);
    return q;
}

void queueDelete(queue *q) {
    pthread_mutex_destroy(q->mut);
    free(q->mut);
    pthread_cond_destroy(q->notFull);
    free(q->notFull);
    pthread_cond_destroy(q->notEmpty);
    free(q->notEmpty);
    free(q);
}

void queueAdd(queue *q, workFunction *in) {
    q->buf[q->tail] = in;
    q->tail++;
    if (q->tail == QUEUESIZE) q->tail = 0;
    if (q->tail == q->head) q->full = 1;
    q->empty = 0;
}

void queueDel(queue *q, workFunction **out) {
    *out = q->buf[q->head];
    q->head++;
    if (q->head == QUEUESIZE) q->head = 0;
    if (q->head == q->tail) q->empty = 1;
    q->full = 0;
}