#define K 10
#define LOCATION "/u/u92/jzage/jzage/ProjectF/ProjF/"
#include "message_management.h"
#include "networking.h"
#include "support.h"

#include <stdio.h>
#include <stdlib.h>
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
    int ret;    
	SHA1(msg, mlen, hash);
    ret = RSA_verify(NID_sha1, hash, 20, sig, siglen, r);
	if(ret != 1)
		return 0;
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

void construct_query(struct message *m, std::string buf)
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
	m->length = 272;
    fread((void*) m->message, 1, 272, myfile);
    fclose(myfile);
}

void construct_update(struct message *m, std::string buf)
{
	string h = "xinu";
    string filename = LOCATION;
    filename += "pubkeys/xinu";
    filename += buf;
    filename += ".cs.purdue.edu.pem";

	h += buf;
	h += ".cs.purdue.edu";
	
	strcpy((char*) m->message, h.c_str());
    strcpy(m->hostname, findhostname().c_str());
	m->length = strlen((char*)m->message);
		
    m->type = UPDATE;
	 
    FILE * myfile;
    myfile = fopen(filename.c_str(), "r");
    if (myfile == NULL)
        return;
	m->length = 272;
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
    int threshold = (hosts.size()-1);
    start_networking(port);
    while (1)
    {
        //menu
        string buf;
        int x;
		int serv;
		char servval[2];
		char b[2];
        map < std::string, bool> verifications;
        cout << "Enter in one of the following:\n"
                << "\t1. Verify certificate\n"
                << "\t2. Membership Update\n"
                << "\t3. Database Update\n";
        getline(cin, buf);
        if (buf.compare("1") == 0)
        {
            cout << "Choose which certificate to verify: Node:\n";
            getline(cin, buf);
            //prepare to send request message to that server
            struct message msg;
            verifications.clear();
            struct message m;
			serv = atoi(buf.c_str());
			if(serv < 10)
			{
				servval[0] = '0';
				b[0] = '0';
				sprintf(servval+1, "%d", serv);
				sprintf(b+1, "%d", serv);
			}
			else
			{
				sprintf(servval, "%d", serv);
				sprintf(b, "%d", serv);
			}
			buf = b;
            construct_query(&m, servval);



			for (x = 0; x < hosts.size(); x++)
            {
                //construct host
                string dest;
                dest = "xinu";
				if((serv+x) % hosts.size() < 10)
				{
					servval[0] = '0';
					sprintf(servval+1, "%d", (int)((serv-1+x) % hosts.size()+1));
				}
				else
					sprintf(servval, "%d", (int)((serv-1+x) % hosts.size()+1));


				dest += servval;
                dest += ".cs.purdue.edu";
                //construct destination
                send_message(&m, dest, port);
                cout << "Sending query about " << "xinu" << buf << " to " << dest << endl;
				int y = 10;
                while (y > 0)
                {
                    if (get_next_message(1, &msg))
                    {
                        string hostn = msg.hostname;
						if(msg.type == QUERY_SUCCEEDED)
							if (verifications.find(hostn) == verifications.end())
								if (verify(msg, m, buf))
									verifications[hostn] = true;
                    }
                    y--;
                    if (verifications.size() > threshold)
                        break;
                }
				
                if (verifications.size() > threshold)
                    break;
            }

            if (verifications.size() > threshold)
            {
                cout << "Root Node xinu" << buf << ".cs.purdue.edu Verified" << endl;
            }
            else
            {
                cout << "Root Node xinu" << buf << ".cs.purdue.edu Failed to Verify" << endl;
            }
            cout << "Statistics: Recieved " << verifications.size() << " verification votes during the " << x << " rounds" << endl;

        }
        if (buf.compare("2") == 0)
        {
            cout << "Choose which CA to update: Node:\n";
            getline(cin, buf);
            //prepare to send request message to that server
            struct message msg;
            verifications.clear();
            struct message m;
			serv = atoi(buf.c_str());
			if(serv < 10)
			{
				servval[0] = '0';
				sprintf(servval+1, "%d", serv);
			}
			else
				sprintf(servval, "%d", serv);
			construct_update(&m, servval);

            //construct host
            string dest;
            dest = "xinu";
            dest += servval;
            dest += ".cs.purdue.edu";

            //construct destination
            send_message(&m, dest, port);
			cout << "Sending Update to " << dest << endl;
			int y = 10;
            while (y > 0)
            {
                if (get_next_message(1, &msg))
                {
                    string hostn = msg.hostname;
					if(msg.type == UPDATE_RESPONSE)
					{
						cout << "Update Response: " << msg.message << endl;
						break;
					}
				}
                y--; 
            }
            
        }
        if (buf.compare("3") == 0)
        {
            cout << "Choose which CA to send update to : Node:\n";
            getline(cin, buf);
            //prepare to send request message to that server
            struct message msg;
            verifications.clear();
            struct message m;
			serv = atoi(buf.c_str());
			if(serv < 10)
			{
				servval[0] = '0';
				sprintf(servval+1, "%d", serv);
			}
			else
				sprintf(servval, "%d", serv);
			construct_update(&m, servval);
			m.type = MEMBER_UPDATE;
            //construct host
            string dest;
            dest = "xinu";
            dest += servval;
            dest += ".cs.purdue.edu";

            //construct destination
            send_message(&m, dest, port);
			cout << "Sending Database Update to " << dest << endl;
			int y = 10;
            while (y > 0)
            {
                if (get_next_message(1, &msg))
                {
                    string hostn = msg.hostname;
					if(msg.type == UPDATE_RESPONSE)
					{
						cout << "Update Response: " << msg.message << endl;
						break;
					}
				}
                y--; 
            }

        }



    }

    return 0;
}
