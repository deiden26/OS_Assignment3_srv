#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>


typedef struct m_sem_t {
    int value;
    pthread_mutex_t lock;
    pthread_cond_t cond;
} m_sem_t;


m_sem_t* sem_init(){

	m_sem_t *sem = malloc(sizeof(m_sem_t));
	sem->value=0;
	pthread_mutex_init(&(sem->lock), NULL);
	pthread_cond_init(&(sem->cond), NULL);
	return sem;
}

void sem_destroy(m_sem_t* sem){
	pthread_mutex_destroy(&(sem->lock));
	pthread_cond_destroy(&(sem->cond));
	free(sem);
}


int sem_wait(m_sem_t *s){


	pthread_mutex_lock(&(s->lock));

	while(s->value == 0){
		pthread_cond_wait(&(s->cond), &(s->lock)); 
	}

	s->value = s->value-1;

	pthread_mutex_unlock(&(s->lock));

	return 0;

}


int sem_post(m_sem_t *s){
	pthread_mutex_lock(&(s->lock));

	s->value = s->value+1;

	pthread_cond_signal(&(s->cond)); 

	pthread_mutex_unlock(&(s->lock));

	return 0;

}

