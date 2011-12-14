#include "../ca_code.h"

void update_member(struct message* request)
{
    int state = 1;
    while(state > 0)
    {
        switch(state)
        {
            case 1:
            {
                
                RSA* new_entry = convert_to_RSA(request->argument, request->arg_length);
                if(new_entry != NULL)
                        database.insert(pair<string, RSA*>(string((char*)request->message),new_entry));
                
                break;
            }
        }
    }
}
