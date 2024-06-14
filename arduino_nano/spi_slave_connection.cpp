#include "spi_slave_connection.h"
#include <SPI.h>
#include <Arduino.h>

SpiSlaveConnection::SpiSlaveConnection(){}

void SpiSlaveConnection::init(){
    rxQueue.clear();
    txData[0]  = (uint8_t)COMMAND_DISTANCE_DATA;
    txData[14]  = (uint8_t)COMMAND_RING_EDGE_DATA;
    txByteIndex = 0;
    bytes_to_receive = 0;
    state = IDLE;
    // Initialize SPI pins and SPI control register as before
    pinMode(MISO, OUTPUT);
    pinMode(SS, INPUT_PULLUP);
    SPCR |= _BV(SPE); // Enable SPI
    SPI.attachInterrupt(); // Attach SPI interrupt
    

}
void SpiSlaveConnection::addData(const uint8_t command, const uint8_t data){
    addData(command, &data);
}
void SpiSlaveConnection::addData(const uint8_t command, const uint8_t data[]){
    if (command == COMMAND_DISTANCE_DATA){
        for(int i = 0; i < 13; i ++){
            txData[i + 1] = data[i];
        }
    }else if(command == COMMAND_RING_EDGE_DATA){
        txData[15] = data[0];
    }
}
uint8_t SpiSlaveConnection::getData(uint8_t data[]){
    uint8_t n = 0;
    while(rxQueue.getNoElements() > 0){
        data[n++] = rxQueue.get();
    }
    return n;
}
static inline uint8_t popcount(uint8_t x) {
    x = x - ((x >> 1) & 0x55);        // Put count of each 2 bits into those 2 bits
    x = (x & 0x33) + ((x >> 2) & 0x33); // Put count of each 4 bits into those 4 bits
    x = (x + (x >> 4)) & 0x0F;          // Put count of each 8 bits into those 8 bits
    return x;                           // Total count for all bits in the byte
}

void SpiSlaveConnection::interrupt() {
    if(state == IDLE ){
        if(SPDR == 0xFF){
            nrof1Transmitted = popcount(BUFFER_CAPACTITY);
            txByteIndex = 0;
            state = BYTES_TO_RECEIVE;
            SPDR = BUFFER_CAPACTITY;
            
        }
    }else if(state == BYTES_TO_RECEIVE){
        //No bytes to transfer state
        bytes_to_receive = SPDR;
        if (txByteIndex < BUFFER_CAPACTITY) {
            uint8_t txByte = txData[txByteIndex++];
            nrof1Transmitted += popcount(txByte);
            SPDR = txByte;
        }
        state = RECEIVING;
    }else if(state == RECEIVING){
        //Receiving state
        if (bytes_to_receive > 0) {
            rxQueue.add(SPDR);
            --bytes_to_receive;
        }
        if (txByteIndex < BUFFER_CAPACTITY) {
            uint8_t txByte = txData[txByteIndex++];
            nrof1Transmitted += popcount(txByte);
            SPDR = txByte;
        }
        if (bytes_to_receive == 0 && txByteIndex >= BUFFER_CAPACTITY) {
            rxQueue.commitData();
            state = NR_OF_1;
        }
    }else{
        SPDR = nrof1Transmitted;
        state = IDLE;
    }
}