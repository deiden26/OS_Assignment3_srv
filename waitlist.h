

#ifndef _WAITLIST_H_
#define _WAITLIST_H_


int add_to_waitlist(int user, int seat);
int remove_from_waitlist(int seat);


typedef struct Hold
{
    int user;
    int seat;
    struct Hold *next;
}Hold;

extern Hold *waitlist_head;

#endif