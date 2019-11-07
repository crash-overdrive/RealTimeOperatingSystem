#include "Constants.hpp"
#include "data-structures/RingBuffer.hpp"
#include "io/bwio.hpp"
#include "io/io.hpp"
#include "io/ts7200.h"
#include "user/client/SensorData.hpp"
#include "user/message/SensorMessage.hpp"
#include "user/message/TextMessage.hpp"
#include "user/message/ThinMessage.hpp"
#include "user/model/Sensor.hpp"
#include "user/syscall/UserSyscall.hpp"

#define FOREVER for(;;)

// class Sensor{
//     public:
//         char sensorBank;
//         int sensorNumber;

//         Sensor() {
//             sensorBank = (char)0;
//             sensorNumber = -1;
//         }

//         Sensor(char sb, int sn) {
//             sensorBank = sb;
//             sensorNumber = sn;
//         }
// };

// int power(int base, int exponent) {
// 	int i;
// 	int result = 1;

// 	for(i = 1; i <= exponent; ++i) {
// 		result = result * base;
// 	}

// 	return result;
// }

// int getSensorBitValue(int bitNumber) {
//     return power(2, 8-bitNumber);
// }

// int getSensorNumber(int sensorBankNumber, int bitNumber) {
//     if (sensorBankNumber % 2 != 0) {
//         return 8 + bitNumber;
//     } else {
//         return bitNumber;
//     }
// }

// char getSensorBank(int sensorBankNumber) {
//     char sensorBank = '\0';
//     if (sensorBankNumber < 2) {
//         sensorBank = 'A';
//     } else if (sensorBankNumber < 4) {
//         sensorBank = 'B';
//     } else if (sensorBankNumber < 6) {
//         sensorBank = 'C';
//     } else if (sensorBankNumber < 8) {
//         sensorBank = 'D';
//     } else {
//         sensorBank = 'E';
//     }
//     return sensorBank;
// }

// class Sensor{
//     public:
//         char bank;
//         char number;

//         Sensor() {
//             bank = (char)0;
//             number = (char)0;
//         }

//         Sensor(char b, char n) {
//             bank = b;
//             number = n;
//         }
// };

char getSensorNumber(int bank, int bit) {
    return (char)(bank % 2 == 0 ? bit + 1 : bit + 9);
}

char getSensorBank(int bank) {
    if (bank < 2) {
        return 'A';
    } else if (bank < 4) {
        return 'B';
    } else if (bank < 6) {
        return 'C';
    } else if (bank < 8) {
        return 'D';
    } else {
        return 'E';
    }
}

void sensorData() {
    int CLOCK_SERVER = WhoIs("CLOCK SERVER");
    int MARKLIN = WhoIs("MARKLIN");
    int GUI = WhoIs("GUI");

    int result;
    // char data[Constants::SensorData::NUMBER_OF_SENSOR_BANKS];
    ThinMessage rdymsg(Constants::MSG::RDY);
    ThinMessage request(Constants::MSG::REQUEST);
    ThinMessage reset(Constants::MSG::RESET);
    TextMessage data;
    SensorMessage sensormsg;
    MessageHeader *mh = (MessageHeader *)&data;

    DataStructures::RingBuffer<Sensor, Constants::SensorData::NUM_SENSORS_DISPLAYED> sensorHistory;
    // int sensorBankValues[Constants::SensorData::NUM_SENSOR_BANKS] = {0};

    result = Send(MARKLIN, (char*)&reset, reset.size(), (char*)&data, data.maxSize());
    if (result < 0) {
        bwprintf(COM2, "Reset request send to Marklin server failed\r\n");
    }

    FOREVER {
        // TODO(sgwaeda): I think we will want to remove this Delay, so that sensor data reads happen as often as possible
        //   Furthermore, even if we want a delay, this actually isn't a good way to do it because it's 100ms from when we get here, but we need to send to other servers
        //   and the delay from sending to those servers will likely skew this to be more than 100ms!
        Delay(CLOCK_SERVER, 10);
        result = Send(MARKLIN, (char*)&request, request.size(), (char*)&data, data.maxSize());
        if (result < 0) {
            bwprintf(COM2, "Data request send to Marklin server failed\r\n");
        }

        if (mh->type == Constants::MSG::TEXT) {
            for (int bank = 0; bank < Constants::SensorData::NUM_SENSOR_BANKS; ++bank) {
                // Only process the data bank if it was triggered
                if (data.msg[bank] > 0) {
                    for (int bit = 0; bit < 8; ++bit) {
                        if ((int)data.msg[bank] & (128>>bit)) {
                            char sensorBank = getSensorBank(bank);
                            char sensorNumber = getSensorNumber(bank, bit);
                            // Only add if not the most recent sensor reading
                            if (!sensorHistory.empty() && !(sensorHistory.peekLast()->bank == sensorBank && sensorHistory.peekLast()->number == sensorNumber)) {
                                // If the queue is full, make room!
                                if (sensorHistory.full()) {
                                    sensorHistory.pop();
                                }
                                sensorHistory.push(Sensor(sensorBank, sensorNumber));
                            } else if (sensorHistory.empty()) {
                                sensorHistory.push(Sensor(sensorBank, sensorNumber));
                            }
                        }
                    }
                }
            }

            for (int i = 0; i < sensorHistory.size(); ++i) {
                Sensor sensor = sensorHistory.pop();
                sensormsg.sensorData[i].bank = sensor.bank;
                sensormsg.sensorData[i].number = sensor.number;
                sensorHistory.push(sensor);
            }

            Send(GUI, (char*)&sensormsg, sensormsg.size(), (char*)&rdymsg, rdymsg.size());
            // TODO: check RDY received in reply
        } else {
            bwprintf(COM2, "Sensor Data - Expected MSG::TEXT but received a different response type");
        }
    }
}
