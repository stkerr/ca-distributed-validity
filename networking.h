/* 
 * File:   networking.h
 * Author: stkerr
 *
 * Created on December 6, 2011, 10:00 AM
 */

#ifndef NETWORKING_H
#define	NETWORKING_H

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <iostream>

#include <list>
#include <vector>
#include <string>

using namespace std;

extern int port;

extern pthread_mutex_t mutex;
extern std::list<struct message > received_messages;

extern list<string> hosts;

int start_networking(int port);

std::string findhostname();
int send_message(struct message *msg, std::string dest, int port);
void send_to_hosts(struct message *msg, int port, std::list<std::string> hosts);
bool get_next_message(int waittime, struct message * msg);
struct message get_next_message();
std::list<struct message> get_messages_over_time(int waittime);

#endif	/* NETWORKING_H */

