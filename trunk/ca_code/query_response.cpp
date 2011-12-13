#include "../ca_code.h"

#include <stdlib.h>

/* 
 * Iterates through the SEND_QUERY state table.
 * This client will be used for querying all other hosts
 */
void query_internal(struct message *request)
{
    RSA* key_to_verify;

    unsigned char *signed_data; // (K_pub)_C
    unsigned int *signed_length;

    list<struct message> responses;

    int state = 1;
    while (state > 0)
    {
        cout << "I am in query_internal() state " << state << endl;;
        
        switch (state)
        {
            case 1:
            {
                char* buffer = (char*) malloc(sizeof (char) *request->length);
                memcpy(buffer, request->message, request->length);

                unsigned char hash[20];
                if (!SHA1(request->message, request->length, hash))
                {
                    cout << "SHA-1 failed!" << endl;
                }

                RSA_sign(NID_sha1, hash, 20, signed_data, signed_length, my_priv_key);

                /* Advance to the next state */
                state = 2;

                break;
            }
            case 2:
            {
                /* Create the Q_i, L message */
                struct message msg;
                strcpy(msg.hostname, thishostname.c_str());
                msg.type = QUERY_INTERNAL;

                /* Copy the signature as the message */
                if (*signed_length > MESSAGE_FIELD_SIZE)
                {
                    cout << "Attempting to store " << *signed_length << " in " << MESSAGE_FIELD_SIZE << " bytes." << endl;
                }

                /* Copy the public key as the message */
                memcpy(msg.message, request->message, request->length);
                msg.length = request->length;

                /* Copy the signature key as the argument */
                memcpy(msg.argument, signed_data, *signed_length);
                msg.arg_length = *signed_length;
                
                /* Send to all other members */
                send_to_hosts(&msg, port, hosts);

                /* Advance states */
                state = 3;

                break;
            }
            case 3:
            {
                /* Wait for a few seconds for responses */
                responses = get_messages_over_time(4);

                if (responses.size() > F)
                {
                    cout << "Received " << responses.size() << " > F = " << F << " responses." << endl;
                    state = 4;
                }
                else
                {
                    cout << "Received " << responses.size() << " <= F = " << F << " responses." << endl;
                    state = 5;
                }

                break;
            }
            case 4:
            {
                /* Forward the > F response back to to the client */
                struct message msg;
                strcpy(msg.hostname, thishostname.c_str());
                msg.type = QUERY_SUCCEEDED;

                list<struct message>::iterator it;
                for(it = responses.begin(); it != responses.end(); it++)
                {
                    if((*it).type != QUERY_INTERNAL_RESPONSE)
                    {
                        cout << "Got message of type: " << (*it).type << " instead of QUERY_INTERNAL_RESPONSE" << endl;
                        continue;
                    }
                    
                    /* Copy the hostname as the msssage */
                    strcpy((char*)msg.message,(*it).hostname);
                    
                    /* Copy the signature as the object */
                    memcpy(msg.argument, (*it).argument, (*it).arg_length);
                    
                    /* Send the message out now */
                    send_message(&msg, string(request->hostname), port);
                }
                
                /* Exit the loop */
                state = -1;
                
                break;
            }
            case 5:
            {
                /* Send a query failed message back */
                struct message msg;
                strcpy(msg.hostname, thishostname.c_str());
                msg.type = QUERY_FAILED;

                /* Send the message out */
                send_message(&msg, string(request->hostname), port);

                /* Exit the loop */
                state = -1;
                
                break;
            }
        }
    }
}
