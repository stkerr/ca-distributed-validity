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

using namespace std;

list<string> parsefile(string filepath);

void write_to_file(string message);


#endif	/* SUPPORT_H */

