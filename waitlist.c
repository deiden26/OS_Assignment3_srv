





#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#include "waitlist.h"

pthread_mutex_t waitlist_lock = PTHREAD_MUTEX_INITIALIZER;



Hold *waitlist_head = NULL;

int add_to_waitlist(int user, int seat){

	pthread_mutex_lock(&waitlist_lock);//course grained lock

	Hold *new = malloc(sizeof(Hold));
	new->user = user;
	new->seat = seat;
	new->next = NULL;

	Hold *curr = waitlist_head;
	int counter = 0;

	if(waitlist_head == NULL){

		waitlist_head = new;
		pthread_mutex_unlock(&waitlist_lock);
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

		pthread_mutex_unlock(&waitlist_lock);

		return counter;
	}

}

int remove_from_waitlist(int seat){

	pthread_mutex_lock(&waitlist_lock);

	if(waitlist_head == NULL){
		pthread_mutex_unlock(&waitlist_lock);
		return -1;
	}

	Hold *curr = waitlist_head;
	Hold *prev = NULL;

	while(curr->seat != seat){
		prev = curr;
		curr = curr->next;

		if(curr == NULL){
			pthread_mutex_unlock(&waitlist_lock);
			return -1;
		}
	}

	int user = curr->user;

	if(prev == NULL)
		waitlist_head = NULL;
	else
		prev->next = curr->next;

	pthread_mutex_unlock(&waitlist_lock);

	free(curr);

	return user;

}