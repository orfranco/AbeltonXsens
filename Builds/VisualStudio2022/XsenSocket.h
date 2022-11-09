#include <JuceHeader.h>
#include <map>
#include <iostream>
#include "../../NewProject/Builds/VisualStudio2022/vcpkg/installed/x64-windows-static/include/sio_client.h"


#define SRC_ADDRESS "http://127.0.0.1:3001"

using namespace sio;

class XsenSocket
{
    typedef std::function<void(std::string)> handlerFunc;

public:
    XsenSocket(handlerFunc);

private:
    std::unique_ptr<client> socketClient;
    handlerFunc handler;

    void onReceiveMsg(event& ev);
    void sensorConnect(event& ev);
    void sensorDisconnect(event& ev);
    void changeSampleRate(int sampleRate);
    void bindSocketEvents();
};

