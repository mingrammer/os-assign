#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>

#define NUM_STU 10
#define NUM_SEAT 3

sem_t sem_stu;
sem_t sem_ta;
//sem_t mutex;
pthread_mutex_t mutex;

int chair[3];
int count = 0; //number of waiting students
int nextseat = 0;
int nextteach = 0;

void* stu_programming(void* i);
void* ta_teaching();

int main(int argc, char **argv){

	//variable declaration
	pthread_t students[NUM_STU];
	pthread_t ta;

	int i=0;
	int idx[NUM_STU]={0};

	//init
	sem_init(&sem_stu,0,0);
	sem_init(&sem_ta,0,1);
	//sem_init(&mutex,0,1);
	pthread_mutex_init(&mutex,NULL);


	//create thread
	pthread_create(&ta,NULL,ta_teaching,NULL);

	for(i=0; i<NUM_STU; i++)
	{
		idx[i]=i+1;
		pthread_create(&students[i],NULL,stu_programming,(void*)&idx[i]);
	} 
	

	pthread_join(ta,NULL);
	for(i=0; i<NUM_STU;i++)
	{
		pthread_join(students[i],NULL);
	}	
	
	
	return 0;
}

void* stu_programming(void* i)
{
	int id = *(int*)i;

	printf("[stu] student %d is programming\n",id);		
	
	srand(time(NULL));

	while(1)
	{
		sleep(rand()%15 + 1);

		//sem_wait(&mutex);
		pthread_mutex_lock(&mutex);
		if(count < NUM_SEAT)	
		{
			chair[nextseat]=id;
			count++;
			printf("	[stu] student %d is waiting\n",id);
			printf("waiting students : [1] %d\t[2]%d\t[3]%d\n",chair[0],chair[1],chair[2]);
			nextseat = (nextseat+1) % NUM_SEAT;
			
			//sem_post(&mutex);
			pthread_mutex_unlock(&mutex);
			
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
		printf("		[ta] TA is teaching student %d\n",chair[nextteach]);	
		chair[nextteach]=0;
		count--;
		printf("waiting students : [1] %d\t[2]%d\t[3]%d\n",chair[0],chair[1],chair[2]);
		nextteach = (nextteach+1) % NUM_SEAT;
		sleep(2);
		printf("		[ta] teaching finish.\n");	
		//sem_post(&mutex);
		pthread_mutex_unlock(&mutex);

		sem_post(&sem_ta);
	}
	
}
