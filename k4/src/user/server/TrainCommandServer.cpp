#include "Constants.hpp"
#include "io/bwio.hpp"
#include "io/ts7200.h"
#include "user/courier/TCSGUICourier.hpp"
#include "user/courier/TCSMarklinCourier.hpp"
#include "user/message/RVMessage.hpp"
#include "user/message/SWMessage.hpp"
#include "user/message/TextMessage.hpp"
#include "user/message/ThinMessage.hpp"
#include "user/message/TRMessage.hpp"
#include "user/server/TrainCommandServer.hpp"
#include "user/syscall/UserSyscall.hpp"

#define FOREVER for(;;)


void trainCommandServer() {
    RegisterAs("TCS");
    int trainSpeeds[Constants::TrainCommandServer::NUM_TRAINS] = {0};
    char switchOrientations[Constants::TrainCommandServer::NUM_SENSORS];

    const int CLOCK_SERVER = WhoIs("CLOCK SERVER");

    int result, tid;
    char msg[128];
    TextMessage *cmd = (TextMessage *)&msg;
    MessageHeader *mh = (MessageHeader*)&msg;

    TRMessage trmsg;
    // trmsg.headlights = true;
    SWMessage swmsg;

    ThinMessage rdymsg(Constants::MSG::RDY);

    // int switchCourierTid = Create(0, trainMarklinCourier);
    // int manualTrainControlTid = Create(0, trainMarklinCourier);
    // int automaticTrainControlTid1 = Create(0, trainMarklinCourier);
    int marklinCourier = Create(7, tcsMarklinCourier);
    int guiCourier = Create(7, tcsGUICourier);

    FOREVER {
        result = Receive(&tid, (char*)&msg, Constants::TrainCommandServer::MAX_SEND_MESSAGE_SIZE);
        if(result < 0) {
            // TODO: handle empty message
        }

        if (mh->type == Constants::MSG::RDY) {
            // Reply with the command when we have it
        } else if (mh->type == Constants::MSG::TEXT) {
            char* commandToken = strtok(cmd->msg, Constants::TrainCommandServer::DELIMITER);

            if (!memcmp(commandToken, "tr\0", 3)) {
                int trainNumber = 0;
                int trainSpeed = 0;

                char* trainNumberToken = strtok(nullptr, Constants::TrainCommandServer::DELIMITER);
                if (trainNumberToken == nullptr) {
                    Reply(tid, &Constants::Server::ERR, 1);
                    continue;
                }

                int numberOfDigitsInTrainNumber = strspn(trainNumberToken, Constants::TrainCommandServer::DIGITS);
                if (numberOfDigitsInTrainNumber == 0 || numberOfDigitsInTrainNumber > 2 ||
                    trainNumberToken[numberOfDigitsInTrainNumber] != '\0') {
                    Reply(tid, &Constants::Server::ERR, 1);
                    continue;
                }

                int temp = 0;
                while (temp < numberOfDigitsInTrainNumber) {
                    trainNumber = trainNumber*10 + trainNumberToken[temp] - '0';
                    ++temp;
                }


                char* trainSpeedToken = strtok(nullptr, Constants::TrainCommandServer::DELIMITER);
                if (trainSpeedToken == nullptr) {
                    Reply(tid, &Constants::Server::ERR, 1);
                    continue;
                }

                int numberOfDigitsInTrainSpeed = strspn(trainSpeedToken, Constants::TrainCommandServer::DIGITS);
                if (numberOfDigitsInTrainSpeed == 0 || numberOfDigitsInTrainSpeed > 2 ||
                    trainSpeedToken[numberOfDigitsInTrainSpeed] != '\0'  ||
                    &cmd->msg[cmd->msglen - 1] != &trainSpeedToken[numberOfDigitsInTrainSpeed]) {
                    Reply(tid, &Constants::Server::ERR, 1);
                    continue;
                }

                temp = 0;
                while (temp < numberOfDigitsInTrainSpeed) {
                    trainSpeed = trainSpeed*10 + trainSpeedToken[temp] - '0';
                    ++temp;
                }

                if(trainSpeed > 31) {
                    Reply(tid, &Constants::Server::ERR, 1);
                    continue;
                }
                Reply(tid, (char *)&rdymsg, rdymsg.size());

                trainSpeeds[trainNumber] = trainSpeed;

                trmsg.train = trainNumber;
                trmsg.speed = trainSpeed;
                Reply(marklinCourier, (char*)&trmsg, trmsg.size());
            } else if (!memcmp(commandToken, "rv\0", 3)) {
                int trainNumber = 0;

                char* trainNumberToken = strtok(nullptr, Constants::TrainCommandServer::DELIMITER);
                if (trainNumberToken == nullptr) {
                    Reply(tid, &Constants::Server::ERR, 1);
                    continue;
                }

                int numberOfDigitsInTrainNumber = strspn(trainNumberToken, Constants::TrainCommandServer::DIGITS);
                if (numberOfDigitsInTrainNumber == 0 || numberOfDigitsInTrainNumber > 2 ||
                    trainNumberToken[numberOfDigitsInTrainNumber] != '\0' ||
                    &trainNumberToken[numberOfDigitsInTrainNumber] != &cmd->msg[cmd->msglen - 1]) {
                    Reply(tid, &Constants::Server::ERR, 1);
                    continue;
                }

                int temp = 0;
                while (temp < numberOfDigitsInTrainNumber) {
                    trainNumber = trainNumber*10 + trainNumberToken[temp] - '0';
                    ++temp;
                }
                Reply(tid, (char *)&rdymsg, rdymsg.size());

                int trainSpeed = trainSpeeds[trainNumber];

                trmsg.train = trainNumber;
                trmsg.speed = Constants::MarklinConsole::STOP_TRAIN;
                Reply(marklinCourier, (char*)&trmsg, trmsg.size());

                Delay(CLOCK_SERVER, 27*trainSpeed);
                Receive(&tid, (char*)&msg, Constants::TrainCommandServer::MAX_SEND_MESSAGE_SIZE);

                trmsg.speed = Constants::MarklinConsole::REVERSE_TRAIN;
                Reply(marklinCourier, (char*)&trmsg, trmsg.size());

                Delay(CLOCK_SERVER, 5);
                Receive(&tid, (char*)&msg, Constants::TrainCommandServer::MAX_SEND_MESSAGE_SIZE);

                trmsg.speed = trainSpeed;
                Reply(marklinCourier, (char*)&trmsg, trmsg.size());
            }

            else if (!memcmp(commandToken, "sw\0", 3)) {
                int switchNumber = 0;
                char switchDirection;

                char* switchNumberToken = strtok(nullptr, Constants::TrainCommandServer::DELIMITER);
                if (switchNumberToken == nullptr) {
                    Reply(tid, &Constants::Server::ERR, 1);
                    continue;
                }

                int numberOfDigitsInSwitchNumber = strspn(switchNumberToken, Constants::TrainCommandServer::DIGITS);
                if (numberOfDigitsInSwitchNumber == 0 || numberOfDigitsInSwitchNumber > 3 ||
                    switchNumberToken[numberOfDigitsInSwitchNumber] != '\0') {
                    Reply(tid, &Constants::Server::ERR, 1);
                    continue;
                }

                int temp = 0;
                while (temp < numberOfDigitsInSwitchNumber) {
                    switchNumber = switchNumber*10 + switchNumberToken[temp] - '0';
                    ++temp;
                }

                char* switchDirectionToken = strtok(nullptr, Constants::TrainCommandServer::DELIMITER);
                if (switchDirectionToken == nullptr) {
                    Reply(tid, &Constants::Server::ERR, 1);
                    continue;
                }

                if (&switchDirectionToken[1] != &cmd->msg[cmd->msglen - 1]) {
                    Reply(tid, &Constants::Server::ERR, 1);
                    continue;
                }
                switchDirection = switchDirectionToken[0];

                if (switchDirection == Constants::TrainCommandServer::STRAIGHT_SWITCH_INPUT || switchDirection == Constants::TrainCommandServer::CURVED_SWITCH_INPUT) {
                    Reply(tid, (char *)&rdymsg, rdymsg.size());

                    swmsg.sw = switchNumber;
                    swmsg.state = switchDirection;
                    Reply(marklinCourier, (char*)&swmsg, swmsg.size());

                    Delay(CLOCK_SERVER, 10);
                    Receive(&tid, (char*)&msg, Constants::TrainCommandServer::MAX_SEND_MESSAGE_SIZE);

                    swmsg.sw = 0;
                    Reply(marklinCourier, (char*)&swmsg, swmsg.size());
                    swmsg.sw = switchNumber;
                    Reply(guiCourier, (char*)&swmsg, swmsg.size());
                } else {
                    Reply(tid, &Constants::Server::ERR, 1);
                }
            } else if (!memcmp(commandToken, "q\0", 2) && cmd->msglen == 2) {
                Reply(tid, (char *)&rdymsg, rdymsg.size());
                SwitchOff();
            } else {
                Reply(tid, &Constants::Server::ERR, 1);
            }
        }
    }
}
