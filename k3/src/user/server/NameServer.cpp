#include <string.h>

#include "Constants.hpp"
#include "io/bwio.hpp"
#include "user/server/NameServer.hpp"
#include "user/syscall/UserSyscall.hpp"

#define FOREVER for(;;)

class nameServerEntry {
    public:
        char taskName[Constants::NameServer::SEND_MESSAGE_MAX_SIZE];
        int tid;
};

void nameServer() {
    // initiliaze
    // bwprintf(COM2, "Name Server - Created Name server\n\r");
    nameServerEntry nameServerEntries[Constants::NUM_TASKS];
    int numberOfEntries = -1;
    
    int sendProcessTid;
    int sendMessageSize;
    char sendMessage[Constants::NameServer::SEND_MESSAGE_MAX_SIZE];
    
    char replyMessage[Constants::NameServer::REPLY_SIZE];
    
    // start of server
    FOREVER {
        
        sendMessageSize = Receive(&sendProcessTid, sendMessage, Constants::NameServer::SEND_MESSAGE_MAX_SIZE);
        
        if (sendMessageSize > 0 && sendMessageSize < Constants::NameServer::SEND_MESSAGE_MAX_SIZE) {
            // process the input
            char requestType = sendMessage[sendMessageSize - 1];
            sendMessage[sendMessageSize - 1] = '\0';
            // bwprintf(COM2, "Name Server - Received from %d - %s - %d\n\r", sendProcessTid, sendMessage, sendMessageSize);

            switch (requestType) {

                case Constants::NameServer::REGISTER_AS: {
                    bool matchFound = false;

                    for (int i = 0; i <= numberOfEntries; ++i) {

                        // int x = strcmp((nameServerEntries[i].taskName), sendMessage);
                        // bwprintf(COM2, "Value of registering comparison is: %d\n\r", x);
                        
                        if (strcmp((nameServerEntries[i].taskName), sendMessage) == 0) {
                            
                            nameServerEntries[i].tid = sendProcessTid;
                            // bwprintf(COM2, "Name Server - MATCH found Replaced name server entry at index: %d %d - %s\n\r", i, sendProcessTid, nameServerEntries[i].taskName);
                            memcpy(replyMessage, &Constants::NameServer::SUCCESS_REPLY, sizeof(Constants::NameServer::SUCCESS_REPLY_SIZE));
                            Reply(sendProcessTid, replyMessage, Constants::NameServer::SUCCESS_REPLY_SIZE);
                            matchFound = true;
                            break;

                        }
                    }
                    if(!matchFound) {
                        // if we reach here then that means that we didnt find a match
                        // time to make a new entry
                        ++numberOfEntries;
                        memcpy(nameServerEntries[numberOfEntries].taskName, sendMessage, sendMessageSize);
                        nameServerEntries[numberOfEntries].tid = sendProcessTid;
                        // bwprintf(COM2, "Name Server - MATCH not found - Created name server entry: %d - %s - %d\n\r", sendProcessTid, nameServerEntries[numberOfEntries].taskName, numberOfEntries);
                        memcpy(replyMessage, &Constants::NameServer::SUCCESS_REPLY, sizeof(Constants::NameServer::SUCCESS_REPLY_SIZE));
                        Reply(sendProcessTid, replyMessage, Constants::NameServer::SUCCESS_REPLY_SIZE);
                    }     
                    break;
                }


                case Constants::NameServer::WHO_IS: {
                    bool matchFound = false;
                    for (int i = 0; i <= numberOfEntries; ++i) {

                        if (strcmp((nameServerEntries[i].taskName), sendMessage) == 0) {
                            matchFound = true;
                            // bwprintf(COM2, "Name Server - Found name server entry at index - %d - %s - %d\n\r", i, nameServerEntries[i].taskName, nameServerEntries[i].tid);
                            memcpy(replyMessage, &i, sizeof(i));
                            Reply(sendProcessTid, replyMessage, sizeof(i));
                            break;
                        }
                    }
                    if(!matchFound) {
                        // if we reached here then that means we didnt find a match
                        // time to return an error
                        memcpy(replyMessage, &Constants::NameServer::FAILURE_REPLY, sizeof(Constants::NameServer::FAILURE_REPLY_SIZE));
                        Reply(sendProcessTid, replyMessage, Constants::NameServer::FAILURE_REPLY_SIZE);
                    }
                    break;
                }


                default: {
                    memcpy(replyMessage, &Constants::NameServer::FAILURE_REPLY, sizeof(Constants::NameServer::FAILURE_REPLY_SIZE));
                    Reply(sendProcessTid, replyMessage, Constants::NameServer::FAILURE_REPLY_SIZE);
                    break;
                }
            }

        } else { // INVALID Request to Name-Server

            memcpy(replyMessage, &Constants::NameServer::FAILURE_REPLY, sizeof(Constants::NameServer::FAILURE_REPLY_SIZE));
            Reply(sendProcessTid, replyMessage, Constants::NameServer::FAILURE_REPLY_SIZE);

        }
    }
}