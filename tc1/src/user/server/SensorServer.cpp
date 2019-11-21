#include "Constants.hpp"
#include "data-structures/RingBuffer.hpp"
#include "io/bwio.hpp"
#include "io/io.hpp"
#include "io/ts7200.h"
#include "user/courier/SensorGUICourier.hpp"
#include "user/courier/SensorTrainCourier.hpp"
#include "user/message/SensorMessage.hpp"
#include "user/message/TextMessage.hpp"
#include "user/message/ThinMessage.hpp"
#include "user/model/Sensor.hpp"
#include "user/server/SensorServer.hpp"
#include "user/syscall/UserSyscall.hpp"

#define FOREVER for(;;)

char SensorServer::getSensorNumber(int bank, int bit) {
    return (char)(bank % 2 == 0 ? bit + 1 : bit + 9);
}

char SensorServer::getSensorBank(int bank) {
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

void SensorServer::processData() {
    for (int bank = 0; bank < Constants::SensorData::NUM_SENSOR_BANKS; ++bank) {
        // Only process the data bank if it was triggered
        if (textmsg->msg[bank] > 0) {
            for (int bit = 0; bit < 8; ++bit) {
                if ((int)textmsg->msg[bank] & (128>>bit)) {
                    char sensorBank = getSensorBank(bank);
                    char sensorNumber = getSensorNumber(bank, bit);
                    sdmsg.sensors[sdmsg.count].bank = sensorBank;
                    sdmsg.sensors[sdmsg.count].number = sensorNumber;
                    sdmsg.count++;
                }
            }
        }
    }
}

void SensorServer::updateHistory() {
    if (samsg->count > 0) {
        for (int i = 0; i < samsg->count; ++i) {
            // If the queue is full, make room!
            if (sensorHistory.full()) {
                sensorHistory.pop();
            }
            sensorHistory.push(samsg->sensorAttrs[i]);
        }
        sensorHistoryUpdated = true;
        samsg->count = 0;
    }
}

void SensorServer::sendTrain() {
    // Only send to the train for attribution if sensors were triggered
    if (sdmsg.count > 0 && trainCourierReady) {
        Reply(trainCourier, (char*)&sdmsg, sdmsg.size());
        trainCourierReady = true;
        sdmsg.count = 0;
    }
};

void SensorServer::sendGUI() {
    if (sensorHistoryUpdated && guiCourierReady) {
        for (unsigned int i = 0; i < sensorHistory.size(); ++i) {
            sensormsg.sensorData[i] = sensorHistory.pop();
            sensorHistory.push(sensormsg.sensorData[i]);
        }
        Reply(guiCourier, (char*)&sensormsg, sensormsg.size());
        sensorHistoryUpdated = false;
    }
}

void SensorServer::init() {
    trainCourier = Create(5, sensorTrainCourier);
    guiCourier = Create(8, sensorGUICourier);
    // guiCourier = 100;
    trainCourierReady = false;
    guiCourierReady = false;
}

void sensorServer() {
    RegisterAs("SENSOR");

    SensorServer sds;
    sds.init();

    int result, tid;

    ThinMessage request(Constants::MSG::REQUEST);

    FOREVER {
        result = Receive(&tid, (char*)&sds.msg, 128);
        if (result < 0) {
            // TODO: handle empty message
        }

        if (sds.mh->type == Constants::MSG::RDY) {
            // Find out which service is ready and react appropriately
            if (tid == sds.guiCourier) {
                sds.guiCourierReady = true;
                sds.sendGUI();
            }
            // TODO: do stuff after we're ready
        } else if (sds.mh->type == Constants::MSG::TEXT) {
            // Sensor Data has arrived from the Marklin server
            sds.processData();
            sds.sendTrain();
            Reply(tid, (char *)&request, request.size()); // Effectively a new request for sensor data
        } else if (sds.mh->type == Constants::MSG::SENSOR_ATTR) {
            sds.trainCourierReady = true;
            sds.updateHistory();
            sds.sendGUI();
        } else {
            bwprintf(COM2, "Switch Server - Unrecognized message type received %d", sds.mh->type);
        }
    }
}
