#line 1 "D:\\Proiecte\\KittyKeeper\\esp32\\server_connection.cpp"
#include "server_connection.h"
#include <WiFi.h>
 

using namespace websockets;
ServerConnection::ServerConnection()
  : WebsocketsClient() 
{
    // this->setCACert(this->echo_org_ssl_ca_cert);
    // this->setPrivateKey(this->client_private_key);
    // this->setCertificate(this->client_cert);
    this->waitClockMs = millis();
}

bool ServerConnection::connect() {
    if(WiFi.status() != WL_CONNECTED){
        if(millis() - waitClockMs > 3000){
            Serial.println("Try connecting to Wifi");
            WiFi.mode(WIFI_STA);
            WiFi.begin(this->ssid, this->password);
            waitClockMs = millis();
        }
        return false;
    }

    if(!this->available()){
        if(millis() - waitClockMs > 2000){
            waitClockMs = millis();
            Serial.println("Try connecting to server");
            this->setCACert(ssl_cert);
            Serial.println( WebsocketsClient::connect(this->websockets_connection_string));
        }
        return false;
    }
    return true;
}


bool ServerConnection::isConnected() {
    return WiFi.status() == WL_CONNECTED && this->available();
}

bool ServerConnection::sendBinary(const uint8_t command, const char* data, const size_t len){
    WebsocketsClient::streamBinary();
    WebsocketsClient::sendBinary((const char*)&command, 1);
    if(data != NULL)
        WebsocketsClient::sendBinary(data, len);
    return WebsocketsClient::end();
}

bool ServerConnection::sendBinary(const char* data, const size_t len){
    WebsocketsClient::streamBinary();
    if(data != NULL)
        WebsocketsClient::sendBinary(data, len);
    return  WebsocketsClient::end();
}

bool ServerConnection::loop(){
    if(!this->ping()){
        if(!this->connect())
            return false;
    }
    return this->poll();
}


const char ServerConnection::ssl_cert[] PROGMEM = \
  "-----BEGIN CERTIFICATE-----\n" \
  "MIIFYDCCBEigAwIBAgIQQAF3ITfU6UK47naqPGQKtzANBgkqhkiG9w0BAQsFADA/\n" \
  "MSQwIgYDVQQKExtEaWdpdGFsIFNpZ25hdHVyZSBUcnVzdCBDby4xFzAVBgNVBAMT\n" \
  "DkRTVCBSb290IENBIFgzMB4XDTIxMDEyMDE5MTQwM1oXDTI0MDkzMDE4MTQwM1ow\n" \
  "TzELMAkGA1UEBhMCVVMxKTAnBgNVBAoTIEludGVybmV0IFNlY3VyaXR5IFJlc2Vh\n" \
  "cmNoIEdyb3VwMRUwEwYDVQQDEwxJU1JHIFJvb3QgWDEwggIiMA0GCSqGSIb3DQEB\n" \
  "AQUAA4ICDwAwggIKAoICAQCt6CRz9BQ385ueK1coHIe+3LffOJCMbjzmV6B493XC\n" \
  "ov71am72AE8o295ohmxEk7axY/0UEmu/H9LqMZshftEzPLpI9d1537O4/xLxIZpL\n" \
  "wYqGcWlKZmZsj348cL+tKSIG8+TA5oCu4kuPt5l+lAOf00eXfJlII1PoOK5PCm+D\n" \
  "LtFJV4yAdLbaL9A4jXsDcCEbdfIwPPqPrt3aY6vrFk/CjhFLfs8L6P+1dy70sntK\n" \
  "4EwSJQxwjQMpoOFTJOwT2e4ZvxCzSow/iaNhUd6shweU9GNx7C7ib1uYgeGJXDR5\n" \
  "bHbvO5BieebbpJovJsXQEOEO3tkQjhb7t/eo98flAgeYjzYIlefiN5YNNnWe+w5y\n" \
  "sR2bvAP5SQXYgd0FtCrWQemsAXaVCg/Y39W9Eh81LygXbNKYwagJZHduRze6zqxZ\n" \
  "Xmidf3LWicUGQSk+WT7dJvUkyRGnWqNMQB9GoZm1pzpRboY7nn1ypxIFeFntPlF4\n" \
  "FQsDj43QLwWyPntKHEtzBRL8xurgUBN8Q5N0s8p0544fAQjQMNRbcTa0B7rBMDBc\n" \
  "SLeCO5imfWCKoqMpgsy6vYMEG6KDA0Gh1gXxG8K28Kh8hjtGqEgqiNx2mna/H2ql\n" \
  "PRmP6zjzZN7IKw0KKP/32+IVQtQi0Cdd4Xn+GOdwiK1O5tmLOsbdJ1Fu/7xk9TND\n" \
  "TwIDAQABo4IBRjCCAUIwDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMCAQYw\n" \
  "SwYIKwYBBQUHAQEEPzA9MDsGCCsGAQUFBzAChi9odHRwOi8vYXBwcy5pZGVudHJ1\n" \
  "c3QuY29tL3Jvb3RzL2RzdHJvb3RjYXgzLnA3YzAfBgNVHSMEGDAWgBTEp7Gkeyxx\n" \
  "+tvhS5B1/8QVYIWJEDBUBgNVHSAETTBLMAgGBmeBDAECATA/BgsrBgEEAYLfEwEB\n" \
  "ATAwMC4GCCsGAQUFBwIBFiJodHRwOi8vY3BzLnJvb3QteDEubGV0c2VuY3J5cHQu\n" \
  "b3JnMDwGA1UdHwQ1MDMwMaAvoC2GK2h0dHA6Ly9jcmwuaWRlbnRydXN0LmNvbS9E\n" \
  "U1RST09UQ0FYM0NSTC5jcmwwHQYDVR0OBBYEFHm0WeZ7tuXkAXOACIjIGlj26Ztu\n" \
  "MA0GCSqGSIb3DQEBCwUAA4IBAQAKcwBslm7/DlLQrt2M51oGrS+o44+/yQoDFVDC\n" \
  "5WxCu2+b9LRPwkSICHXM6webFGJueN7sJ7o5XPWioW5WlHAQU7G75K/QosMrAdSW\n" \
  "9MUgNTP52GE24HGNtLi1qoJFlcDyqSMo59ahy2cI2qBDLKobkx/J3vWraV0T9VuG\n" \
  "WCLKTVXkcGdtwlfFRjlBz4pYg1htmf5X6DYO8A4jqv2Il9DjXA6USbW1FzXSLr9O\n" \
  "he8Y4IWS6wY7bCkjCWDcRQJMEhg76fsO3txE+FiYruq9RUWhiF1myv4Q6W+CyBFC\n" \
  "Dfvp7OOGAN6dEOM4+qR9sdjoSYKEBpsr6GtPAQw4dy753ec5\n" \
  "-----END CERTIFICATE-----\n";


#ifdef CLUJ_WIFI
const char ServerConnection::ssid[] PROGMEM = "Faster-Faster";
const char ServerConnection::password[] PROGMEM = "voidmain()";
// const char ServerConnection::websockets_connection_string[] PROGMEM = "wss://server-m5viojw2ba-lm.a.run.app/connect/robot";
const char ServerConnection::websockets_connection_string[] PROGMEM = "ws://192.168.1.136:5000/connect/robot";
#elif defined(PHONE_WIFI)
const char ServerConnection::ssid[] PROGMEM = "BRG";
const char ServerConnection::password[] PROGMEM = "brgbrgbrg3";
const char ServerConnection::websockets_connection_string[] PROGMEM = "ws://192.168.58.41:5000/connect/robot";
#elif defined(TURCENI_WIFI)
const char ServerConnection::ssid[] PROGMEM = "DIGI_ea72ea";
const char ServerConnection::password[] PROGMEM = "5de30098";
const char ServerConnection::websockets_connection_string[] PROGMEM = "ws://192.168.1.4:5000/connect/robot";
#else
const char ServerConnection::ssid[] PROGMEM = "BRG-PC";
const char ServerConnection::password[] PROGMEM = "brgbrgbrg3";
const char ServerConnection::websockets_connection_string[] PROGMEM = "ws://192.168.137.1:5000/connect/robot";
#endif