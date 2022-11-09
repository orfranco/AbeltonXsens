#include "XsenSocket.h"


XsenSocket::XsenSocket(handlerFunc handler)
{
    // initialize AnalyzerSocket:
    this->socketClient = std::make_unique<client>();
    this->socketClient->connect(SRC_ADDRESS);
    this->bindSocketEvents();
    this->handler = handler;
}


void XsenSocket::onReceiveMsg(event& ev) {
    std::string msg = ev.get_message()->get_string();
    this->handler(msg);
}

/**
   binded to sensor-connect, this method updates this Module when a sensor connected
   to the Xsens Module.
   creates a sensor object and add it to the currConnectedSensors hashmap.
*/
void XsenSocket::sensorConnect(event& ev) {
    //TODO:implement.
}

/**
   binded to sensor-disconnect, this method updates this Module when a sensor disconnected
   from the Xsens Module.
   deletes the disconnected sensor from the currConnectedSensors hashmap.
*/
void XsenSocket::sensorDisconnect(event& ev) {
    //TODO:implement.
}

/**
   this method send message to Xsens Module for changing the sample rate.
*/
void XsenSocket::changeSampleRate(int sampleRate) {
    //TODO:implement.
}


// bind Socket Events:
void XsenSocket::bindSocketEvents() {
    std::function<void(event& event)> const& func1 = std::bind(&XsenSocket::onReceiveMsg, this, std::placeholders::_1);
    this->socketClient->socket()->on("recieve-data", func1);
    std::function<void(event& event)> const& func2 = std::bind(&XsenSocket::sensorConnect, this, std::placeholders::_1);
    this->socketClient->socket()->on("sensor-connect", func2);
    std::function<void(event& event)> const& func3 = std::bind(&XsenSocket::sensorDisconnect, this, std::placeholders::_1);
    this->socketClient->socket()->on("sensor-disconnect", func3);
}
