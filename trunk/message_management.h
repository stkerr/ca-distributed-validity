/* 
 * File:   message_management.h
 * Author: stkerr
 *
 * Created on December 6, 2011, 10:06 AM
 */

#ifndef MESSAGE_MANAGEMENT_H
#define	MESSAGE_MANAGEMENT_H

#include <list>
#include <string>
#include <string.h>
#include <stdlib.h>

using namespace std;

/* Functions */
list<struct message> get_messages_over_time(int time);
struct message get_next_message();
bool get_next_message(int waittime, struct message * msg);

void zero_message(struct message*);
void render_message(struct message*);

/* Data structures */
enum MSG_TYPE
{
    QUERY,
    QUERY_RESPONSE,
    UPDATE,
    UPDATE_RESPONSE,
    MEMBER_UPDATE,
    MEMBER_RESPONSE
};
        
struct message {
    char hostname[100]; /* The hostname of the sender */
    char message[256]; /* The message */
    int type; /* Defines which of the union members to use */
};


#endif	/* MESSAGE_MANAGEMENT_H */

