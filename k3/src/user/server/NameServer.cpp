#include "user/server/NameServer.hpp"
#include "user/syscall/UserSyscall.hpp"

#define FOREVER for(;;)

void nameServer() {
    Exit();
    int sendProcessTid = -1;
    // TODO: evaluate the size, put it in constants file as well
    int namelen = 33; 
    char name[namelen];
    int responseSize;
    FOREVER {
        responseSize = Receive(&sendProcessTid, name, namelen);

        if (responseSize > 0) {

        }
    }
}