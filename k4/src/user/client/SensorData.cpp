#include "Constants.hpp"
#include "data-structures/RingBuffer.hpp"
#include "io/bwio.hpp"
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

int getSensorValue(int sensorNumber) {
    
}

char getSensorBank(int sensorNumber) {
    char sensorBank = '\0';
    if (sensorNumber < 2) {
        sensorBank = 'A';
    } else if (sensorNumber < 4) {
        sensorBank = 'B';
    } else if (sensorNumber < 6) {        
        sensorBank = 'C';
    } else if (sensorNumber < 8) {        
        sensorBank = 'D';
    } else {
        sensorBank = 'E';
    }
    return sensorBank;
}



void sensorData() {
    DataStructures::RingBuffer<Sensor, Constants::SensorData::SENSOR_HISTORY_SIZE> sensorHistory;

    int sensorValues[Constants::SensorData::NUMBER_OF_SENSOR_BANKS] = {0};

    int UART1_SERVER = WhoIs("UART1RX");

    bwputc(COM1, Constants::MarklinConsole::SET_RESET_ON_FOR_SENSORS);
    
    FOREVER {
        bwputc(COM1, Constants::MarklinConsole::REQUEST_5_SENSOR_DATA);
        for (int i = 0; i < 10; ++i) {
            sensorValues[i] = Getc(UART1_SERVER, COM1);

            if (sensorValues[i] != 0) {

                char sensorBank = getSensorBank(i);

                for (int sensorNumber = 7; sensorNumber >= 0; --sensorNumber) {

                    int sensorValue = power(2, sensorNumber);

                    if(sensorValues[i] >= sensorValue) {

                        sensorValues[i] = sensorValues[i] - sensorValue;
                        if (i%2 != 0) {
                            sensorHistory.push(Sensor(sensorBank, sensorNumber + 9));
                            bwprintf(COM2, "%c %d sensor tripped\n\r", sensorBank, sensorNumber+9);
                        } else {
                            sensorHistory.push(Sensor(sensorBank, sensorNumber + 1));
                            bwprintf(COM2, "%c %d sensor tripped\n\r", sensorBank, sensorNumber+9);
                        }

                    } 
                }

                if(sensorValues[i] != 0) {
                    bwprintf(COM2, "Sensor Data - invalid %d bank not 0\n\r", i);
                }
            }

        }
    }
}