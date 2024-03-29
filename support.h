/* 
 * File:   support.h
 * Author: stkerr
 *
 * Created on December 6, 2011, 10:20 AM
 */

#ifndef SUPPORT_H
#define	SUPPORT_H

#include <list>
#include <string>
#include <map>

#include <openssl/crypto.h>
#include <openssl/pem.h>

using namespace std;

list<string> parsefile(string filepath);

void write_to_file(string message);

RSA* convert_to_RSA(unsigned char* data, int length);

extern map<string, RSA*> database;
extern RSA *my_priv_key, *my_pub_key;

extern string thishostname;

extern int F;

#endif	/* SUPPORT_H */

