#include "../ca_code.h"

#include <stdlib.h>

/* 
 * Iterates through the SEND_QUERY state table.
 * This client will be used for querying all other hosts
 */
void query_internal(struct message *msg)
{
    RSA* key_to_verify;
    
    int state = 1;
    switch(state)
    {
        case 1:
        {
            char buffer[1024];
            memcpy(buffer,msg->message,1024);
            break;
        }
        case 2:
        {
            break;
        }
        case 3:
        {
            break;
        }
        case 4:
        {
            break;
        }
        case 5:
        {
            break;
        }
        case 6:
        {
            break;
        }
        case 7:
        {
            break;
        }
        
    }
}
