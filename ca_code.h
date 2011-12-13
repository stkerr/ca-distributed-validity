/* 
 * File:   ca_code.h
 * Author: stkerr
 *
 * Created on December 12, 2011, 2:24 PM
 */

#ifndef CA_CODE_H
#define	CA_CODE_H

using namespace std;

#include <openssl/pem.h>
#include <openssl/crypto.h>

#include "message_management.h"
#include "networking.h"

#include "support.h"

void query_internal(struct message *msg);

#endif	/* CA_CODE_H */

