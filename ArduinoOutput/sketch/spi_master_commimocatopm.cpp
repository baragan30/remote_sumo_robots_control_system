#line 1 "D:\\Proiecte\\KittyKeeper\\esp32\\spi_master_commimocatopm.cpp"
#include "spi_master_communication.h"
#include <SPI.h>
#include <algorithm>
#include <Arduino.h>
const int8_t SPI_SCK_PIN = 14;
const int8_t SPI_MISO_PIN = 12;
const int8_t SPI_MOSI_PIN = 13;
const int8_t SPI_SS_PIN = 15;

static inline uint8_t popcount(uint8_t x) {
    x = x - ((x >> 1) & 0x55);        // Put count of each 2 bits into those 2 bits
    x = (x & 0x33) + ((x >> 2) & 0x33); // Put count of each 4 bits into those 4 bits
    x = (x + (x >> 4)) & 0x0F;          // Put count of each 8 bits into those 8 bits
    return x;                           // Total count for all bits in the byte
}


SpiMasterCommunication::SpiMasterCommunication(/* args */){};

void SpiMasterCommunication::init(){
    rxSize = 0;
    txSize = 0;
    SPI.begin(SPI_SCK_PIN, SPI_MISO_PIN,SPI_MOSI_PIN,SPI_SS_PIN);
    lastTransfer = micros();
}

void SpiMasterCommunication::addData(const uint8_t command,const uint8_t data){
    if(DATA_SIZE - txSize >= 2){
        txData[txSize++] = command;
        txData[txSize++] = data;
    }
}


void SpiMasterCommunication::addData(const uint8_t *data,const uint8_t size){
    if(DATA_SIZE - txSize >= size){
        for(uint8_t i = 0; i < size; i ++){
            txData[txSize++] = data[i];
        }
    }
    
}

uint8_t SpiMasterCommunication::transfer(uint8_t data){
    while(micros() - lastTransfer <= 20);
    uint8_t recData = SPI.transfer(data);
    lastTransfer = micros();
    return recData;
}

void SpiMasterCommunication::communication() {
    // Start SPI transaction
    SPI.beginTransaction(SPISettings(4000000, SPI_MSBFIRST, SPI_MODE0));
    transfer(0xFF);
    // Transmission of sizes and immediate data transfer
    rxSize = transfer(txSize);
    uint8_t nrOf1Received = popcount(rxSize);
    // Transmission of data
    uint8_t maxIterations = std::max(rxSize, txSize);
    rxSize = min(DATA_SIZE, rxSize);
    for (uint8_t i = 0; i < maxIterations; i++) {
        uint8_t txByte = (i < txSize) ? txData[i] : 0x00;  // Use dummy data if txSize exceeded
        uint8_t rxByte = transfer(txByte);

        if (i < rxSize) {
            nrOf1Received += popcount(rxByte);
            rxData[i] = rxByte;  // Store received data if within rxSize
        }
    }

    uint8_t nrOf1 =  transfer(0x00);
    
    // End SPI transaction
    SPI.endTransaction();

   
    //Post Processing
    // Serial.print(rxSize);
    // Serial.print(" ");
    Serial.print(nrOf1Received);
    Serial.print(" ");
    Serial.println(nrOf1);
    // for(uint8_t i = 0; i < rxSize; i ++){
    //     Serial.print(rxData[i]);
    //     Serial.print(" ");
    // }Serial.println();

    if(nrOf1 != nrOf1Received){
        Serial.println("Problem Detected");
        rxSize = 0;
    }

    // Reset transaction size
    txSize = 0;
}