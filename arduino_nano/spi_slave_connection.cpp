#include "spi_slave_connection.h"
#include <SPI.h>
#include <Arduino.h>

SpiSlaveConnection::SpiSlaveConnection(){}

void SpiSlaveConnection::init(){
    rxQueue.clear();
    txQueue.clear();
    bytes_to_transmit = 0;
    bytes_to_receive = 0;
    state = IDLE;
    // Initialize SPI pins and SPI control register as before
    pinMode(MISO, OUTPUT);
    pinMode(SS, INPUT_PULLUP);
    SPCR |= _BV(SPE); // Enable SPI
    SPI.attachInterrupt(); // Attach SPI interrupt

}
void SpiSlaveConnection::addData(const uint8_t command, const uint8_t data){
    addData(command, &data, 1);
}
void SpiSlaveConnection::addData(const uint8_t command, const uint8_t data[],const uint8_t dataSize){
    if(txQueue.getEmptySpace() < dataSize + 1)
        return;
    txQueue.add(command);
    for(uint8_t i = 0 ; i < dataSize ; i++)
        txQueue.add(data[i]);
    txQueue.commitData();
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
            state = NO_BYTES;
            bytes_to_transmit = txQueue.getNoElements();
            nrof1Transmittted = popcount(bytes_to_transmit);
            SPDR = bytes_to_transmit = txQueue.getNoElements();
        }
    }else if(state == NO_BYTES){
        //No bytes to transfer state
        bytes_to_receive = SPDR;
        if (bytes_to_transmit > 0) {
            uint8_t txData = txQueue.get();
            nrof1Transmittted += popcount(txData);
            SPDR = txData;
            --bytes_to_transmit;
        }
        state = RECEIVING;
    }else if(state == RECEIVING){
        //Receiving state
        if (bytes_to_receive > 0) {
            rxQueue.add(SPDR);
            --bytes_to_receive;
        }
        if (bytes_to_transmit > 0) {
            uint8_t txData = txQueue.get();
            nrof1Transmittted += popcount(txData);
            SPDR = txData;
            --bytes_to_transmit;
        }
        if (bytes_to_receive == 0 && bytes_to_transmit == 0) {
            rxQueue.commitData();
            state = IDLE;
        }
    }else{
        SPDR = nrof1Transmittted;
        state = IDLE;
    }
}