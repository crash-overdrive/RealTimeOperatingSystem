#include "user/server/NameServer.hpp"
#include "user/syscall/UserSyscall.hpp"
#include "Constants.hpp"
#include <string.h>
#include "io/bwio.hpp"

#define FOREVER for(;;)

class nameServerEntry {
    public:
        char taskName[Constants::TASK_NAME_SIZE];
        int tid;
};

void nameServer() {
    // initiliaze
    // bwprintf(COM2, "Name Server - Created Name server\n\r");
    nameServerEntry nameServerEntries[Constants::NUM_TASKS];
    int numberOfEntries = -1;
    
    int sendProcessTid;
    
    int nameLength = Constants::TASK_NAME_SIZE;
    int messageSize;

    char* success = "s";
    char* failure = "w";

    

    // start of server
    FOREVER {
        char name[nameLength];
        char requestResponse[2];

        messageSize = Receive(&sendProcessTid, name, nameLength);

        // bwprintf(COM2, "Name Server - Received from %d - %s - %d - %c\n\r", sendProcessTid, name, messageSize, name[0]);
        
        if (messageSize > 0) {
            // process the input
            // do appropriate stuff and reply
            bool matchFound = false;
            if (name[0] == 'r' && messageSize < Constants::TASK_NAME_SIZE) { // REGISTER-AS
                
                for (int i = 0; i <= numberOfEntries; ++i) {

                    int x = strcmp((nameServerEntries[i].taskName), name+1);
                    // bwprintf(COM2, "Value of registering comparison is: %d\n\r", x);
                    
                    if (strcmp((nameServerEntries[i].taskName), name+1) == 0) {
                        
                        nameServerEntries[i].tid = sendProcessTid;
                        // bwprintf(COM2, "Name Server - MATCH found Created name server entry at index: %d %d - %s\n\r", i, sendProcessTid, nameServerEntries[i].taskName);
                        Reply(sendProcessTid, success, 2);
                        matchFound = true;
                        break;

                    }
                }
                if(!matchFound) {
                    // if we reach here then that means that we didnt find a match
                    // time to make a new entry
                    ++numberOfEntries;
                    memcpy(nameServerEntries[numberOfEntries].taskName, name+1, messageSize-1);
                    nameServerEntries[numberOfEntries].tid = sendProcessTid;
                    // bwprintf(COM2, "Name Server - MATCH not found - Created name server entry: %d - %s - %d\n\r", sendProcessTid, nameServerEntries[numberOfEntries].taskName, numberOfEntries);
                    Reply(sendProcessTid, success, 2);
                }                    

            } else if (name[0] == 'w') { // WHO-IS
                bool matchFound = false;
                for (int i = 0; i <= numberOfEntries; ++i) {

                    if (strcmp((nameServerEntries[i].taskName), name+1) == 0) {

                        matchFound = true;
                        // bwprintf(COM2, "Name Server - Found name server entry at index - %d - %s - %d\n\r", i, name+1, nameServerEntries[i].tid);
                        requestResponse[0] = (char) nameServerEntries[i].tid;
                        requestResponse[1] = '\0';
                        Reply(sendProcessTid, requestResponse, 2);
                        break;

                    }
                }
                if(!matchFound) {
                    // if we reached here then that means we didnt find a match
                    // time to return an error
                    Reply(sendProcessTid, failure, 2);
                }
            } else { // INVALID Request to Name-Server

                Reply(sendProcessTid, failure, 2);

            }

        } else { // INVALID Request to Name-Server

            Reply(sendProcessTid, failure, 2);

        }
    }
}