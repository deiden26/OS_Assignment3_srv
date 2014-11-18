





#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#include "waitlist.h"
#include "semaphore.h"


Hold *waitlist_head = NULL;

m_sem_t *sem = NULL;

void init_waitlist()
{
	if(sem==NULL)
		sem = sem_init(1);
}


int add_to_waitlist(int user, int seat){

	sem_wait(sem);

	Hold *new = malloc(sizeof(Hold));
	new->user = user;
	new->seat = seat;
	new->next = NULL;

	Hold *curr = waitlist_head;
	int counter = 0;

	if(waitlist_head == NULL){

		waitlist_head = new;
		sem_post(sem);
		return counter;
	}else{

		if(curr->seat == seat)
				counter++;

		while(curr->next != NULL){
			if(curr->seat == seat)
				counter++;
			curr = curr->next;
		}

		curr->next = new;

		sem_post(sem);

		return counter;
	}

}

int remove_from_waitlist(int seat){

	sem_wait(sem);

	if(waitlist_head == NULL){
		sem_post(sem);
		return -1;
	}

	Hold *curr = waitlist_head;
	Hold *prev = NULL;

	while(curr->seat != seat){
		prev = curr;
		curr = curr->next;

		if(curr == NULL){
			sem_post(sem);
			return -1;
		}
	}

	int user = curr->user;

	if(prev == NULL)
		waitlist_head = NULL;
	else
		prev->next = curr->next;

	sem_post(sem);

	free(curr);

	return user;

}