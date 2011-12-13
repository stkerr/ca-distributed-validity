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
    MEMBER_RESPONSE,
    QUERY_INTERNAL,
    QUERY_INTERNAL_RESPONSE,
    UPDATE_INTERNAL,
    UPDATE_INTERNAL_RESPONSE,
    MEMBER_UPDATE_INTERNAL,
    MEMBER_UPDATE_INTERNAL_RESPONSE,
    QUERY_RECEIVED_INTERNAL,
    QUERY_RECEIVED_RESPONSE
};
        
struct message {
    char hostname[100]; /* The hostname of the sender */
    int type; /* Defines which of the union members to use */
    char message[1024]; /* The message */
    char argument[1024];
};


#endif	/* MESSAGE_MANAGEMENT_H */

