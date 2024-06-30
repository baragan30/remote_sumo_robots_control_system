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
        // if(this->wifi_no == 1){
        //     Serial.print("Try connecting to Wifi: ");
        //     Serial.println(this->SSID1);
        //     WiFi.begin(this->SSID1, this->PASSWORD1);
        // }else if(this->wifi_no == 2){
        //     Serial.print("Try connecting to Wifi: ");
        //     Serial.println(this->SSID2);
        //     WiFi.begin(this->SSID2, this->PASSWORD2);
        // }else if(this->wifi_no == 3){
            Serial.print("Try connecting to Wifi: ");
            Serial.println(this->SSID3);
            WiFi.begin(this->SSID3, this->PASSWORD3);
        // }else {
        //     this->wifi_no = 3;
        // }
        const unsigned long connectionTimeout = 3000;
        unsigned long startAttemptTime = millis() + connectionTimeout;
        
        while (WiFi.status() != WL_CONNECTED && millis() < connectionTimeout);
        if(WiFi.status() == WL_CONNECTED){
            IPAddress localIP = WiFi.localIP();
            IPAddress serverIp(localIP[0], localIP[1], localIP[2], this->STATIC_IP);
            this->url = "ws://" + serverIp.toString() + ":5000/connect/robot";
        }else {
            // this->wifi_no = (this->wifi_no) % 2 + 1;
            return false;
        }
    }

    if(!this->available()){
        Serial.println("Try connecting to " + this->url);
        WebsocketsClient::connect(this->url);
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
    if(!ping()){
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

const uint8_t ServerConnection::STATIC_IP PROGMEM = 136;

const char ServerConnection::SSID1[] PROGMEM = "Faster-Faster";
const char ServerConnection::PASSWORD1[] PROGMEM = "voidmain()";

const char ServerConnection::SSID2[] PROGMEM = "BRG";
const char ServerConnection::PASSWORD2[] PROGMEM = "brgbrgbrg3";

const char ServerConnection::SSID3[] PROGMEM = "Robot";
const char ServerConnection::PASSWORD3[] PROGMEM = "12345678";

