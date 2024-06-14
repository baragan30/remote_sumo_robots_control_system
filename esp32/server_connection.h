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
    static const char SSID1[] PROGMEM;
    static const char PASSWORD1[] PROGMEM;
    static const char SSID2[] PROGMEM;
    static const char PASSWORD2[] PROGMEM;
    static const uint8_t STATIC_IP PROGMEM;

    unsigned long waitClockMs;
    String url;
    uint8_t wifi_no  = 2;

};

#endif