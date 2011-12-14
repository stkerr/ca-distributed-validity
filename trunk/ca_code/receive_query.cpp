#include "../ca_code.h"

/*
 * Processes a QUERY_INTERNAL request
 */
void receive_query(struct message *request)
{
    int state = 1;

    RSA* pub_key;

    while (state > 0)
    {
        cout << "I am in receive_query() state " << state << endl;

        switch (state)
        {
            case 1:
            {
                /* Recover the public key we were provided */
                pub_key = convert_to_RSA(request->message, request->length);

                /* Verify the signature */
                unsigned char hash[20], *sig;
                unsigned int siglen;
                SHA1(request->message, request->length, hash);
                int ret = RSA_verify(NID_sha1, hash, 20, request->argument, request->arg_length, pub_key);
                if(ret != 1)
                {
                    cout << "RSA_verify() was not 1!" << endl;
                    unsigned long err = ERR_get_error();
                    cout << "    Error code was: " << err << endl;
                    cout << ERR_error_string(err, NULL);
                    
                    state = 0;
                    break;
                }
                //ret = RSA_verify(NID_sha1, hash, 20, sig, siglen, r);

                /* Advance states */
                state = 2;

                break;
            }
            case 2:
            {
                map<string, RSA*>::iterator temp = database.find(string(request->hostname));

                if (temp == database.end())
                {
                    cout << "Could not locate host " << request->hostname << endl;
                    return;
                }

                RSA* recorded_key = temp->second;

                /* Is the recorded key the same as the one we received? */
                if (recorded_key->e == pub_key->e
                        && recorded_key->n == pub_key->n)
                {
                    state = 3;
                }
                else
                {
                    cout << "Keys do not match: " << endl;
                    cout << "   " << recorded_key->e << endl;
                    cout << "   " << pub_key->e << endl;
                    cout << "   " << recorded_key->n << endl;
                    cout << "   " << pub_key->n << endl;
                    state = 4;
                }

                break;
            }
            case 3:
            {
                /* Sign the public key that we found */
                unsigned char *signed_data; // (K_pub)_C
                unsigned int *signed_length;
                unsigned char hash[20];
                if (!SHA1(request->message, request->length, hash))
                {
                    cout << "SHA-1 failed!" << endl;
                    state = 0;
                    
                    break;
                }
                
                int ret = RSA_sign(NID_sha1, hash, 20, signed_data, signed_length, my_priv_key);
                if(ret != 1)
                {
                    cout << "RSA_sign() was not 1!" << endl;
                    unsigned long err = ERR_get_error();
                    cout << "    Error code was: " << err << endl;
                    cout << ERR_error_string(err, NULL);
                    
                    state = 0;
                    
                    break;
                }
                
                struct message msg;
                msg.type = QUERY_INTERNAL_RESPONSE;
                strcpy(msg.hostname, thishostname.c_str());

                /* Copy the message */
                memcpy(msg.message, request->message, request->length);
                msg.length = request->length;

                /* Copy our signed version of the key */
                memcpy(msg.argument, signed_data, *signed_length);
                msg.arg_length = *signed_length;

                /* Send our response out */
                send_message(&msg, string(request->hostname), port);
                        
                /* Exit the loop */
                state = 0;

                break;
            }
            case 4:
            {
                /* This a no vote, so just timeout here */

                /* Exit the loop */
                state = 0;

                break;
            }
        }
    }
}