#include <stdio.h>
#include <stdlib.h>
#include <string.h> // memset
#include <pthread.h> // pthread_t, pthread_create, pthread_join
#include <semaphore.h> // sem_init, sem_wait, sem_post
#include <time.h>

#define NUM_SEAT 3

#define SLEEP_MAX 5

//sematphores
sem_t sem_stu;
sem_t sem_ta;

//sem_t mutex;
pthread_mutex_t mutex;

int chair[3];
int count = 0; //number of waiting students
int next_seat = 0;
int next_teach = 0;

void rand_sleep(void);
void* stu_programming(void* stu_id);
void* ta_teaching();

int main(int argc, char **argv){

	//thread(s)
	pthread_t *students;
	pthread_t ta;
	
	int* student_ids;
	int student_num;
	
	//index
	int i;

	//get number of students from user
	printf("How many students? ");
	scanf("%d", &student_num);

	//initialize
	students = (pthread_t*)malloc(sizeof(pthread_t) * student_num);
	student_ids = (int*)malloc(sizeof(int) * student_num);

	memset(student_ids, 0, student_num);

	sem_init(&sem_stu,0,0);
	sem_init(&sem_ta,0,1);
	
	//set random
	srand(time(NULL));

	//sem_init(&mutex,0,1);
	pthread_mutex_init(&mutex,NULL);

	//create thread
	pthread_create(&ta,NULL,ta_teaching,NULL);

	//create threads
	for(i=0; i<student_num; i++)
	{
		student_ids[i] = i+1;
		pthread_create(&students[i], NULL, stu_programming, (void*) &student_ids[i]);
	} 
	
	pthread_join(ta, NULL);
	
	for(i=0; i<student_num;i++)
	{
		pthread_join(students[i],NULL);
	}	
	
	return 0;
}

void* stu_programming(void* stu_id)
{
	int id = *(int*)stu_id;

	printf("[stu] student %d is programming\n",id);		
	
	while(1)
	{
		rand_sleep();

		//sem_wait(&mutex);
		pthread_mutex_lock(&mutex);

		if(count < NUM_SEAT)	
		{
			chair[next_seat] = id;
			count++;
			
			printf("	[stu] student %d is waiting\n",id);
			printf("waiting students : [1] %d [2] %d [3] %d\n",chair[0],chair[1],chair[2]);
			next_seat = (next_seat+1) % NUM_SEAT;
			
			//sem_post(&mutex);
			pthread_mutex_unlock(&mutex);
	
			//wakeup ta
			sem_post(&sem_stu);
			sem_wait(&sem_ta);
		}
		else //no more chairs
		{
			//sem_post(&mutex);
			pthread_mutex_unlock(&mutex);
			
			printf("[stu] no more chairs. student %d is programming\n",id);		
		}
	}				
}

void* ta_teaching()
{
	while(1)
	{
		sem_wait(&sem_stu);	
		
		//sem_wait(&mutex);
		pthread_mutex_lock(&mutex);
		
		printf("		[ta] TA is teaching student %d\n",chair[next_teach]);	
		chair[next_teach]=0;
		count--;
		
		printf("waiting students : [1] %d [2] %d [3] %d\n",chair[0],chair[1],chair[2]);
		next_teach = (next_teach + 1) % NUM_SEAT;
		
		rand_sleep();

		printf("		[ta] teaching finish.\n");	

		//sem_post(&mutex);
		pthread_mutex_unlock(&mutex);

		sem_post(&sem_ta);
	}	
}

void rand_sleep(void){
	int time = rand() % SLEEP_MAX + 1;
	sleep(time);
}
