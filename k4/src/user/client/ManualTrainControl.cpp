#include "Constants.hpp"
#include "user/client/ManualTrainControl.hpp"
#include "user/syscall/UserSyscall.hpp"

#define FOREVER for(;;)

// TODO: take these out
extern int Getc(int tid, int uart);
extern int Putc(int tid, int uart, char ch);
int uart1 = 0;
int uart2 = 1;

void manualTrainControl() {
    
    int uart2SendTid = WhoIs("UART2 SEND");
    int uart2GetTid = WhoIs("UART2 GET");
    int uart1SendTid = WhoIs("UART1 SEND");
    int uart1GetTid = WhoIs("UART1 GET");

    int trainSpeeds[Constants::ManualTrainControl::NUM_TRAINS] = {0};
    char switchOrientations[Constants::ManualTrainControl::NUM_SENSORS];

    FOREVER {
        char ch;
        char input[Constants::ManualTrainControl::MAX_COMMAND_SIZE] = {0};
        int inputSize = 0;

        // TODO: print "Enter a command: "

        do {
            
            ch = Getc(uart2GetTid, uart2);
            int x = Putc(uart2SendTid, uart2, ch);
            input[inputSize] = ch;
            ++inputSize;

            // TODO: support for backspace?

        } while(ch != Constants::ManualTrainControl::ENTER);
        input[inputSize - 1] = '\0';

        char* commandToken = strtok(input, Constants::ManualTrainControl::DELIMITER);

        if (!memcmp(commandToken, "tr", 2)) {
            int trainNumber = 0;
            int trainSpeed = 0;

            char* trainNumberToken = strtok(nullptr, Constants::ManualTrainControl::DELIMITER);
            // TODO: check if above is nullptr then error
            int numberOfDigitsInTrainNumber = strspn(trainNumberToken, Constants::ManualTrainControl::DIGITS);
            // TODO: check if above is 0 or > 2 then error
            int temp = 0;
            while (temp < numberOfDigitsInTrainNumber) {
                trainNumber = trainNumber*10 + trainNumberToken[temp] - '0';
                ++temp;
            }


            char* trainSpeedToken = strtok(nullptr, Constants::ManualTrainControl::DELIMITER);
            // TODO: check if above is nullptr then error
            int numberOfDigitsInTrainSpeed = strspn(trainSpeedToken, Constants::ManualTrainControl::DIGITS);
            // TODO: check if above is 0 or > 2 then error
            temp = 0;
            while (temp < numberOfDigitsInTrainSpeed) {
                trainSpeed = trainSpeed*10 + trainSpeedToken[temp] - '0';
                ++temp;
            }

            Putc(uart1SendTid, uart1, trainSpeed);
            Putc(uart1SendTid, uart1, trainNumber);
            trainSpeeds[trainNumber] = trainSpeed;

        } 
        
        else if (!memcmp(commandToken, "rv", 2)) {
            int trainNumber = 0;

            char* trainNumberToken = strtok(nullptr, Constants::ManualTrainControl::DELIMITER);
            // TODO: check if above is nullptr then error
            int numberOfDigitsInTrainNumber = strspn(trainNumberToken, Constants::ManualTrainControl::DIGITS);
            // TODO: check if above is 0 or > 2 then error
            int temp = 0;
            while (temp < numberOfDigitsInTrainNumber) {
                trainNumber = trainNumber*10 + trainNumberToken[temp] - '0';
                ++temp;
            }

            int trainSpeed = trainSpeeds[trainNumber];
            Putc(uart1SendTid, uart1, Constants::MarklinConsole::STOP_TRAIN);
            Putc(uart1SendTid, uart1, trainNumber);
            
            Putc(uart1SendTid, uart1, Constants::MarklinConsole::REVERSE_TRAIN);
            Putc(uart1SendTid, uart1, trainNumber);

            Putc(uart1SendTid, uart1, trainSpeed);
            Putc(uart1SendTid, uart1, trainNumber);

        } 
        
        else if (!memcmp(commandToken, "sw", 2)) {
            int switchNumber = 0;
            char switchDirection;

            char* switchNumberToken = strtok(nullptr, Constants::ManualTrainControl::DELIMITER);
            // TODO: check if above is nullptr then error
            int numberOfDigitsInSwitchNumber = strspn(switchNumberToken, Constants::ManualTrainControl::DIGITS);
            // TODO: check if above is 0 or > 3 then error
            int temp = 0;
            while (temp < numberOfDigitsInSwitchNumber) {
                switchNumber = switchNumber*10 + switchNumberToken[temp] - '0';
                ++temp;
            }

            char* switchDirectionToken = strtok(nullptr, Constants::ManualTrainControl::DELIMITER);
            // TODO: check if above is nullptr then error
            switchDirection = switchDirectionToken[0];

            if (switchDirection == Constants::ManualTrainControl::STRAIGHT_SWITCH_INPUT) {
                Putc(uart1SendTid, uart1, Constants::MarklinConsole::STRAIGHT_SWITCH);
                Putc(uart1SendTid, uart1, switchNumber);
                Putc(uart1SendTid, uart1, Constants::MarklinConsole::SWITCH_OFF_TURNOUT);
            } else if (switchDirection == Constants::ManualTrainControl::CURVED_SWITCH_INPUT) {
                Putc(uart1SendTid, uart1, Constants::MarklinConsole::CURVED_SWITCH);
                Putc(uart1SendTid, uart1, switchNumber);
                Putc(uart1SendTid, uart1, Constants::MarklinConsole::SWITCH_OFF_TURNOUT);
            } else {
                // TODO: Error out
            }


        } 
        
        else if (!memcmp(commandToken, "q", 1) && inputSize == 1) {
            Exit();
        }
        
        else {
            // TODO: Error
        }

    }
}