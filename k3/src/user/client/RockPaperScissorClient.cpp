#include "user/client/RockPaperScissorClient.hpp"
#include "user/syscall/UserSyscall.hpp"
#include "io/bwio.h"

void rockPaperScissorClient() {
    char* signup = "s";
    char* quit = "q";
    char* rock = "r";
    char response[5];

    int rpsServer = 3;

    int reply = Send(rpsServer, signup, 2, response, 5);

    if (reply == 2 && response[0] == 'r') {
        for (int i = 1; i <= 3; ++i) {
            reply = Send(rpsServer, rock, 2, response, 5);
            if (reply == 2) {
                if (response[0] == 'w') {
                    bwprintf(COM2, "Rock client won\n\r");
                } else if (response[0] == 'l') {
                    bwprintf(COM2, "Rock client lost\n\r");
                } else if (response[0] == 'd') {
                    bwprintf(COM2, "Rock client draw\n\r");
                } else if (response[0] == 'q') {
                    bwprintf(COM2, "Other player quit..\n\r");
                    break;
                } else {
                    bwprintf(COM2, "RPS CLIENT ROCK Received bad value: %s\n\r", response);
                }

            } else {
                bwprintf(COM2, "RPS CLIENT ROCK Received incorrect length: %d - bad value: %s\n\r", reply, response);
            }
        }
        reply = Send(rpsServer, quit, 2, response, 5);
    } else {
        bwprintf(COM2, "RPS CLIENT ROCK Received incorrect length: %d - bad value: %s\n\r", reply, response);
    }
    bwprintf(COM2, "Rock Client exiting!!\n\r");
    Exit();

}