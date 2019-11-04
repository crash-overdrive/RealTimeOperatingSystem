#include "Constants.hpp"
#include "io/bwio.hpp"
#include "io/ts7200.h"
#include "user/message/TRMessage.hpp"
#include "user/message/RVMessage.hpp"
#include "user/message/SWMessage.hpp"
#include "user/server/TrainCommandServer.hpp"
#include "user/syscall/UserSyscall.hpp"

#define FOREVER for(;;)


void trainCommandServer() {
    RegisterAs("TRAIN_SERVER");
    int trainSpeeds[Constants::TrainCommandServer::NUM_TRAINS] = {0};
    char switchOrientations[Constants::TrainCommandServer::NUM_SENSORS];

    const int UART1_TX_SERVER = WhoIs("UART1TX");
    const int CLOCK_SERVER = WhoIs("CLOCK SERVER");

    FOREVER {
        int sendTid;
        char sendMessage[Constants::TrainCommandServer::MAX_SEND_MESSAGE_SIZE] = {0};

        int sendMessageSize = Receive(&sendTid, sendMessage, Constants::TrainCommandServer::MAX_SEND_MESSAGE_SIZE);
        // TODO: Verify if this should be -1 or -2
        sendMessage[sendMessageSize - 1] = '\0';

        char* commandToken = strtok(sendMessage, Constants::TrainCommandServer::DELIMITER);

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

            trainSpeeds[trainNumber] = trainSpeed;

            Reply(sendTid, &Constants::Server::ACK, 1);

            Putc(UART1_TX_SERVER, UART1, trainSpeed);
            Putc(UART1_TX_SERVER, UART1, trainNumber);
            // printf(UART2_TX_SERVER, UART2, "Setting train: %d to speed: %d\n\r", trainNumber, trainSpeed);

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
            Putc(UART1_TX_SERVER, UART1, Constants::MarklinConsole::STOP_TRAIN);
            Putc(UART1_TX_SERVER, UART1, trainNumber);

            Delay(CLOCK_SERVER, 27*trainSpeed);

            Putc(UART1_TX_SERVER, UART1, Constants::MarklinConsole::REVERSE_TRAIN);
            Putc(UART1_TX_SERVER, UART1, trainNumber);

            Delay(CLOCK_SERVER, 5);

            Putc(UART1_TX_SERVER, UART1, trainSpeed);
            Putc(UART1_TX_SERVER, UART1, trainNumber);

            // printf(UART2_TX_SERVER, UART2, "Reversing train: %d to speed: %d\n\r", trainNumber, trainSpeed);

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
                Putc(UART1_TX_SERVER, UART1, Constants::MarklinConsole::STRAIGHT_SWITCH);
                Putc(UART1_TX_SERVER, UART1, switchNumber);
                Putc(UART1_TX_SERVER, UART1, Constants::MarklinConsole::SWITCH_OFF_TURNOUT);
                
                // printf(UART2_TX_SERVER, UART2, "Switching %d to %c\n\r", switchNumber, Constants::TrainCommandServer::STRAIGHT_SWITCH_INPUT);
                
            } else if (switchDirection == Constants::TrainCommandServer::CURVED_SWITCH_INPUT) {
                Reply(sendTid, &Constants::Server::ACK, 1);
                Putc(UART1_TX_SERVER, UART1, Constants::MarklinConsole::CURVED_SWITCH);
                Putc(UART1_TX_SERVER, UART1, switchNumber);
                Putc(UART1_TX_SERVER, UART1, Constants::MarklinConsole::SWITCH_OFF_TURNOUT);
                
                // printf(UART2_TX_SERVER, UART2, "Switching %d to %c\n\r", switchNumber, Constants::TrainCommandServer::CURVED_SWITCH_INPUT);
                
            } else {
                Reply(sendTid, &Constants::Server::ERR, 1);
            }


        } 
        
        else if (!memcmp(commandToken, "q\0", 2) && sendMessageSize == 2) {
            Reply(sendTid, &Constants::Server::ACK, 1);
            SwitchOff();
        }
        
        else {
            // printf(UART2_TX_SERVER, UART2, "Invalid Command\n\r");
            Reply(sendTid, &Constants::Server::ERR, 1);
            
        }

    }
}