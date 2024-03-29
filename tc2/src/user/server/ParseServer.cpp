#include "Constants.hpp"
#include "io/bwio.hpp"
#include "io/ts7200.h"
#include "user/courier/ParseCSCourier.hpp"
#include "user/courier/ParseNavCourier.hpp"
#include "user/message/DTMessage.hpp"
#include "user/message/RVMessage.hpp"
#include "user/message/RTMessage.hpp"
#include "user/message/SWMessage.hpp"
#include "user/message/TextMessage.hpp"
#include "user/message/ThinMessage.hpp"
#include "user/message/TPMessage.hpp"
#include "user/message/TRMessage.hpp"
#include "user/server/ParseServer.hpp"
#include "user/syscall/UserSyscall.hpp"

#define FOREVER for(;;)

// Returns the number of characters consumed
int parseWhitespace(char *str, int index) {
    int count = 0;
    char c = str[index];
    while (c == ' ' || c == '\t') {
        count++;
        c = str[index + count];
    }
    return count;
}

// Returns the number of characters read
int parseInt(char *str, int index, int* ret) {
    int count = 0;
    int result = 0;
    char c = str[index];
    while (c >= '0' && c <= '9') {
        result = result * 10 + c - '0';
        count++;
        c = str[index + count];
    }
    *ret = result;
    return count;
}

int parseAlphaNum(char *src, char *dest, int index) {
    int count = 0;
    char c = src[index];
    while ((c >= '0' && c <= '9') || (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z')) {
        dest[count] = c;
        count++;
        c = src[index + count];
    }
    dest[count] = 0;
    return count;
}

void parseServer() {
    RegisterAs("PARSE");

    int result, tid;
    char msg[128];
    MessageHeader *mh = (MessageHeader*)&msg;
    TextMessage *cmd = (TextMessage *)&msg;

    TRMessage trmsg;
    RVMessage rvmsg;
    SWMessage swmsg;
    RTMessage rtmsg;
    DTMessage dtmsg;
    TPMessage tpmsg;

    ThinMessage rdymsg(Constants::MSG::RDY);
    ThinMessage errmsg(Constants::MSG::ERR);

    int navCourier = Create(5, parseNavCourier);
    int commandCourier = Create(7, parseCSCourier);

    FOREVER {
        result = Receive(&tid, (char*)&msg, Constants::TrainCommandServer::MAX_SEND_MESSAGE_SIZE);
        if(result < 0) {
            // TODO: handle empty message
        }

        if (mh->type == Constants::MSG::RDY) {
            // RDY is received from the command server
            // RDY is also received from the nav server
        } else if (mh->type == Constants::MSG::TEXT) {
            // char* commandToken = strtok(cmd->msg, Constants::TrainCommandServer::DELIMITER);
            int index = 0;
            index += parseWhitespace(cmd->msg, index);

            // if (!memcmp(&cmd->msg[index], "tr", 2)) {
            if (cmd->msg[index] == 't' && cmd->msg[index + 1] == 'r') {
                int train = 0;
                int speed = 0;

                index += 2;

                // Consume whitespace
                result = parseWhitespace(cmd->msg, index);
                if (result < 1) {
                    Reply(tid, (char*)&errmsg, errmsg.size());
                    continue;
                }
                index += result;

                // Get the train number
                result = parseInt(cmd->msg, index, &train);
                if (result <= 0) {
                    Reply(tid, (char*)&errmsg, errmsg.size());
                    continue;
                }
                index += result;
                // Check that train is valid
                if (train < 1 || train > 80) {
                    Reply(tid, (char*)&errmsg, errmsg.size());
                    continue;
                }

                // Consume whitespace
                result = parseWhitespace(cmd->msg, index);
                if (result < 1) {
                    Reply(tid, (char*)&errmsg, errmsg.size());
                    continue;
                }
                index += result;

                // Get the speed
                result = parseInt(cmd->msg, index, &speed);
                if (result <= 0) {
                    Reply(tid, (char*)&errmsg, errmsg.size());
                    continue;
                }
                index += result;
                // Check that speed is valid
                if (speed > 14) {
                    Reply(tid, (char*)&errmsg, errmsg.size());
                    continue;
                }

                // Consume whitespace
                index += parseWhitespace(cmd->msg, index);
                // Check that we've reached the end of input
                if (cmd->msg[index] != '\0') {
                    Reply(tid, (char*)&errmsg, errmsg.size());
                    continue;
                }

                trmsg.train = (char)train;
                trmsg.speed = (char)speed;

                Reply(tid, (char*)&rdymsg, rdymsg.size());
                Reply(commandCourier, (char*)&trmsg, trmsg.size());
            } else if (cmd->msg[index] == 'r' && cmd->msg[index + 1] == 'v') {
                int train = 0;

                index += 2;

                // Consume whitespace
                result = parseWhitespace(cmd->msg, index);
                if (result < 1) {
                    Reply(tid, (char*)&errmsg, errmsg.size());
                    continue;
                }
                index += result;

                // Get the train number
                result = parseInt(cmd->msg, index, &train);
                if (result <= 0) {
                    Reply(tid, (char*)&errmsg, errmsg.size());
                    continue;
                }
                index += result;
                // Check that train is valid
                if (train < 1 || train > 80) {
                    Reply(tid, (char*)&errmsg, errmsg.size());
                    continue;
                }

                // Consume whitespace
                index += parseWhitespace(cmd->msg, index);
                // Check that we've reached the end of input
                if (cmd->msg[index] != '\0') {
                    Reply(tid, (char*)&errmsg, errmsg.size());
                    continue;
                }

                rvmsg.train = (char)train;

                Reply(tid, (char*)&rdymsg, rdymsg.size());
                Reply(commandCourier, (char*)&rvmsg, rvmsg.size());
            } else if (cmd->msg[index] == 's' && cmd->msg[index + 1] == 'w') {
                int sw = 0;
                char state;

                index += 2;

                // Consume whitespace
                result = parseWhitespace(cmd->msg, index);
                if (result < 1) {
                    Reply(tid, (char*)&errmsg, errmsg.size());
                    continue;
                }
                index += result;

                // Get the switch number
                result = parseInt(cmd->msg, index, &sw);
                if (result <= 0) {
                    Reply(tid, (char*)&errmsg, errmsg.size());
                    continue;
                }
                index += result;
                // Check that switch is valid
                if (sw < 1 || (sw > 18 && sw < 153) || sw > 156) {
                    Reply(tid, (char*)&errmsg, errmsg.size());
                    continue;
                }

                // Consume whitespace
                result = parseWhitespace(cmd->msg, index);
                if (result < 1) {
                    Reply(tid, (char*)&errmsg, errmsg.size());
                    continue;
                }
                index += result;

                // Get the switch state
                state = cmd->msg[index++];
                // Check that the switch state is valid
                if (!(state == 'c' || state == 'C' || state == 's' || state == 'S')) {
                    Reply(tid, (char*)&errmsg, errmsg.size());
                    continue;
                }

                // Consume whitespace
                index += parseWhitespace(cmd->msg, index);
                // Check that we've reached the end of input
                if (cmd->msg[index] != '\0') {
                    Reply(tid, (char*)&errmsg, errmsg.size());
                    continue;
                }

                swmsg.sw = (char)sw;
                swmsg.state = state;

                Reply(commandCourier, (char*)&swmsg, swmsg.size());
                Reply(tid, (char*)&rdymsg, rdymsg.size());
            } else if (cmd->msg[index] == 'r' && cmd->msg[index + 1] == 't') {
                // rt tr src dest
                int train = 0;

                index += 2;

                // Consume whitespace
                result = parseWhitespace(cmd->msg, index);
                if (result < 1) {
                    Reply(tid, (char*)&errmsg, errmsg.size());
                    continue;
                }
                index += result;

                // Get the train number
                result = parseInt(cmd->msg, index, &train);
                if (result <= 0) {
                    Reply(tid, (char*)&errmsg, errmsg.size());
                    continue;
                }
                index += result;
                // Check that train is valid
                if (train < 1 || train > 80) {
                    Reply(tid, (char*)&errmsg, errmsg.size());
                    continue;
                }

                rtmsg.train = train;

                // Consume whitespace
                result = parseWhitespace(cmd->msg, index);
                if (result < 1) {
                    Reply(tid, (char*)&errmsg, errmsg.size());
                    continue;
                }
                index += result;

                // Get the src
                // TODO: build a trie to check input
                result = parseAlphaNum(cmd->msg, rtmsg.src, index);
                if (result < 1) {
                    Reply(tid, (char*)&errmsg, errmsg.size());
                    continue;
                }
                index += result;

                // Consume whitespace
                result = parseWhitespace(cmd->msg, index);
                if (result < 1) {
                    Reply(tid, (char*)&errmsg, errmsg.size());
                    continue;
                }
                index += result;

                // Get the dest
                result = parseAlphaNum(cmd->msg, rtmsg.dest, index);
                if (result < 1) {
                    Reply(tid, (char*)&errmsg, errmsg.size());
                    continue;
                }
                index += result;

                // Consume whitespace
                index += parseWhitespace(cmd->msg, index);
                // Check that we've reached the end of input
                if (cmd->msg[index] != '\0') {
                    Reply(tid, (char*)&errmsg, errmsg.size());
                    continue;
                }

                Reply(navCourier, (char*)&rtmsg, rtmsg.size());
                Reply(tid, (char*)&rdymsg, rdymsg.size());
            } else if (cmd->msg[index] == 'd' && cmd->msg[index + 1] == 't') {
                // dt tr dest
                int train = 0;

                index += 2;

                // Consume whitespace
                result = parseWhitespace(cmd->msg, index);
                if (result < 1) {
                    Reply(tid, (char*)&errmsg, errmsg.size());
                    continue;
                }
                index += result;

                // Get the train number
                result = parseInt(cmd->msg, index, &train);
                if (result <= 0) {
                    Reply(tid, (char*)&errmsg, errmsg.size());
                    continue;
                }
                index += result;
                // Check that train is valid
                if (train < 1 || train > 80) {
                    Reply(tid, (char*)&errmsg, errmsg.size());
                    continue;
                }

                dtmsg.train = train;

                // Consume whitespace
                result = parseWhitespace(cmd->msg, index);
                if (result < 1) {
                    Reply(tid, (char*)&errmsg, errmsg.size());
                    continue;
                }
                index += result;

                // Get the dest
                // TODO: build a trie to check input
                result = parseAlphaNum(cmd->msg, dtmsg.dest, index);
                if (result < 1) {
                    Reply(tid, (char*)&errmsg, errmsg.size());
                    continue;
                }
                index += result;

                // Consume whitespace
                index += parseWhitespace(cmd->msg, index);
                // Check that we've reached the end of input
                if (cmd->msg[index] != '\0') {
                    Reply(tid, (char*)&errmsg, errmsg.size());
                    continue;
                }

                Reply(navCourier, (char*)&dtmsg, dtmsg.size());
                Reply(tid, (char*)&rdymsg, rdymsg.size());
            } else if (cmd->msg[index] == 't' && cmd->msg[index + 1] == 'p') {
                // tp tr src
                int train = 0;

                index += 2;

                // Consume whitespace
                result = parseWhitespace(cmd->msg, index);
                if (result < 1) {
                    Reply(tid, (char*)&errmsg, errmsg.size());
                    continue;
                }
                index += result;

                // Get the train number
                result = parseInt(cmd->msg, index, &train);
                if (result <= 0) {
                    Reply(tid, (char*)&errmsg, errmsg.size());
                    continue;
                }
                index += result;
                // Check that train is valid
                if (train < 1 || train > 80) {
                    Reply(tid, (char*)&errmsg, errmsg.size());
                    continue;
                }

                tpmsg.train = train;

                // Consume whitespace
                result = parseWhitespace(cmd->msg, index);
                if (result < 1) {
                    Reply(tid, (char*)&errmsg, errmsg.size());
                    continue;
                }
                index += result;

                // Get the dest
                // TODO: build a trie to check input
                result = parseAlphaNum(cmd->msg, tpmsg.src, index);
                if (result < 1) {
                    Reply(tid, (char*)&errmsg, errmsg.size());
                    continue;
                }
                index += result;

                // Consume whitespace
                index += parseWhitespace(cmd->msg, index);
                // Check that we've reached the end of input
                if (cmd->msg[index] != '\0') {
                    Reply(tid, (char*)&errmsg, errmsg.size());
                    continue;
                }

                Reply(navCourier, (char*)&tpmsg, tpmsg.size());
                Reply(tid, (char*)&rdymsg, rdymsg.size());
            } else if (cmd->msg[index] == 'q') {
                index += 1;

                // Consume whitespace
                index += parseWhitespace(cmd->msg, index);
                // Check that we've reached the end of input
                if (cmd->msg[index] != '\0') {
                    Reply(tid, (char*)&errmsg, errmsg.size());
                    continue;
                }

                Reply(tid, (char *)&rdymsg, rdymsg.size());
                // TODO: make this send to commandCourier
                Shutdown();
            } else {
                Reply(tid, (char*)&errmsg, errmsg.size());
            }
        }
    }
}

