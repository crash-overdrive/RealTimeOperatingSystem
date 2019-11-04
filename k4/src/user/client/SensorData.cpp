#include "Constants.hpp"
#include "data-structures/RingBuffer.hpp"
#include "io/bwio.hpp"
#include "io/io.hpp"
#include "io/ts7200.h"
#include "user/client/SensorData.hpp"
#include "user/message/TextMessage.hpp"
#include "user/message/ThinMessage.hpp"
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
    DataStructures::RingBuffer<Sensor, Constants::SensorData::NUMBER_OF_SENSORS> sensorHistory;

    int sensorBankValues[Constants::SensorData::NUMBER_OF_SENSOR_BANKS] = {0};

    // int UART1_TX_SERVER = WhoIs("UART1TX");
    // int UART1_RX_SERVER = WhoIs("UART1RX");
    int UART2_TX_SERVER = WhoIs("UART2TX");
    int CLOCK_SERVER = WhoIs("CLOCK SERVER");
    int TERM = WhoIs("TERM"); // TODO(sgaweda): Remove this when GUI server is created
    int MARKLIN = WhoIs("MARKLIN");

    int result;
    // char data[Constants::SensorData::NUMBER_OF_SENSOR_BANKS];
    ThinMessage reset(Constants::MSG::RESET);
    ThinMessage request(Constants::MSG::REQUEST);
    TextMessage data;
    MessageHeader *mh = (MessageHeader *)&data;

    result = Send(MARKLIN, (char*)&reset, reset.size(), (char*)&data, data.size());
    if (result < 0) {
        bwprintf(COM2, "Reset request send to Marklin server failed\r\n");
    }
    
    FOREVER {
        // TODO(sgwaeda): I think we will want to remove this Delay, so that sensor data reads happen as often as possible
        //   Furthermore, even if we want a delay, this actually isn't a good way to do it because it's 100ms from when we get here, but we need to send to other servers
        //   and the delay from sending to those servers will likely skew this to be more than 100ms!
        Delay(CLOCK_SERVER, 10);
        result = Send(MARKLIN, (char*)&request, request.size(), (char*)&data, data.size());
        if (result < 0) {
            bwprintf(COM2, "Data request send to Marklin server failed\r\n");
        }

        if (mh->type == Constants::MSG::TEXT) {
            for (int sensorBankNumber = 0; sensorBankNumber < Constants::SensorData::NUMBER_OF_SENSOR_BANKS; ++sensorBankNumber) {
                // TODO(sgaweda): Remove the redundancy here, we don't need sensorBankValues because we have data
                //   ALSO: why is sensoBankValues an array of ints instead of bytes??? Is it because you wanted to print it below?
                sensorBankValues[sensorBankNumber] = data.msg[sensorBankNumber];
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
                // TODO(sgaweda): remove this hogwash, GUI should be printing this anyway
                //   SHASH! Maybe come up with a proper string format function?
                printf(UART2_TX_SERVER, UART2, "%c%d", sensor.sensorBank, sensor.sensorNumber);
            }
        } else {
            bwprintf(COM2, "Sensor Data - Expected MSG::TEXT but received a different response type");
        }
    }
}