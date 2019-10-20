#include "Constants.hpp"
#include "io/bwio.hpp"
#include "io/ts7200.h"
#include "user/client/ManualTrainControl.hpp"
#include "user/syscall/UserSyscall.hpp"

#define FOREVER for(;;)


void manualTrainControl() {
    
    const int UART1_SERVER = WhoIs("UART1RX");
    const int UART2_RX_SERVER = WhoIs("UART2RX");
    const int UART2_TX_SERVER = WhoIs("UART2TX");

    int trainSpeeds[Constants::ManualTrainControl::NUM_TRAINS] = {0};
    char switchOrientations[Constants::ManualTrainControl::NUM_SENSORS];


    FOREVER {
        int ch;
        char input[Constants::ManualTrainControl::MAX_COMMAND_SIZE] = {0};
        int inputSize = 0;

        // TODO: print "Enter a command: "
        bwprintf(COM2, "Enter a command: ");

        int a, b, c, d;
        bwprintf(COM2, "\r\n\r\nBefore we said hi %d %d %d\r\n", UART2, UART2_TX_SERVER, Constants::UART2TXServer::TID);
        a = Putc(UART2_TX_SERVER, UART2, 'H');
        b = Putc(UART2_TX_SERVER, UART2, 'I');
        c = Putc(UART2_TX_SERVER, UART2, '\r');
        d = Putc(UART2_TX_SERVER, UART2, '\n');
        bwprintf(COM2, "After we said hi %d %d %d %d\r\n\r\n", a, b, c, d);

        do {
            
            ch = Getc(UART2_RX_SERVER, COM2);
            // int x = Putc(UART2_SERVER, COM2, ch);
            bwprintf(COM2, "%c", ch);
            input[inputSize] = ch;
            ++inputSize;

            if (ch == Constants::ManualTrainControl::BACKSPACE && inputSize > 0) {
                --inputSize;
                bwputc(COM2, ch);
                bwprintf(COM2, "%c", ch);
            }
            // TODO: support for backspace?

        } while(ch != Constants::ManualTrainControl::ENTER && inputSize < Constants::ManualTrainControl::MAX_COMMAND_SIZE);
        bwprintf(COM2, "\n\r");
        input[inputSize - 1] = '\0';

        char* commandToken = strtok(input, Constants::ManualTrainControl::DELIMITER);

        if (!memcmp(commandToken, "tr", 2)) {
            int trainNumber = 0;
            int trainSpeed = 0;

            char* trainNumberToken = strtok(nullptr, Constants::ManualTrainControl::DELIMITER);
            if (trainNumberToken == nullptr) {
                continue;
            }

            int numberOfDigitsInTrainNumber = strspn(trainNumberToken, Constants::ManualTrainControl::DIGITS);
            if (numberOfDigitsInTrainNumber == 0 || numberOfDigitsInTrainNumber > 2) {
                continue;
            }

            int temp = 0;
            while (temp < numberOfDigitsInTrainNumber) {
                trainNumber = trainNumber*10 + trainNumberToken[temp] - '0';
                ++temp;
            }


            char* trainSpeedToken = strtok(nullptr, Constants::ManualTrainControl::DELIMITER);
            if (trainSpeedToken == nullptr) {
                continue;
            }
            
            int numberOfDigitsInTrainSpeed = strspn(trainSpeedToken, Constants::ManualTrainControl::DIGITS);
            if (numberOfDigitsInTrainSpeed == 0 || numberOfDigitsInTrainSpeed > 2) {
                continue;
            }

            temp = 0;
            while (temp < numberOfDigitsInTrainSpeed) {
                trainSpeed = trainSpeed*10 + trainSpeedToken[temp] - '0';
                ++temp;
            }

            Putc(UART2_TX_SERVER, UART2, 'H');
            Putc(UART2_TX_SERVER, UART2, 'I');

            bwprintf(COM2, "Setting train: %d to speed: %d\n\r", trainNumber, trainSpeed);

            bwputc(COM1, trainSpeed);
            bwputc(COM1, trainNumber);
            trainSpeeds[trainNumber] = trainSpeed;

        } 
        
        else if (!memcmp(commandToken, "rv", 2)) {
            int trainNumber = 0;

            char* trainNumberToken = strtok(nullptr, Constants::ManualTrainControl::DELIMITER);
            if (trainNumberToken == nullptr) {
                continue;
            }

            int numberOfDigitsInTrainNumber = strspn(trainNumberToken, Constants::ManualTrainControl::DIGITS);
            if (numberOfDigitsInTrainNumber == 0 || numberOfDigitsInTrainNumber > 2) {
                continue;
            }

            int temp = 0;
            while (temp < numberOfDigitsInTrainNumber) {
                trainNumber = trainNumber*10 + trainNumberToken[temp] - '0';
                ++temp;
            }

            int trainSpeed = trainSpeeds[trainNumber];
            bwputc(COM1, Constants::MarklinConsole::STOP_TRAIN);
            bwputc(COM1, trainNumber);
            
            bwputc(COM1, Constants::MarklinConsole::REVERSE_TRAIN);
            bwputc(COM1, trainNumber);

            bwputc(COM1, trainSpeed);
            bwputc(COM1, trainNumber);
            bwprintf(COM2, "Reversing train: %d to speed: %d\n\r", trainNumber, trainSpeed);

        } 
        
        else if (!memcmp(commandToken, "sw", 2)) {
            int switchNumber = 0;
            char switchDirection;

            char* switchNumberToken = strtok(nullptr, Constants::ManualTrainControl::DELIMITER);
            if (switchNumberToken == nullptr) {
                continue;
            }

            int numberOfDigitsInSwitchNumber = strspn(switchNumberToken, Constants::ManualTrainControl::DIGITS);
            if (numberOfDigitsInSwitchNumber == 0 || numberOfDigitsInSwitchNumber > 3) {
                continue;
            }

            int temp = 0;
            while (temp < numberOfDigitsInSwitchNumber) {
                switchNumber = switchNumber*10 + switchNumberToken[temp] - '0';
                ++temp;
            }

            char* switchDirectionToken = strtok(nullptr, Constants::ManualTrainControl::DELIMITER);
            if (switchDirectionToken == nullptr) {
                continue;
            }
            
            switchDirection = switchDirectionToken[0];

            if (switchDirection == Constants::ManualTrainControl::STRAIGHT_SWITCH_INPUT) {
                bwputc(COM1, Constants::MarklinConsole::STRAIGHT_SWITCH);
                bwputc(COM1, switchNumber);
                bwputc(COM1, Constants::MarklinConsole::SWITCH_OFF_TURNOUT);
                bwprintf(COM2, "Switching %d to %c\n\r", switchNumber, Constants::ManualTrainControl::STRAIGHT_SWITCH_INPUT);
            } else if (switchDirection == Constants::ManualTrainControl::CURVED_SWITCH_INPUT) {
                bwputc(COM1, Constants::MarklinConsole::CURVED_SWITCH);
                bwputc(COM1, switchNumber);
                bwputc(COM1, Constants::MarklinConsole::SWITCH_OFF_TURNOUT);
                bwprintf(COM2, "Switching %d to %c\n\r", switchNumber, Constants::ManualTrainControl::CURVED_SWITCH_INPUT);
            } else {
                // TODO: Error out
            }


        } 
        
        else if (!memcmp(commandToken, "q", 1) && inputSize == 2) {
            Exit();
        }
        
        else {
            // TODO: Error
            bwprintf(COM2, "Invalid Command\n\r");
        }

    }
}