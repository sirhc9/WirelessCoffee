/*
  Todo: Quit wasting time and roast some beans
*/
#ifndef WIRELESSCOFFEE_H_
#define WIRELESSCOFFEE_H_

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

class WirelessCoffee {
  
  RF24* radio;
  
  public:
    WirelessCoffee();
    void begin(int CE, int CSN, HardwareSerial* serialDebug = nullptr); //Initalises read/write communication
    void end();	 //For shutdown? (Needed?)
    void sleep(); //Enter low power mode
    void wake();  //Leave low power mode (Needed?)
    void requestStatus(); //Request coffee status from network
    void sendStatus(); //Broadcast coffee status to network
    void loop(); //Perform loop tasks
};

#endif /* WIRELESSCOFFEE_H_ */
