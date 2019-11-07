#ifndef GUI_SERVER_HPP
#define GUI_SERVER_HPP

#include "user/message/TextMessage.hpp"

void guiServer();

class GUI {
public:
    // Constants::GUIServer::STYLE style;
    // const char *base;
    // int baseWidth;
    // const char *base = SET TO GUI STYLE MANUALLY
    // int baseWidth = SET TO GUI STYLE MANUALLY
    // bool drawingBase;
    int termCourier;
    TextMessage drawmsg;

    int insertSetDisplayAttrs(char *str, int index, int attr);
    int insertSaveCursorAndAttrs(char *str, int index);
    int insertRestorCursorAndAttrs(char *str, int index);
    int insertForceCursorPos(char *str, int index, int row, int col);

    void init();
    void drawTime(char *msg);
    void drawIdle(char *msg);
    void drawSensors(char *msg);
    void drawSwitch(char *msg);
    void drawTrain(char *msg);
};

#endif