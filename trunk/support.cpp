#include "support.h"
#include "networking.h"

#include <fstream>
#include <list>
#include <string>

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