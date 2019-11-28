#ifndef LOCATION_MESSAGE
#define LOCATION_MESSAGE

#include "Constants.hpp"
#include "user/message/MessageHeader.hpp"
#include "user/model/LocationInfo.hpp"

class LocationMessage {
public:
    MessageHeader mh;
    int count;
    LocationInfo locationInfo[5];

    LocationMessage();

    int size();
    int maxSize();
};

#endif
