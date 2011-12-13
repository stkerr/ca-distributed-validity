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

#define MESSAGE_FIELD_SIZE 1024

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
    QUERY,              // <message, argument> => <key in question, N/A>
    QUERY_SUCCEEDED,    // <message, argument> => <sender, signed key>
    QUERY_FAILED,       // <message, argument> => <N/A, N/A>
    
    UPDATE,
    UPDATE_RESPONSE,
    
    MEMBER_UPDATE,
    MEMBER_RESPONSE,
    
    QUERY_INTERNAL,             // <message, argument> => <public key, signed key>
    QUERY_INTERNAL_RESPONSE,    // <message, argument> => <public key, signed key>
    
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
    int length; /* The length argument. Used for multiple purposes. */
    unsigned char message[MESSAGE_FIELD_SIZE]; /* The message */
    int arg_length;
    unsigned char argument[MESSAGE_FIELD_SIZE];
};


#endif	/* MESSAGE_MANAGEMENT_H */

