#include "networking.h"
#include "message_management.h"
#include "support.h"
#include "ca_code.h"

#include <math.h>
#include <iostream>
#include <openssl/pem.h>
#include <openssl/crypto.h>

using namespace std;

int main(int argc, char** argv)
{
    if (argc != 5)
    {
        cout << "Usage: " << argv[0] << " -port PORT -hostfile hostfile" << endl;
    }

    if (strcmp(argv[1], "-port") != 0)
    {
        std::cout << "Enter the port!" << std::endl;
        return -1;
    }

    port = atoi(argv[2]);
    std::cout << "Using port: " << port << std::endl;

    if (strcmp(argv[3], "-hostfile") != 0)
    {
        std::cout << "Enter the hostfile!" << std::endl;
        return -1;
    }

    /* Get the host file */
    string hostfile = string(argv[4]);

    /* Get the list of other hosts */
    hosts = parsefile(hostfile);
    list<string>::iterator it;

    if (hosts.size() == 0)
    {
        cout << "Host input file not found!" << std::endl;
        return -1;
    }
    else
    {
        cout << "N: " << hosts.size() << endl;
        cout << "N = 2F + 2 => " << floor((hosts.size()-2)/2.0) << endl;
        F = (int)floor((hosts.size()-2)/2.0);
    }

    /* Get this processes host name */
    thishostname = findhostname();
    cout << "I am host: " << thishostname << endl;

    /* Load the private key */
    FILE* fp = fopen((string("keys/")+(thishostname) + string(".pem")).c_str(), "r");
    my_priv_key = PEM_read_RSAPrivateKey(fp, NULL, 0, NULL);
    fclose(fp);

    /* Load the public key */
    fp = fopen((string("pubkeys/")+(thishostname) + string(".pem")).c_str(), "r");
    my_pub_key = PEM_read_RSAPublicKey(fp, NULL, 0, NULL);
    fclose(fp);

    /* Load the database (public keys for all parties) */
    int i = 0;
    for (it = hosts.begin(); it != hosts.end(); it++)
    {
        fp = fopen((string("pubkeys/")+(*it) + string(".pem")).c_str(), "r");

        /* Get the public key for this party */
        database.insert(pair<string, RSA*>(*it,PEM_read_RSAPublicKey(fp, NULL, 0, NULL)));

        fclose(fp);
    }

    /* Wait for commands */
    while (1)
    {
        struct message msg = get_next_message();

        switch (msg.type)
        {
            case QUERY:
            {
                /* For this message, the msg->message field holds K_pub to verify, argument is empty */
                query_internal(&msg);
                break;
            }

            case QUERY_INTERNAL:
            {
                /* Execute the RECEIVE_QUERY state diagram */
                receive_query(&msg);
                break;
            }

            case UPDATE:
            {
                /* Used to update the currently stored public key for a host */
                update_member(&msg);
                
                break;
            }

            case MEMBER_UPDATE:
            {
                break;
            }

            default:
                cout << "Got unexpected message type of: " << msg.type << endl;
        }
    }

    return 0;
}
