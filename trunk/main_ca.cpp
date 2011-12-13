#include "networking.h"
#include "message_management.h"
#include "support.h"
#include "ca_code.h"

#include <iostream>
#include <openssl/pem.h>
#include <openssl/crypto.h>

using namespace std;

int main(int argc, char** argv)
{
    if (argc != 11)
    {
        cout << "Usage: " << argv[0] << " -port PORT -hostfile hostfile -privkey PATH -pubkey PATH -database PATH" << endl;
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
    list<string> hosts = parsefile(hostfile);
    vector<string> host_vector;
    list<string>::iterator it;

    if (hosts.size() == 0)
    {
        cout << "Host input file not found!" << std::endl;
        return -1;
    }

    /* Get this processes host name */
    string thishostname = findhostname();
    cout << "I am host: " << thishostname << endl;

    /* Load the private key */
    FILE* fp = fopen((string("keys/")+(thishostname) + string(".pem")).c_str(), "r");
    priv_key = PEM_read_RSAPrivateKey(fp, NULL, 0, NULL);
    fclose(fp);

    /* Load the public key */
    fp = fopen((string("certs/")+(thishostname) + string(".crt")).c_str(), "r");
    pub_key = PEM_read_RSAPublicKey(fp, NULL, 0, NULL);
    fclose(fp);

    /* Load the database (certicates for all parties) */
    int i = 0;
    for (it = hosts.begin(); it != hosts.end(); it++)
    {
        fp = fopen((string("certs/")+(*it) + string(".crt")).c_str(), "r");

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
            case QUERY_INTERNAL:
            {
                /* For this message, the msg->message field holds K_pub to verify, argument is empty */
                query_internal(&msg);
                break;
            }

            case QUERY_RECEIVED_INTERNAL:
            {
                break;
            }

            case UPDATE_INTERNAL:
            {
                break;
            }

            case MEMBER_UPDATE_INTERNAL:
            {
                break;
            }

            default:
                cout << "Got unexpected message type of: " << msg.type << endl;
        }
    }

    return 0;
}
