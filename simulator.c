#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

#define CHAIR_NUM 3

/*
** Maximum of sleeping time of threads
*/
#define SLEEP_MAX 5

void rand_sleep(void);

/*
** @var threads: Array containing student threads
** @var queue: Array containing ready tasks
** @var thread_num: Number of threads
** @var queue_size: Size of queue
** @var head: Index of the first element
** @var tail: Index of the last element
** @var count: Number of readying tasks
** @var has_assistant: Check assistant is there
*/
typedef struct {
	pthread_mutex_t lock;
	pthread_t *threads;
	long unsigned *queue;
	int thread_num;
	int queue_size;
	int head;
	int tail;
	int count;
	int has_assistant;
} threadpool_t;

threadpool_t *threadpool_create(int thread_num, int queue_size);

/* Semaphore */
sem_t sem;

void *threadpool_thread(void *threadpool){
	threadpool_t *pool = (threadpool_t *)threadpool;
	pthread_t id;
	
	/* Current semaphore value */
	int sval;

	id = pthread_self() % 10000;

	while(1){
		if (!pool->has_assistant){
			rand_sleep();
			continue;
		}

		/* Check maximum waiting */
		if (pool->count == pool->queue_size){
			rand_sleep();
			continue;
		}

		/* Lock the thread for adding new thread to queue */
		pthread_mutex_lock(&(pool->lock));

		sem_getvalue(&sem, &sval);

		pool->queue[pool->tail] = id;

		pool->tail += 1;
		pool->tail = pool->tail % pool->queue_size;

		pool->count += 1;

		/* Wakeup TA is sleeping */
		if (sval == 0){
			sem_post(&sem);
		}
		
		pthread_mutex_unlock(&(pool->lock));

		printf("TA helping %lu student and %d students are waiting\n", pool->queue[pool->head], pool->count-1);
		printf("waiting students : [1] %lu [2] %lu [3] %lu \n", pool->queue[(pool->head+1) % pool->queue_size], 
			pool->queue[(pool->head+2) % pool->queue_size], pool->queue[(pool->head+3) % pool->queue_size]);

		/* Running task */
		rand_sleep();
		
		/* Lock the thread for removing thread is finished from queue*/
		pthread_mutex_lock(&(pool->lock));

		printf("%lu student is finish\n\n", pool->queue[pool->head]);
		
		pool->queue[pool->head] = 0;

		pool->head += 1;
		pool->head = pool->head % pool->queue_size;

		pool->count -= 1;	

		pthread_mutex_unlock(&(pool->lock));	
	}
}

void *assistant_thread(void *threadpool){
	threadpool_t *pool = (threadpool_t *)threadpool;

	pool->has_assistant = 1;

	while(1){
		if(pool->count == 0){
			sem_wait(&sem);
		}

		if(pool->count > 0){
			sleep(1000);
		}
	}
}

int main(int argc, char **argv){
	threadpool_t *students;
	pthread_t ta;
	int student_num;
	
	int status;
	int i;

	/* Initialize semaphore */
	if (sem_init(&sem, 0, 1) == -1){
		perror("Error");
		exit(0);
	}

	printf("Type number of student : ");
	scanf("%d", &student_num);	

	puts("start");

	students = threadpool_create(student_num, CHAIR_NUM + 1);

	if(pthread_create(&ta, NULL, assistant_thread, (void *)students) != 0){
		perror("Failed to create thread");
		exit(0);
	}

	for (i = 0; i < student_num; i++){
		pthread_join(students->threads[i], (void *)&status);
	}

	pthread_join(ta, (void *)&status);
	
	pthread_mutex_destroy(&(students->lock));

	return 0;
}

void rand_sleep(void){
	int time = rand() % SLEEP_MAX + 1;
	sleep(time);
}

threadpool_t *threadpool_create(int thread_num, int queue_size){
	threadpool_t *pool;
	int i;

	/* Initialize */
	pool->head = pool->tail = pool->count = 0;
	pool->queue_size = queue_size;

	/* Allocate threads */
	pool->threads = (pthread_t *)malloc(sizeof(pthread_t) * thread_num);
	pool->queue = (long unsigned *)malloc(sizeof(long unsigned) * queue_size);

	if(pthread_mutex_init(&(pool->lock), NULL) != 0) return NULL;

	for(i = 0; i < thread_num; i++){
		if(pthread_create(&(pool->threads[i]), NULL, threadpool_thread, (void*)pool) != 0){
			return NULL;
		}
		pool->thread_num++;
	}

	return pool;
}
