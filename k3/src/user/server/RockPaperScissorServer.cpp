#include "data-structures/RingBuffer.hpp"
#include "user/syscall/UserSyscall.hpp"
#include "io/bwio.hpp"
#include "Util.hpp"
#include "string.h"

#define FOREVER for(;;)

void rockPaperScissorServer() {
    // TODO: evaluate the size, put it in constants file as well
    DataStructures::RingBuffer<int, 10> registrationList;

    int sendProcessTid = -1;
    // TODO: evaluate the size, put it in constants file as well
    int msglen = 5; 
    char msg[msglen];
    int responseSize;

    int tidPlayer1 = -1;
    int tidPlayer2 = -1;
    char responsePlayer1 = '\0';
    char responsePlayer2 = '\0';
    bool hasPlayer1Responded = false;
    bool hasPlayer2Responded = false;
    char* ready = "r";
    char* quit = "q";
    char* win = "w";
    char* loss = "l";
    char* draw = "d";
    bool gameOver = true;
    char name[] = "rRPS SERVER";

    RegisterAs(name);

    FOREVER {
        
        responseSize = Receive(&sendProcessTid, msg, msglen);

        // process the value of what we received
        if (responseSize != 2) {
            bwprintf(COM2, "Response Size while receiving in RPS is not 2: %d %d %s\n\r", responseSize, sendProcessTid, msg);
            Util::assert(false);
        }
        else if (msg[0] == 's') { //SIGN UP       
            registrationList.push((int *)sendProcessTid);
            bwprintf(COM2, "Got sign up request from %d %d\n\r", sendProcessTid, registrationList.size());

            if (registrationList.size() >= 2 && gameOver) {
                tidPlayer1 = (int)registrationList.pop();
                tidPlayer2 = (int)registrationList.pop();
            
                gameOver = false;

                bwprintf(COM2, "Sending ready signals to %d %d\n\r", tidPlayer1, tidPlayer2);

                Reply(tidPlayer1, ready, 2);
                Reply(tidPlayer2, ready, 2);
            }

        } else if (msg[0] == 'q') { //QUIT

            // Let the client go
            Reply(sendProcessTid, quit, 2);

            // both clients send q on same iteration
            if (gameOver && registrationList.size() >= 2) {
                tidPlayer1 = (int)registrationList.pop();
                tidPlayer2 = (int)registrationList.pop();

                gameOver = false;

                Reply(tidPlayer1, ready, 2);
                Reply(tidPlayer2, ready, 2);
                
                continue;
            }

            // Set gameover state to be true so that on next iteration other task can be let go
            gameOver = true;

        } else if (msg[0] == 'r' || msg [0] == 'p' || msg[0] == 'x') { // PLAY

            // If one client has already called quits to the game
            if (gameOver) {
                // send quit signal to the other process that the game has ended
                Reply(sendProcessTid, quit, 2);

                // handle popping off next elements which are ready and then get out of this loop
                if (registrationList.size() >= 2) {
                    // release the next 2 if available
                    tidPlayer1 = *registrationList.pop();
                    tidPlayer2 = *registrationList.pop();

                    gameOver = false;

                    Reply(tidPlayer1, ready, 2);
                    Reply(tidPlayer2, ready, 2);   
                    continue;
                }
            }

            if (sendProcessTid == tidPlayer1) {
                responsePlayer1 = msg[0];
                hasPlayer1Responded = true;
            } else if (sendProcessTid == tidPlayer2) {
                responsePlayer2 = msg[0];
                hasPlayer2Responded = true;
            } else {
                bwprintf(COM2, "A task which shouldnt have sent message, sent a message!");
            }

            if (hasPlayer1Responded && hasPlayer2Responded) {  
                bwprintf(COM2, "Press any key to continue...\n\r");
                bwgetc(COM2);
                if (responsePlayer1 == responsePlayer2) { 
                    Reply(tidPlayer1, draw, 2);
                    Reply(tidPlayer2, draw, 2);

                } else if ((responsePlayer1 == 'p' && responsePlayer2 == 'r') ||
                (responsePlayer1 == 'x' && responsePlayer2 == 'p') ||
                (responsePlayer1 == 'r' && responsePlayer2 == 'x')) {  
                    Reply(tidPlayer1, win, 2);
                    Reply(tidPlayer2, loss, 2);

                } else {
                    Reply(tidPlayer1, loss, 2);
                    Reply(tidPlayer2, win, 2);

                }
                responsePlayer1 = responsePlayer2 = '\0';
                hasPlayer1Responded = false;
                hasPlayer2Responded = false;
            }
        }
    }
}