#include "networking.h"
#include "message_management.h"
#include "support.h"

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

    if (hosts.size() == 0)
    {
        cout << "Host input file not found!" << std::endl;
        return -1;
    }

    /* Get this processes host name */
    string thishostname = findhostname();

    /* Load the private key */
    FILE* fp = fopen(string("keys/").append(thishostname).append(".pem").c_str(), "r");
    RSA* priv_key = PEM_read_RSAPrivateKey(fp, NULL, 0, NULL);
    fclose(fp);

    /* Load the public key */

    /* Load the database */

    /* Wait for commands */
    while (1)
    {
        struct message msg = get_next_message();

        switch (msg.type)
        {
            case QUERY:

            case UPDATE:

            case MEMBER_UPDATE:

            default:
                cout << "Got unexpected message type of: " << msg.type << endl;
        }
    }

    return 0;
}
