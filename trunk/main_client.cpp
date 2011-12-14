#define K 10
#define LOCATION "/u/u92/jzage/jzage/ProjectF/ProjF/"
#include "message_management.h"
#include "networking.h"
#include <iostream>
#include <fstream>
#include <map>
#include <openssl/pem.h>
#include <openssl/sha.h>
#include <openssl/rsa.h>
using namespace std;

int spc_verify(unsigned char *msg, unsigned int mlen, unsigned char *sig, unsigned int siglen, RSA *r)
{
    unsigned char hash[20];
    BN_CTX *c;
    int ret;

    if (!(c = BN_CTX_new())) return 0;
    if (!SHA1(msg, mlen, hash) || !RSA_blinding_on(r, c))
    {
        BN_CTX_free(c);
        return 0;
    }
    ret = RSA_verify(NID_sha1, hash, 20, sig, siglen, r);
    RSA_blinding_off(r);
    BN_CTX_free(c);
    return ret;
}

bool verify(struct message msg, struct message sent, std::string buf)
{
    FILE * fp;
    string filename = LOCATION;
    filename += "pubkeys/xinu";
    filename += buf;
    filename += ".cs.purdue.edu.pem";
    cout << "reading public key from" << filename << endl;
    fp = fopen(filename.c_str(), "r");
    RSA *r = PEM_read_RSAPublicKey(fp, NULL, NULL, NULL);
    if (r == NULL)
        return false;
    unsigned char* ptr = (unsigned char*) &sent;
    if (spc_verify(ptr, sizeof (sent), (unsigned char*) msg.argument, msg.arg_length, r) == 0)
        return false;

    return true;
}

void construct_message(struct message *m, std::string buf)
{
    string filename = LOCATION;
    filename += "pubkeys/xinu";
    filename += buf;
    filename += ".cs.purdue.edu.pem";
    cout << "reading public key from" << filename << endl;
    m->type = QUERY;
    strcpy(m->hostname, findhostname().c_str());
    FILE * myfile;
    myfile = fopen(filename.c_str(), "r");
    if (myfile == NULL)
        return;
    fread((void*) m->message, 1, 272, myfile);
    fclose(myfile);
}

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
    
    start_networking(port);
    while (1)
    {
        //menu
        string buf;
        int x;
        map < std::string, bool> verifications;
        cout << "Enter in one of the following:\n"
                << "\t1. Verify certificate\n"
                << "\t2. Membership Update\n"
                << "\t3. Database Update\n";
        getline(cin, buf);
        if (buf.compare("1") == 0)
        {
            cout << "Choose which certificate to verify: Node 1 to Node 20\n";
            getline(cin, buf);
            //prepare to send request message to that server
            struct message msg;
            verifications.clear();
            struct message m;
            construct_message(&m, buf);

            for (x = 0; x < 20; x++)
            {
                //construct host
                string dest;
                dest = "xinu";
                dest += buf;
                dest += ".cs.purdue.edu";

                //construct destination
                send_message(&m, dest, port);
                int y = 10;
                while (y > 0)
                {
                    if (get_next_message(1, &msg))
                    {
                        string hostn = msg.hostname;
                        if (verifications.find(hostn) == verifications.end())
                            if (verify(msg, m, buf))
                                verifications[hostn] = true;
                    }
                    y--;
                    if (verifications.size() > K)
                        break;
                }

                if (verifications.size() > K)
                    break;
            }

            if (verifications.size() > K)
            {
                cout << "Root Node " << msg.message << " Verified" << endl;
            }
            else
            {
                cout << "Root Node " << msg.message << " Failed to Verify" << endl;
            }
            cout << "Statistics: Recieved " << verifications.size() << "verification votes during the" << x << "rounds" << endl;

        }
        if (buf.compare("2") == 0)
        {

        }
        if (buf.compare("3") == 0)
        {

        }



    }

    return 0;
}
