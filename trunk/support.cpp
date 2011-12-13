#include "support.h"
#include "networking.h"

#include <stdio.h>
#include <fstream>
#include <list>
#include <string>

#include <openssl/crypto.h>
#include <openssl/pem.h>

string thishostname;

int F; // the amount of tolerable failures

map<string, RSA*> database;
RSA *my_priv_key, *my_pub_key;

/**
 * Given a file containing a list of data, one per line
 * parse the file and return the entries in a list
 * 
 * @param filepath The path to the hostfile
 * @return A list of std::string containing the various entries
 */
list<string> parsefile(string filepath)
{
    /* Store the entries in a list*/
    list<string> entries;

    string line;
    ifstream myfile(filepath.c_str());
    if (myfile.is_open())
    {
        while (myfile.good())
        {
            getline(myfile, line);
            if (line.empty()) /* Ignore blank lines */
                continue;
            entries.push_front(line);
        }
        myfile.close();
    }
    entries.reverse();
    return entries;
}

void write_to_file(string message)
{
    ofstream myfile;
    myfile.open(string("files/").append(findhostname()).append(".received").c_str(), ios::out | ios::app | ios::binary);
    if(myfile.is_open())
    {
        myfile << message << endl;
        myfile.close();
    }
    else
        cout << "Unable to open file " << string("files/").append(findhostname()).append(".received") << endl;
}

/*
 * Makes a temporary file to hold the contents of a public key
 * and returns the corresponding RSA 
 */
RSA* convert_to_RSA(unsigned char* data, int length)
{
    FILE* fp = fopen((string("tempfile.")+thishostname).c_str(), "w+");
    fwrite(data, sizeof(unsigned char), length, fp);
    fseek(fp, 0, SEEK_SET);
    
    RSA* new_rsa = PEM_read_RSAPublicKey(fp, NULL, 0, NULL);
    if(new_rsa == NULL)
    {
        cout << "Error reading private key from " << (string("tempfile.")+thishostname) << endl;
    }
    
    fclose(fp);
    
    return new_rsa;
}