#include "Constants.hpp"
#include "io/bwio.hpp"
#include "io/ts7200.h"
#include "user/courier/TrainMarklinCourier.hpp"
#include "user/message/RVMessage.hpp"
#include "user/message/SWMessage.hpp"
#include "user/message/TextMessage.hpp"
#include "user/message/ThinMessage.hpp"
#include "user/message/TRMessage.hpp"
#include "user/server/TrainCommandServer.hpp"
#include "user/syscall/UserSyscall.hpp"

#define FOREVER for(;;)


void trainCommandServer() {
    RegisterAs("TRAIN_SERVER");
    int trainSpeeds[Constants::TrainCommandServer::NUM_TRAINS] = {0};
    char switchOrientations[Constants::TrainCommandServer::NUM_SENSORS];

    const int CLOCK_SERVER = WhoIs("CLOCK SERVER");
    TRMessage trmsg;
    trmsg.headlights = true;
    SWMessage swmsg;
    TextMessage sendTextMessage;
    MessageHeader* messageHeader = (MessageHeader*)&sendTextMessage;
    ThinMessage readyMessage(Constants::MSG::RDY);

    int switchCourierTid = Create(0, trainMarklinCourier);
    int manualTrainControlTid = Create(0, trainMarklinCourier);
    // int automaticTrainControlTid1 = Create(0, trainMarklinCourier);

    FOREVER {
        int sendTid;

        int result = Receive(&sendTid, (char*)&sendTextMessage, Constants::TrainCommandServer::MAX_SEND_MESSAGE_SIZE);
        if(result < 0) {
            // TODO: handle empty message
        }

        if (messageHeader->type != Constants::MSG::TEXT) {
            bwprintf(COM2, "TrainCommandServer - Expected MSG::TEXT but received a different response type\n\r");
            // Reply(sendTid, )
        }
        // char* sendMessage = sendTextMessage.msg;
        //TODO: fix this
        // sendMessage[sendMessageSize - 1] = '\0';

        char* commandToken = strtok(sendTextMessage.msg, Constants::TrainCommandServer::DELIMITER);

        if (!memcmp(commandToken, "tr\0", 3)) {
            int trainNumber = 0;
            int trainSpeed = 0;

            char* trainNumberToken = strtok(nullptr, Constants::TrainCommandServer::DELIMITER);
            if (trainNumberToken == nullptr) {
                Reply(sendTid, &Constants::Server::ERR, 1);
                continue;
            }

            int numberOfDigitsInTrainNumber = strspn(trainNumberToken, Constants::TrainCommandServer::DIGITS);
            if (numberOfDigitsInTrainNumber == 0 || numberOfDigitsInTrainNumber > 2 ||
                trainNumberToken[numberOfDigitsInTrainNumber] != '\0') {
                Reply(sendTid, &Constants::Server::ERR, 1);
                continue;
            }

            int temp = 0;
            while (temp < numberOfDigitsInTrainNumber) {
                trainNumber = trainNumber*10 + trainNumberToken[temp] - '0';
                ++temp;
            }


            char* trainSpeedToken = strtok(nullptr, Constants::TrainCommandServer::DELIMITER);
            if (trainSpeedToken == nullptr) {
                Reply(sendTid, &Constants::Server::ERR, 1);
                continue;
            }

            int numberOfDigitsInTrainSpeed = strspn(trainSpeedToken, Constants::TrainCommandServer::DIGITS);
            if (numberOfDigitsInTrainSpeed == 0 || numberOfDigitsInTrainSpeed > 2 ||
                trainSpeedToken[numberOfDigitsInTrainSpeed] != '\0'  ||
                &sendMessage[sendMessageSize - 1] != &trainSpeedToken[numberOfDigitsInTrainSpeed]) {
                Reply(sendTid, &Constants::Server::ERR, 1);
                continue;
            }

            temp = 0;
            while (temp < numberOfDigitsInTrainSpeed) {
                trainSpeed = trainSpeed*10 + trainSpeedToken[temp] - '0';
                ++temp;
            }

            if(trainSpeed > 31) {
                Reply(sendTid, &Constants::Server::ERR, 1);
                continue;
            }
            Reply(sendTid, &Constants::Server::ACK, 1);

            trainSpeeds[trainNumber] = trainSpeed;

            trmsg.train = trainNumber;
            trmsg.speed = trainSpeed;
            Reply(courierTid, (char*)&trmsg, trmsg.size());
        }

        else if (!memcmp(commandToken, "rv\0", 3)) {
            int trainNumber = 0;

            char* trainNumberToken = strtok(nullptr, Constants::TrainCommandServer::DELIMITER);
            if (trainNumberToken == nullptr) {
                Reply(sendTid, &Constants::Server::ERR, 1);
                continue;
            }

            int numberOfDigitsInTrainNumber = strspn(trainNumberToken, Constants::TrainCommandServer::DIGITS);
            if (numberOfDigitsInTrainNumber == 0 || numberOfDigitsInTrainNumber > 2 ||
                trainNumberToken[numberOfDigitsInTrainNumber] != '\0' ||
                &trainNumberToken[numberOfDigitsInTrainNumber] != &sendMessage[sendMessageSize - 1]) {
                Reply(sendTid, &Constants::Server::ERR, 1);
                continue;
            }

            int temp = 0;
            while (temp < numberOfDigitsInTrainNumber) {
                trainNumber = trainNumber*10 + trainNumberToken[temp] - '0';
                ++temp;
            }
            Reply(sendTid, &Constants::Server::ACK, 1);

            int trainSpeed = trainSpeeds[trainNumber];

            trmsg.train = trainNumber;
            trmsg.speed = Constants::MarklinConsole::STOP_TRAIN;
            Reply(courierTid, (char*)&trmsg, trmsg.size());

            Delay(CLOCK_SERVER, 27*trainSpeed);

            trmsg.speed = Constants::MarklinConsole::REVERSE_TRAIN;
            Reply(courierTid, (char*)&trmsg, trmsg.size());

            Delay(CLOCK_SERVER, 5);

            trmsg.speed = trainNumber;
            Reply(courierTid, (char*)&trmsg, trmsg.size());
        }

        else if (!memcmp(commandToken, "sw\0", 3)) {
            int switchNumber = 0;
            char switchDirection;

            char* switchNumberToken = strtok(nullptr, Constants::TrainCommandServer::DELIMITER);
            if (switchNumberToken == nullptr) {
                Reply(sendTid, &Constants::Server::ERR, 1);
                continue;
            }

            int numberOfDigitsInSwitchNumber = strspn(switchNumberToken, Constants::TrainCommandServer::DIGITS);
            if (numberOfDigitsInSwitchNumber == 0 || numberOfDigitsInSwitchNumber > 3 ||
                switchNumberToken[numberOfDigitsInSwitchNumber] != '\0') {
                Reply(sendTid, &Constants::Server::ERR, 1);
                continue;
            }

            int temp = 0;
            while (temp < numberOfDigitsInSwitchNumber) {
                switchNumber = switchNumber*10 + switchNumberToken[temp] - '0';
                ++temp;
            }

            char* switchDirectionToken = strtok(nullptr, Constants::TrainCommandServer::DELIMITER);
            if (switchDirectionToken == nullptr) {
                Reply(sendTid, &Constants::Server::ERR, 1);
                continue;
            }

            if (&switchDirectionToken[1] != &sendMessage[sendMessageSize - 1]) {
                Reply(sendTid, &Constants::Server::ERR, 1);
                continue;
            }
            switchDirection = switchDirectionToken[0];

            if (switchDirection == Constants::TrainCommandServer::STRAIGHT_SWITCH_INPUT) {
                Reply(sendTid, &Constants::Server::ACK, 1);

                swmsg.sw = switchNumber;
                swmsg.state = Constants::MarklinConsole::STRAIGHT_SWITCH;
                Reply(courierTid, (char*)&swmsg, swmsg.size());
            } else if (switchDirection == Constants::TrainCommandServer::CURVED_SWITCH_INPUT) {
                Reply(sendTid, &Constants::Server::ACK, 1);

                swmsg.sw = switchNumber;
                swmsg.state = Constants::MarklinConsole::CURVED_SWITCH;
                Reply(courierTid, (char*)&swmsg, swmsg.size());
            } else {
                Reply(sendTid, &Constants::Server::ERR, 1);
            }
        }

        else if (!memcmp(commandToken, "q\0", 2) && sendMessageSize == 2) {
            Reply(sendTid, &Constants::Server::ACK, 1);
            SwitchOff();
        }

        else {
            Reply(sendTid, &Constants::Server::ERR, 1);
        }
    }
}
