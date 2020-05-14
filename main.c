#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

#define MAX_BUF 100
typedef struct LinkedList{
	int data;
	struct LinkedList *next;
} LinkedList;

LinkedList *head,  *tail, *temp = NULL;

void *producer(void*);
void *consumer(void*);


//shared variables
int buffer[MAX_BUF];
int count = 0;
int in = -1, out = -1;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t buffer_has_space = PTHREAD_COND_INITIALIZER;
pthread_cond_t buffer_has_data = PTHREAD_COND_INITIALIZER;

int main(void){
	int i;
	pthread_t threads[2];
	pthread_create ( &threads[0], NULL, producer, NULL);
	pthread_create ( &threads[1], NULL, consumer, NULL);
	for(i =0; i< 2; i++)
		pthread_join( threads[i], NULL);
	return 0;
}

void *producer ( void *v){
	int i;
	int num = 0;

	for( i =0; i< 100; i++){
		pthread_mutex_lock( &mutex);
		if ( count == MAX_BUF)
			pthread_cond_wait( &buffer_has_space, &mutex);

		buffer[i] = rand()%10;

		if( head == NULL){
			temp = (LinkedList *)malloc(sizeof(LinkedList));
			temp -> data = buffer[i];
			temp -> next = NULL;
			head = temp;
			tail = head;
		}

		temp = (LinkedList *)malloc(sizeof(LinkedList));
		temp -> data = buffer[i];
		
		head -> next = temp;
		head = temp;

		count++;

		pthread_cond_signal( &buffer_has_data);
		pthread_mutex_unlock( &mutex);
		printf("Producer : %d, Data = %d\n", i, buffer[i]);
	}
}

void *consumer( void *v){
	int i, x;

	for( i =0; i < 100; i++){
		pthread_mutex_lock( &mutex);
		if( count == 0)
			pthread_cond_wait( &buffer_has_data, &mutex);

		temp = tail;
		tail = tail -> next;
		x = temp -> data;

		free(temp);
		count--;

		if(tail == NULL)
			pthread_cond_wait( &buffer_has_data, &mutex);

		pthread_cond_signal( &buffer_has_space);
		pthread_mutex_unlock( &mutex);
		printf("Consumer : %d, Data = %d\n", i, x);
	}
}
