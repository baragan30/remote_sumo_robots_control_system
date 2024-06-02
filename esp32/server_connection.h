#ifndef SERVER_CONNECTION_H 
#define SERVER_CONNECTION_H
#include <ArduinoWebsockets.h>

#define CLUJ_WIFI
// #define PHONE_WIFI
// #define TURCENI_WIFI
using namespace websockets;
class ServerConnection : public websockets::WebsocketsClient {
public:
    ServerConnection();
    bool connect();
    bool isConnected();
    bool sendBinary(const uint8_t command, const char* data, const size_t len);
    bool sendBinary(const char* data, const size_t len);
    bool loop();

private:
    static const char ssl_cert[] PROGMEM;
    static const char ssid[] PROGMEM; 
    static const char password[] PROGMEM; 
    static const char websockets_connection_string[] PROGMEM;
    unsigned long waitClockMs;

};

#endif