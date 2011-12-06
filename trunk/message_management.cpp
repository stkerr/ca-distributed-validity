#include "message_management.h"
#include "networking.h"

#include <strings.h>
#include <unistd.h>
#include <string>
#include <iostream>
#include <time.h>
#include <fstream>

/* Returns all messages received over a given length of time */
std::list<struct message> get_messages_over_time(int waittime)
{
    if (waittime < 0)
        waittime = 2;

    time_t timer = time(NULL);

    std::list<struct message> returnable;

    while (1)
    {
        time_t curr_time = time(NULL);
        double diff = difftime(curr_time, timer);

        if (received_messages.size() > 0)
        {
            returnable.push_back(received_messages.front());
            received_messages.pop_front();
        }

        /* Time out, we're done */
        if (diff > waittime)
        {
            break;
        }
    }

    return returnable;
}

/* 
 * Blocks until a message is received and returns that message.
 * Will return immediately upon detecting commander death
 */
struct message get_next_message()
{
    /* Wait for a message */
    while (received_messages.size() == 0)
    {
    }

    struct message msg = received_messages.front();

    received_messages.pop_front();

    return msg;
}

/* 
 * Waits for a message is received and returns that message or at a timeout.
 * Will return immediately upon detecting commander death
 */
bool get_next_message(int waittime, struct message * msg)
{
    if (waittime < 0)
        waittime = 2;

    time_t timer = time(NULL);

    while (1)
    {
        time_t curr_time = time(NULL);
        double diff = difftime(curr_time, timer);

        /* Time out, we're done */
        if (diff > waittime)
        {
            break;
        }

        if (received_messages.size() > 0)
        {
            *msg = received_messages.front();
            received_messages.pop_front();
            return true;
        }
    }

    return false;
}

/* 
 * Given a specified message block,
 * zero out the various fields
 */
void zero_message(struct message* msg)
{
    bzero(msg->hostname, 100);
    bzero(msg->message, 256);
}

void render_message(struct message* msg)
{
    return;
    //#ifndef DEBUG_MODE
    //    return;
    //#endif
    std::cout << "------Rendering message------" << std::endl;
    std::cout << "Hostname: " << msg->hostname << std::endl;
    std::cout << "Message: " << msg->message << std::endl;

    switch (msg->type)
    {
    }
}


