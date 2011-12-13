#include "message_management.h"
#include "networking.h"

#include <strings.h>
#include <unistd.h>
#include <string>
#include <iostream>
#include <time.h>
#include <fstream>
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


