#include "Constants.hpp"
#include "data-structures/RingBuffer.hpp"
#include "io/bwio.hpp"
#include "io/io.hpp"
#include "io/ts7200.h"
#include "user/client/SensorData.hpp"
#include "user/syscall/UserSyscall.hpp"

#define FOREVER for(;;)

class Sensor{
    public:
        char sensorBank;
        int sensorNumber;

        Sensor() {
            sensorBank = (char)0;
            sensorNumber = -1;
        }

        Sensor(char sb, int sn) {
            sensorBank = sb;
            sensorNumber = sn;
        }
};

int power(int base, int exponent) {
	int i;
	int result = 1;

	for(i = 1; i <= exponent; ++i) {
		result = result * base;
	}

	return result;
}

int getSensorBitValue(int bitNumber) {
    return power(2, 8-bitNumber);
}

int getSensorNumber(int sensorBankNumber, int bitNumber) {
    if (sensorBankNumber % 2 != 0) {
        return 8 + bitNumber;
    } else {
        return bitNumber;
    }
}

char getSensorBank(int sensorBankNumber) {
    char sensorBank = '\0';
    if (sensorBankNumber < 2) {
        sensorBank = 'A';
    } else if (sensorBankNumber < 4) {
        sensorBank = 'B';
    } else if (sensorBankNumber < 6) {
        sensorBank = 'C';
    } else if (sensorBankNumber < 8) {
        sensorBank = 'D';
    } else {
        sensorBank = 'E';
    }
    return sensorBank;
}



void sensorData() {
    DataStructures::RingBuffer<Sensor, Constants::SensorData::SENSOR_HISTORY_SIZE> sensorHistory;

    int sensorBankValues[Constants::SensorData::NUMBER_OF_SENSOR_BANKS] = {0};

    int UART1_RX_SERVER = WhoIs("UART1RX");
    int UART2_TX_SERVER = WhoIs("UART2TX");

    bwputc(COM1, Constants::MarklinConsole::SET_RESET_ON_FOR_SENSORS);
    
    FOREVER {
        Delay(Constants::ClockServer::TID, 10);
        bwputc(COM1, Constants::MarklinConsole::REQUEST_5_SENSOR_DATA);
        for (int sensorBankNumber = 0; sensorBankNumber < 10; ++sensorBankNumber) {
            sensorBankValues[sensorBankNumber] = Getc(UART1_RX_SERVER, COM1);
            // bwprintf(COM2, "%d", sensorBankNumber);

            if (sensorBankValues[sensorBankNumber] != 0) {

                char sensorBank = getSensorBank(sensorBankNumber);

                for (int sensorBitNumber = 1; sensorBitNumber <= 8; ++sensorBitNumber) {
                    int sensorBitValue = getSensorBitValue(sensorBitNumber);

                    if(sensorBankValues[sensorBankNumber] >= sensorBitValue) {
                        sensorBankValues[sensorBankNumber] = sensorBankValues[sensorBankNumber] - sensorBitValue;
                        int sensorNumber = getSensorNumber(sensorBankNumber, sensorBitNumber);
                        sensorHistory.push(Sensor(sensorBank, sensorNumber));
                        // bwprintf(COM2, "%c %d sensor Tripped\n\r", sensorBank, sensorNumber);
                    }
                }

                if(sensorBankValues[sensorBankNumber] != 0) {
                    bwprintf(COM2, "Sensor Data - invalid %d bank not 0: %d\n\r", sensorBankNumber, sensorBankValues[sensorBankNumber]);
                }
            }

        }

        while(!sensorHistory.empty()) {
            Sensor sensor = sensorHistory.pop();
            printf(UART2_TX_SERVER, UART2, "%c %d\n\r", sensor.sensorBank, sensor.sensorNumber);
        }
    }
}