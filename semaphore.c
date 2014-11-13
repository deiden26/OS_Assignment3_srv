#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>


typedef struct m_sem_t {
    int value;
    pthread_mutex_t lock;
} m_sem_t;


int sem_wait(m_sem_t *s){
	
	while(s->value == 0){;}

	pthread_mutex_lock(&(s->lock));

	s->value = s->value-1;

	pthread_mutex_unlock(&(s->lock));

}


int sem_post(m_sem_t *s){
	pthread_mutex_lock(&(s->lock));

	s->value = s->value+1;

	pthread_mutex_unlock(&(s->lock));

}

