#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>

#include "seats.h"
#include "waitlist.h"

seat_t* seat_header = NULL;
pthread_mutex_t seatLock = PTHREAD_MUTEX_INITIALIZER;

char seat_state_to_char(seat_state_t);

void list_seats(char* buf, int bufsize)
{
    //Get the lock
    pthread_mutex_lock(&seatLock);

    seat_t* curr = seat_header;
    int index = 0;
    while(curr != NULL && index < bufsize+ strlen("%d %c,"))
    {
        int length = snprintf(buf+index, bufsize-index, 
            "%d %c,", curr->id, seat_state_to_char(curr->state));
        if (length > 0)
            index = index + length;
        curr = curr->next;
    }

    //Release the lock
    pthread_mutex_unlock(&seatLock);

    if (index > 0)
        snprintf(buf+index-1, bufsize-index-1, "\n");
    else
        snprintf(buf, bufsize, "No seats not found\n\n");
}

void view_seat(char* buf, int bufsize,  int seat_id, int customer_id, int customer_priority)
{
    //Get the lock
    pthread_mutex_lock(&seatLock);

    seat_t* curr = seat_header;
    while(curr != NULL)
    {
        if(curr->id == seat_id)
        {
            if(curr->state == AVAILABLE || (curr->state == PENDING && curr->customer_id == customer_id))
            {
                snprintf(buf, bufsize, "Confirm seat: %d %c ?\n\n",
                    curr->id, seat_state_to_char(curr->state));
                curr->state = PENDING;
                curr->customer_id = customer_id;
            }
            else if(curr->state == PENDING && curr->customer_id != customer_id)
            {
                snprintf(buf, bufsize, "Seat unavailable. Press okay to get on  standby list\n\n");
            }
            else
            {
                snprintf(buf, bufsize, "Seat unavailable\n\n");
            }

            //Release the lock
            pthread_mutex_unlock(&seatLock);

            return;
        }
        curr = curr->next;
    }

    //Release the lock
    pthread_mutex_unlock(&seatLock);

    snprintf(buf, bufsize, "Requested seat not found\n\n");
    return;
}

void confirm_seat(char* buf, int bufsize, int seat_id, int customer_id, int customer_priority)
{
    //Get the lock
    pthread_mutex_lock(&seatLock);

    seat_t* curr = seat_header;
    while(curr != NULL)
    {
        if(curr->id == seat_id)
        {
            if(curr->state == PENDING && curr->customer_id == customer_id )
            {
                snprintf(buf, bufsize, "Seat confirmed: %d %c\n\n",
                    curr->id, seat_state_to_char(curr->state));
                curr->state = OCCUPIED;
            }
            else if(curr->customer_id != customer_id && curr->state == PENDING)
            {
                int pos = add_to_waitlist(customer_id, seat_id);
                snprintf(buf, bufsize, "Added to the waitlit at position %d\n\n", pos);
            }
            else if(curr->customer_id != customer_id )
            {
                snprintf(buf, bufsize, "Permission denied - seat held by another user\n\n");
            }
            else if(curr->state != PENDING)
            {
                snprintf(buf, bufsize, "No pending request\n\n");
            }

            handle_waitlist();

            //Release the lock
            pthread_mutex_unlock(&seatLock);

            return;
        }
        curr = curr->next;
    }

    //Release the lock
    pthread_mutex_unlock(&seatLock);

    snprintf(buf, bufsize, "Requested seat not found\n\n");
    
    return;
}

void cancel(char* buf, int bufsize, int seat_id, int customer_id, int customer_priority)
{
    printf("Cancelling seat %d for user %d\n", seat_id, customer_id);

    //Get the lock
    pthread_mutex_lock(&seatLock);

    seat_t* curr = seat_header;
    while(curr != NULL)
    {
        if(curr->id == seat_id)
        {
            if(curr->state == PENDING && curr->customer_id == customer_id )
            {
                snprintf(buf, bufsize, "Seat request cancelled: %d %c\n\n",
                    curr->id, seat_state_to_char(curr->state));
                curr->state = AVAILABLE;
            }
            else if(curr->customer_id != customer_id )
            {
                snprintf(buf, bufsize, "Permission denied - seat held by another user\n\n");
            }
            else if(curr->state != PENDING)
            {
                snprintf(buf, bufsize, "No pending request\n\n");
            }

            handle_waitlist();

            //Release the lock
            pthread_mutex_unlock(&seatLock);

            

            return;
        }
        curr = curr->next;
    }

    //Release the lock
    pthread_mutex_unlock(&seatLock);

    snprintf(buf, bufsize, "Seat not found\n\n");
    
    return;
}

void load_seats(int number_of_seats)
{
    //Initialize lock
    pthread_mutex_init(&seatLock, NULL);

    //Get the lock
    pthread_mutex_lock(&seatLock);

    //Create Seats
    seat_t* curr = NULL;
    int i;
    for(i = 0; i < number_of_seats; i++)
    {   
        seat_t* temp = (seat_t*) malloc(sizeof(seat_t));
        temp->id = i;
        temp->customer_id = -1;
        temp->state = AVAILABLE;
        temp->next = NULL;
        
        if (seat_header == NULL)
        {
            seat_header = temp;
        }
        else
        {
            curr-> next = temp;
        }
        curr = temp;
    }

    //Release the lcok
    pthread_mutex_unlock(&seatLock);
}

void unload_seats()
{
    //Get the lock
    pthread_mutex_lock(&seatLock);

    seat_t* curr = seat_header;
    while(curr != NULL)
    {
        seat_t* temp = curr;
        curr = curr->next;
        free(temp);
    }

    //Release the lock
    pthread_mutex_unlock(&seatLock);

    //destroy the lock
    pthread_mutex_destroy(&seatLock);
}

char seat_state_to_char(seat_state_t state)
{
    switch(state)
    {
        case AVAILABLE:
        return 'A';
        case PENDING:
        return 'P';
        case OCCUPIED:
        return 'O';
    }

    return '?';
}

void handle_waitlist(){



        seat_t* curr = seat_header;
        while(curr != NULL)
        {
            if(curr->state == AVAILABLE)
            {
                int user = remove_from_waitlist(curr->id);

                if(user != -1){
                    curr->customer_id = user;
                    curr->state = OCCUPIED;
                }
            }if(curr->state == OCCUPIED){

                while(remove_from_waitlist(curr->id) != -1);
            }
            curr = curr->next;
        }
    


}