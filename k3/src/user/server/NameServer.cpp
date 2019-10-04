#include "user/server/NameServer.hpp"
#include "user/syscall/UserSyscall.hpp"
#include "Constants.hpp"

#define FOREVER for(;;)

class nameServerEntry {
    public:
        char taskName[Constants::TASK_NAME_SIZE];
        int tid = -1;
};

void nameServer() {
    // initiliaze
    nameServerEntry nameServerEntries[Constants::NUM_TASKS];
    int numberOfEntries = -1;
    
    int sendProcessTid = -1;
    char name[Constants::TASK_NAME_SIZE];
    int nameLength = Constants::TASK_NAME_SIZE;
    int responseSize;

    // start of server
    FOREVER {

        responseSize = Receive(&sendProcessTid, name, nameLength);

        if (responseSize > 0) {
            // process the input
            // do appropriate stuff and reply
            if (name[0] == 'r') {

            } else if (name[0] == 'w') {
                int i;
            }

        }
    }
}