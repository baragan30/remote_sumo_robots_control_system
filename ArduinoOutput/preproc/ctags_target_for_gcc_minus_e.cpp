# 1 "D:\\Proiecte\\KittyKeeper\\esp32\\esp32.ino"
# 2 "D:\\Proiecte\\KittyKeeper\\esp32\\esp32.ino" 2
# 3 "D:\\Proiecte\\KittyKeeper\\esp32\\esp32.ino" 2
# 4 "D:\\Proiecte\\KittyKeeper\\esp32\\esp32.ino" 2

# 6 "D:\\Proiecte\\KittyKeeper\\esp32\\esp32.ino" 2


using namespace websockets;
ServerConnection serverConnection;
SpiMasterCommunication spiCommunication;

const int FLASH_GPIO_NUM = 4;
const uint8_t ROBOT_UUID_SIZE = 16;
char robot_uuid[ROBOT_UUID_SIZE];
long long lastLoginClock = millis();

bool gotFirstUUID = false;
bool authentificatedWithUID = false;
bool transmitVideStreaming = true;


static const uint8_t MAX_LOOP_DURATIONS = 30; // Defines the number of loop durations to store
uint8_t currentDurationIndex = 0; // Index to keep track of the current loop duration entry
uint32_t loopDurations[MAX_LOOP_DURATIONS];
uint32_t totalLoopDuration;

enum Commands{
    COMMAND_REGISTER = 0x00,
    COMMAND_LINK = 0x01,
    COMMAND_CONNECTION = 0x02,
    COMMAND_VIDEO_STREAMING = 0x04,
    COMMAND_FRAME = 0x05,
    COMMAND_MOTOR_POWER =0x06,
    COMMAND_DISTANCE_DATA=0x07,
    COMMAND_RING_EDGE_DATA= 0x08,
};


void try_login(){
    if(gotFirstUUID == false){
        serverConnection.sendBinary((char)COMMAND_REGISTER,
# 41 "D:\\Proiecte\\KittyKeeper\\esp32\\esp32.ino" 3 4
                                                          __null
# 41 "D:\\Proiecte\\KittyKeeper\\esp32\\esp32.ino"
                                                              , 0);
    }else{
        serverConnection.sendBinary((char)COMMAND_REGISTER, robot_uuid, ROBOT_UUID_SIZE);
    }
}


void onMessageCallback(WebsocketsMessage message) {
    const uint8_t *s = (uint8_t *)message.c_str();
    uint8_t command = s[0];
    switch(command){
        case COMMAND_REGISTER:
            memcpy(robot_uuid, s+1, ROBOT_UUID_SIZE);
            gotFirstUUID = true;
            authentificatedWithUID = true;
            spiCommunication.addData(COMMAND_CONNECTION, 0x01);
        break;
        case COMMAND_LINK:
        break;
        case COMMAND_CONNECTION:
        break;
        case COMMAND_VIDEO_STREAMING:
            transmitVideStreaming = (bool) s[1];
        break;
        case COMMAND_FRAME:
        break;
        case COMMAND_MOTOR_POWER:
            Serial.print(s[0]);
            Serial.print(" ");
            Serial.print(s[1]);
            Serial.print(" ");
            Serial.print(s[2]);
            Serial.println(" ");
            spiCommunication.addData(s, 3);
        break;

        default:
        break;
    }
}
void onEventsCallback(WebsocketsEvent event, String data) {
    if(event == WebsocketsEvent::ConnectionOpened) {
        Serial.println("Connnection Opened");
    } else if(event == WebsocketsEvent::ConnectionClosed) {
        authentificatedWithUID = false;
        Serial.println("Connnection Closed");
    } else if(event == WebsocketsEvent::GotPing) {
        // Serial.println("Got a Ping!");
    } else if(event == WebsocketsEvent::GotPong) {
        // Serial.println("Got a Pong!");
    }
}

void initCamera(){
    camera_config_t config;
    config.ledc_channel = LEDC_CHANNEL_0;
    config.ledc_timer = LEDC_TIMER_0;
    config.pin_d0 = 5;
    config.pin_d1 = 18;
    config.pin_d2 = 19;
    config.pin_d3 = 21;
    config.pin_d4 = 36;
    config.pin_d5 = 39;
    config.pin_d6 = 34;
    config.pin_d7 = 35;
    config.pin_xclk = 0;
    config.pin_pclk = 22;
    config.pin_vsync = 25;
    config.pin_href = 23;
    config.pin_sscb_sda = 26;
    config.pin_sscb_scl = 27;
    config.pin_pwdn = 32;
    config.pin_reset = -1;
    config.xclk_freq_hz = 20000000;
    config.pixel_format = PIXFORMAT_JPEG;
    config.grab_mode = CAMERA_GRAB_LATEST;
    config.frame_size = FRAMESIZE_QVGA; //FRAMESIZE_96X96; //FRAMESIZE_QQVGA; //FRAMESIZE_SVGA;
    config.jpeg_quality = 25;
    config.fb_count = 1;
    config.fb_location = CAMERA_FB_IN_DRAM;
    esp_err_t err = esp_camera_init(&config);
    if (err != 0 /*!< esp_err_t value indicating success (no error) */) {
        Serial.printf("Msg: Camera init failed with error 0x%x", err);
        return;
    }
    sensor_t * s = esp_camera_sensor_get();
    // drop down frame size for higher initial frame rate
    s->set_framesize(s, FRAMESIZE_QVGA);
}


void setup() {
    Serial.begin(250000);
    Serial.println("Programmed started");
    serverConnection.onMessage(onMessageCallback);
    serverConnection.onEvent(onEventsCallback);
    serverConnection.connect();
    initCamera();
    spiCommunication.init();
    for(int i = 0; i < MAX_LOOP_DURATIONS; i++){
        loopDurations[i] = 0;
    }
    totalLoopDuration = 0;
}

long long sterge = millis();

void transmitSensorsData(){
    uint8_t *p = spiCommunication.getReceivedData();
    uint8_t size = spiCommunication.getReceivedDataSize();
    const uint8_t *end = p + size;


    bool corect = true;
    for(uint8_t i = 1; i < 8; i ++){
        if(p[i] != i){
          corect = false;
          break;
        }
    }
    if(!corect && size > 0){
      Serial.println("size = ");
      Serial.println(size);
       for(uint8_t * i = p; i < end; i ++){
          Serial.print(i[0]);
          Serial.print(" ");
      }Serial.println();
    }


    // while(p < end){

    //     switch(p[0]){
    //         case COMMAND_DISTANCE_DATA:
    //             serverConnection.sendBinary((const char *)p, 14);
    //             p+=14;
    //         break;
    //         case COMMAND_RING_EDGE_DATA:
    //             serverConnection.sendBinary((const char *)p, 2);
    //             p+=2;
    //         break;
    //         default:
    //             p++;
    //         break;
    //     }
    // }
}



uint16_t loopsPerSecond(){
  uint32_t timePerLoopInMicrosSec = (totalLoopDuration / MAX_LOOP_DURATIONS);
  if(timePerLoopInMicrosSec == 0)
    timePerLoopInMicrosSec = 1;
  return 1000000/timePerLoopInMicrosSec;
}

void loop() {
    unsigned long startLoopTime = micros();

    //Eschange information with arduino nano
    spiCommunication.communication();

    // //Check and restablish internet connection
    // if(!serverConnection.loop() ){
    //     spiCommunication.addData(COMMAND_CONNECTION, 0x00);
    //     return;
    // }

    // //Authentificating to Server
    // if(!authentificatedWithUID){
    //     if(millis()- lastLoginClock > 500 ){
    //        Serial.println("Try Login...");
    //        try_login();
    //        lastLoginClock = millis();
    //     }
    //     spiCommunication.addData(COMMAND_CONNECTION, 0x00);
    //     return;
    // }

    //transmit Data getted from sensors
    transmitSensorsData();

    // // Transmit Video Freame
    // if(transmitVideStreaming &&  loopsPerSecond() > 20){
    //     camera_fb_t * fb = NULL;
    //     fb = esp_camera_fb_get();
    //     serverConnection.sendBinary(COMMAND_FRAME,(const char *)fb->buf, fb->len);
    //     esp_camera_fb_return(fb);
    // }
    // ! Delete this
    delay(10);

    uint32_t newLoopDuration = micros() - startLoopTime;
    totalLoopDuration = totalLoopDuration + newLoopDuration - loopDurations[currentDurationIndex];
    loopDurations[currentDurationIndex] = newLoopDuration;
    currentDurationIndex = (currentDurationIndex + 1) % MAX_LOOP_DURATIONS;
    // Serial.print("Loops per second ");
    // Serial.println(loopsPerSecond());
}
# 1 "D:\\Proiecte\\KittyKeeper\\esp32\\_esp32.ino"
// #include <WiFi.h>
// #include <ArduinoWebsockets.h>

// using namespace websockets;

// WebsocketsClient client;
// WiFiClient espClient;

// const char* ssid = "Faster-Faster";
// const char* password = "voidmain()";

// /*
// To get the SSL certificate run: 

//     openssl s_client -showcerts -connect socketsbay.com:443

// See https://github.com/gilmaimon/ArduinoWebsockets/issues/101 for details.
// */
// const char ssl_cert[]  = //   "-----BEGIN CERTIFICATE-----\n" //   "MIIFYDCCBEigAwIBAgIQQAF3ITfU6UK47naqPGQKtzANBgkqhkiG9w0BAQsFADA/\n" //   "MSQwIgYDVQQKExtEaWdpdGFsIFNpZ25hdHVyZSBUcnVzdCBDby4xFzAVBgNVBAMT\n" //   "DkRTVCBSb290IENBIFgzMB4XDTIxMDEyMDE5MTQwM1oXDTI0MDkzMDE4MTQwM1ow\n" //   "TzELMAkGA1UEBhMCVVMxKTAnBgNVBAoTIEludGVybmV0IFNlY3VyaXR5IFJlc2Vh\n" //   "cmNoIEdyb3VwMRUwEwYDVQQDEwxJU1JHIFJvb3QgWDEwggIiMA0GCSqGSIb3DQEB\n" //   "AQUAA4ICDwAwggIKAoICAQCt6CRz9BQ385ueK1coHIe+3LffOJCMbjzmV6B493XC\n" //   "ov71am72AE8o295ohmxEk7axY/0UEmu/H9LqMZshftEzPLpI9d1537O4/xLxIZpL\n" //   "wYqGcWlKZmZsj348cL+tKSIG8+TA5oCu4kuPt5l+lAOf00eXfJlII1PoOK5PCm+D\n" //   "LtFJV4yAdLbaL9A4jXsDcCEbdfIwPPqPrt3aY6vrFk/CjhFLfs8L6P+1dy70sntK\n" //   "4EwSJQxwjQMpoOFTJOwT2e4ZvxCzSow/iaNhUd6shweU9GNx7C7ib1uYgeGJXDR5\n" //   "bHbvO5BieebbpJovJsXQEOEO3tkQjhb7t/eo98flAgeYjzYIlefiN5YNNnWe+w5y\n" //   "sR2bvAP5SQXYgd0FtCrWQemsAXaVCg/Y39W9Eh81LygXbNKYwagJZHduRze6zqxZ\n" //   "Xmidf3LWicUGQSk+WT7dJvUkyRGnWqNMQB9GoZm1pzpRboY7nn1ypxIFeFntPlF4\n" //   "FQsDj43QLwWyPntKHEtzBRL8xurgUBN8Q5N0s8p0544fAQjQMNRbcTa0B7rBMDBc\n" //   "SLeCO5imfWCKoqMpgsy6vYMEG6KDA0Gh1gXxG8K28Kh8hjtGqEgqiNx2mna/H2ql\n" //   "PRmP6zjzZN7IKw0KKP/32+IVQtQi0Cdd4Xn+GOdwiK1O5tmLOsbdJ1Fu/7xk9TND\n" //   "TwIDAQABo4IBRjCCAUIwDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMCAQYw\n" //   "SwYIKwYBBQUHAQEEPzA9MDsGCCsGAQUFBzAChi9odHRwOi8vYXBwcy5pZGVudHJ1\n" //   "c3QuY29tL3Jvb3RzL2RzdHJvb3RjYXgzLnA3YzAfBgNVHSMEGDAWgBTEp7Gkeyxx\n" //   "+tvhS5B1/8QVYIWJEDBUBgNVHSAETTBLMAgGBmeBDAECATA/BgsrBgEEAYLfEwEB\n" //   "ATAwMC4GCCsGAQUFBwIBFiJodHRwOi8vY3BzLnJvb3QteDEubGV0c2VuY3J5cHQu\n" //   "b3JnMDwGA1UdHwQ1MDMwMaAvoC2GK2h0dHA6Ly9jcmwuaWRlbnRydXN0LmNvbS9E\n" //   "U1RST09UQ0FYM0NSTC5jcmwwHQYDVR0OBBYEFHm0WeZ7tuXkAXOACIjIGlj26Ztu\n" //   "MA0GCSqGSIb3DQEBCwUAA4IBAQAKcwBslm7/DlLQrt2M51oGrS+o44+/yQoDFVDC\n" //   "5WxCu2+b9LRPwkSICHXM6webFGJueN7sJ7o5XPWioW5WlHAQU7G75K/QosMrAdSW\n" //   "9MUgNTP52GE24HGNtLi1qoJFlcDyqSMo59ahy2cI2qBDLKobkx/J3vWraV0T9VuG\n" //   "WCLKTVXkcGdtwlfFRjlBz4pYg1htmf5X6DYO8A4jqv2Il9DjXA6USbW1FzXSLr9O\n" //   "he8Y4IWS6wY7bCkjCWDcRQJMEhg76fsO3txE+FiYruq9RUWhiF1myv4Q6W+CyBFC\n" //   "Dfvp7OOGAN6dEOM4+qR9sdjoSYKEBpsr6GtPAQw4dy753ec5\n" //   "-----END CERTIFICATE-----\n";
# 52 "D:\\Proiecte\\KittyKeeper\\esp32\\_esp32.ino"
// void setup() {
//   // put your setup code here, to run once:
//   Serial.begin(115200);
//   Serial.println("Hello, ESP32!");


//   // We start by connecting to a WiFi network
//   Serial.println();
//   Serial.print("Connecting to ");
//   Serial.println(ssid);

//   WiFi.mode(WIFI_STA);
//   WiFi.begin(ssid, password);

//   while (WiFi.status() != WL_CONNECTED) {
//     delay(500);
//     Serial.print(".");
//   }

//   Serial.println("WiFi connected");

//     client.setCACert(ssl_cert);
//     bool connected = false;
//     int port;
//     // while(!connected){
//         // connected = client.connect("wss://echo.websocket.org");
//         // connected = client.connect("echo.websocket.org",443,"/" );
//         connected = client.connect("wss://server-m5viojw2ba-lm.a.run.app:443/connect/robot");
//         // bool connected = client.connect("wss://192.168.1.136:5000/connect/robot");
//         // for(int port = 0 ; port <= 1024 && !connected; port++){
//         //     connected =client.connect("echo.websocket.org", port, "/");
//         // }
//         // connected = client.connect("wss://echo.websocket.org");
//         // connected = client.connect("echo.websocket.org", 443, "/");

//         if (connected) {
//             Serial.print("Connected : ");
//             Serial.println(port);
//         } else {
//             Serial.println("Connection failed.");
//         }

//         client.onMessage([&](WebsocketsMessage message) {
//             Serial.print("Got Message: ");
//             Serial.println(message.data());
//         });
//     // }

// }

// void loop() {
//   client.poll();
//   // client.send("HELLO");
// }
