#include "networking.h"
#include <pthread.h>
#include <strings.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <vector>
#include <algorithm>

#include "message_management.h"

#define BUFFERSIZE 8192

int port;
std::list<struct message > received_messages;
pthread_mutex_t mutex;

list<string> hosts;

void * networking_loop(void *arg)
{
    /* Create the socket itself */
    int server_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    //std::cout << "Started with " << server_socket << std::endl;
    if (server_socket < 0)
    {
        std::cout << "Could not make a socket" << std::endl;
        return (void*)1;
    }

    /* Bind the socket*/
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(*(int*)arg);

    if (bind(server_socket, (struct sockaddr*) &addr, sizeof (addr)) < 0)
    {
        std::cout << "Bind error. " << std::endl;
        close(server_socket);
    }
    
    /* Reserve BUFFERSIZE for received messages */
    struct message buffer;

    while (1)
    {
        /* Zero the buffer */
        bzero(&buffer, sizeof(struct message));

        /* Receive the data */
        recvfrom(server_socket, &buffer, sizeof(struct message), 0, 0, 0);
        
        /* Grab the mutex for the shared list */
        pthread_mutex_lock(&mutex);

        /* Read in the buffer */
        errno = 0;
        received_messages.push_back(buffer);

        if (errno != 0)
        {
            char err_msg[100];
            strerror_r(errno, err_msg, 100);
            std::cout << "Errno: " << errno << "; Msg: " << err_msg << std::endl;
        }
        render_message(&buffer);

        /* Free the mutex for the shared list */
        pthread_mutex_unlock(&mutex);

    }

    /* Should never get here... */
    close(server_socket);

    /* destroys the mutex */
    pthread_mutex_destroy(&mutex);

    return (void*) - 2;
}

/**
 * This function initializes the socket that acts as a
 * server for this process. It starts a new thread and
 * binds to the specified port.
 * 
 * @param port The specified port that the process should bind to.
 * @return 0 if successful, non-zero if there was an error
 */
int start_networking(int the_port)
{
    port = the_port;
    
    /* Create the necessary mutexes */
    pthread_mutex_t mutex;
    pthread_mutex_init(&mutex, NULL);

    /* Spin off into the networking loop */
    pthread_t loop_thread;
    pthread_create(&loop_thread, NULL, networking_loop, (void*) &port);

    return 0;
}

/**
 * Returns the hostname of the computer this process is running on.
 * This could be useful for comparisons against entries in the hostfile.
 * 
 * @return A std::string representing the hostname
 */
std::string findhostname()
{
    char name[255];
    gethostname(name, 255);
    return std::string(name);
}

int send_message(struct message *msg, std::string dest, int port)
{
    /* Dont't message ourselves... */
    if (strcmp(msg->hostname, dest.c_str()) == 0)
    {
        std::cout << "Refusing to message myself. " << std::endl;
        render_message(msg);
        return -15;
    }

    if (strcmp(msg->hostname, findhostname().c_str()) != 0)
    {
        std::cout << "Sending a message, but not addressed from myself...." << std::endl;
        render_message(msg);
    }

    struct hostent *server;
    struct sockaddr_in serv_addr;

    int send_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (send_socket < 0)
    {
        std::cout << "Error opening sending socket!" << std::endl;
        return 1;
    }

    /**
     * 
     * 
     * Fix this stupid gethostbyname_r call
     * NEEDED FOR MULTI_THREADED
     */

    //	std::cout << "server->h_addr: " << inet_ntoa(h_addr) << std::endl;
    while (1)
    {
        server = gethostbyname(dest.c_str());
        if(server == 0)
        {
            cout << "Nulls out, retrying resolution. Errno: " << errno << endl;
            continue;
        }
        struct in_addr h_addr;
        h_addr.s_addr = *((unsigned long *) server->h_addr);

        if (strcmp(inet_ntoa(h_addr), "127.0.0.1") == 0)
        {
//            std::cout << "Resolved to localhost! Trying again! " << std::endl;
        }
        else
            break;
    }

    if (server == NULL)
    {
        std::cout << "Invalid host name: " << dest << std::endl;
        close(send_socket);
        return 2;
    }
    bzero((char *) &serv_addr, sizeof (serv_addr));
    serv_addr.sin_family = AF_INET;

    bcopy((char*) server->h_addr, (char*) &serv_addr.sin_addr.s_addr,
            server->h_length);
    serv_addr.sin_port = htons(port);
    if (connect(send_socket, (struct sockaddr*) &serv_addr, sizeof (serv_addr)) < 0)
    {
        close(send_socket);
        return 3;
    }
    
    /* Send the message */
    int n = write(send_socket, (void*) msg, sizeof (struct message));
    
    close(send_socket);
}

/* Send the given message to every host in the specified list */
void send_to_hosts(struct message *msg, int port, std::list<std::string> hosts)
{
    std::list<std::string>::iterator it;

    for (it = hosts.begin(); it != hosts.end(); it++)
    {
        /* Don't message ourself */
        if (strcmp((*it).c_str(), msg->hostname) == 0)
            continue;

        send_message(msg, *it, port);
    }
}

/* Returns all messages received over a given length of time */
std::list<struct message> get_messages_over_time(int waittime)
{
    if (waittime < 0)
        waittime = 2;

    time_t timer = time(NULL);

    std::list<struct message> returnable;

    while (1)
    {
        time_t curr_time = time(NULL);
        double diff = difftime(curr_time, timer);

        pthread_mutex_lock(&mutex);
        while (received_messages.size() > 0)
        {
            returnable.push_back(received_messages.front());
            received_messages.pop_front();
        }
        pthread_mutex_lock(&mutex);
        
        /* Time out, we're done */
        if (diff > waittime)
        {
            break;
        }
    }
    
    cout << "Received " << returnable.size() << " messages over " << waittime << " seconds" << endl;

    return returnable;
}

/* 
 * Blocks until a message is received and returns that message.
 * Will return immediately upon detecting commander death
 */
struct message get_next_message()
{
    /* Wait for a message */
    while (received_messages.size() == 0)
    {
    }

    struct message msg = received_messages.front();

    received_messages.pop_front();

    return msg;
}

/* 
 * Waits for a message is received and returns that message or at a timeout.
 * Will return immediately upon detecting commander death
 */
bool get_next_message(int waittime, struct message * msg)
{
    if (waittime < 0)
        waittime = 2;

    time_t timer = time(NULL);

    while (1)
    {
        time_t curr_time = time(NULL);
        double diff = difftime(curr_time, timer);

        /* Time out, we're done */
        if (diff > waittime)
        {
            break;
        }

        if (received_messages.size() > 0)
        {
            *msg = received_messages.front();
            received_messages.pop_front();
            return true;
        }
    }

    return false;
}
