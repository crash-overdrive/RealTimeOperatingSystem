#include "Constants.hpp"
#include "io/bwio.hpp"
#include "io/ts7200.h"
#include "user/client/TrainController.hpp"
#include "user/syscall/UserSyscall.hpp"

#define FOREVER for(;;)


void trainController() {
    
    int trainSpeeds[Constants::TrainController::NUM_TRAINS] = {0};
    char switchOrientations[Constants::TrainController::NUM_SENSORS];


    FOREVER {
        int sendTid;
        char sendMessage[Constants::TrainController::MAX_SEND_MESSAGE_SIZE] = {0};

        int sendMessageSize = Receive(&sendTid, sendMessage, Constants::TrainController::MAX_SEND_MESSAGE_SIZE);
        // TODO: Verify if this should be -1 or -2
        sendMessage[sendMessageSize - 1] = '\0';

        char* commandToken = strtok(sendMessage, Constants::TrainController::DELIMITER);

        if (!memcmp(commandToken, "tr ", 3)) {
            int trainNumber = 0;
            int trainSpeed = 0;

            char* trainNumberToken = strtok(nullptr, Constants::TrainController::DELIMITER);
            if (trainNumberToken == nullptr) {
                Reply(sendTid, &Constants::Server::ERR, 1);
                continue;
            }

            int numberOfDigitsInTrainNumber = strspn(trainNumberToken, Constants::TrainController::DIGITS);
            if (numberOfDigitsInTrainNumber == 0 || numberOfDigitsInTrainNumber > 2) {
                Reply(sendTid, &Constants::Server::ERR, 1);
                continue;
            }

            int temp = 0;
            while (temp < numberOfDigitsInTrainNumber) {
                trainNumber = trainNumber*10 + trainNumberToken[temp] - '0';
                ++temp;
            }


            char* trainSpeedToken = strtok(nullptr, Constants::TrainController::DELIMITER);
            if (trainSpeedToken == nullptr) {
                Reply(sendTid, &Constants::Server::ERR, 1);
                continue;
            }
            
            int numberOfDigitsInTrainSpeed = strspn(trainSpeedToken, Constants::TrainController::DIGITS);
            if (numberOfDigitsInTrainSpeed == 0 || numberOfDigitsInTrainSpeed > 2) {
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

            // Putc(UART2_TX_SERVER, UART2, 'H');
            // Putc(UART2_TX_SERVER, UART2, 'I');

            // printf(UART2_TX_SERVER, UART2, "Setting train: %d to speed: %d\n\r", trainNumber, trainSpeed);

            // bwputc(COM1, trainSpeed);
            // bwputc(COM1, trainNumber);
            trainSpeeds[trainNumber] = trainSpeed;

            Reply(sendTid, &Constants::Server::ACK, 1);

        } 
        
        else if (!memcmp(commandToken, "rv ", 3)) {
            int trainNumber = 0;

            char* trainNumberToken = strtok(nullptr, Constants::TrainController::DELIMITER);
            if (trainNumberToken == nullptr) {
                Reply(sendTid, &Constants::Server::ERR, 1);
                continue;
            }

            int numberOfDigitsInTrainNumber = strspn(trainNumberToken, Constants::TrainController::DIGITS);
            if (numberOfDigitsInTrainNumber == 0 || numberOfDigitsInTrainNumber > 2) {
                Reply(sendTid, &Constants::Server::ERR, 1);
                continue;
            }

            int temp = 0;
            while (temp < numberOfDigitsInTrainNumber) {
                trainNumber = trainNumber*10 + trainNumberToken[temp] - '0';
                ++temp;
            }

            int trainSpeed = trainSpeeds[trainNumber];

            Reply(sendTid, &Constants::Server::ACK, 1);
            // bwputc(COM1, Constants::MarklinConsole::STOP_TRAIN);
            // bwputc(COM1, trainNumber);
            
            // bwputc(COM1, Constants::MarklinConsole::REVERSE_TRAIN);
            // bwputc(COM1, trainNumber);

            // bwputc(COM1, trainSpeed);
            // bwputc(COM1, trainNumber);
            // printf(UART2_TX_SERVER, UART2, "Reversing train: %d to speed: %d\n\r", trainNumber, trainSpeed);

        } 
        
        else if (!memcmp(commandToken, "sw ", 3)) {
            int switchNumber = 0;
            char switchDirection;

            char* switchNumberToken = strtok(nullptr, Constants::TrainController::DELIMITER);
            if (switchNumberToken == nullptr) {
                Reply(sendTid, &Constants::Server::ERR, 1);
                continue;
            }

            int numberOfDigitsInSwitchNumber = strspn(switchNumberToken, Constants::TrainController::DIGITS);
            if (numberOfDigitsInSwitchNumber == 0 || numberOfDigitsInSwitchNumber > 3) {
                Reply(sendTid, &Constants::Server::ERR, 1);
                continue;
            }

            int temp = 0;
            while (temp < numberOfDigitsInSwitchNumber) {
                switchNumber = switchNumber*10 + switchNumberToken[temp] - '0';
                ++temp;
            }

            char* switchDirectionToken = strtok(nullptr, Constants::TrainController::DELIMITER);
            if (switchDirectionToken == nullptr) {
                Reply(sendTid, &Constants::Server::ERR, 1);
                continue;
            }
            
            switchDirection = switchDirectionToken[0];

            if (switchDirection == Constants::TrainController::STRAIGHT_SWITCH_INPUT) {
                // bwputc(COM1, Constants::MarklinConsole::STRAIGHT_SWITCH);
                // bwputc(COM1, switchNumber);
                // bwputc(COM1, Constants::MarklinConsole::SWITCH_OFF_TURNOUT);
                // printf(UART2_TX_SERVER, UART2, "Switching %d to %c\n\r", switchNumber, Constants::TrainController::STRAIGHT_SWITCH_INPUT);
                Reply(sendTid, &Constants::Server::ACK, 1);
            } else if (switchDirection == Constants::TrainController::CURVED_SWITCH_INPUT) {
                // bwputc(COM1, Constants::MarklinConsole::CURVED_SWITCH);
                // bwputc(COM1, switchNumber);
                // bwputc(COM1, Constants::MarklinConsole::SWITCH_OFF_TURNOUT);
                // printf(UART2_TX_SERVER, UART2, "Switching %d to %c\n\r", switchNumber, Constants::TrainController::CURVED_SWITCH_INPUT);
                Reply(sendTid, &Constants::Server::ACK, 1);
            } else {
                Reply(sendTid, &Constants::Server::ERR, 1);
            }


        } 
        
        else if (!memcmp(commandToken, "q", 1) && sendMessageSize == 2) {
            Reply(sendTid, &Constants::Server::ACK, 1);
            Exit();
        }
        
        else {
            // printf(UART2_TX_SERVER, UART2, "Invalid Command\n\r");
            Reply(sendTid, &Constants::Server::ERR, 1);
            
        }

    }
}