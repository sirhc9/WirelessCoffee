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
  
  static const byte readAddress[6];
  static const byte writeAddress[6];
  
  struct MSG_TYPE
  {
    static const char STATUS_BROADCAST = 1;
  };
  
  enum class _COFFEE_STATUS: char
  {
    UNKNOWN = 0
  } COFFEE_STATUS;
  
  byte coffeeStatus;
  char myID[6];
  
  public:
    WirelessCoffee();
    void begin(int CE, int CSN, char ID[6], HardwareSerial* serialDebug = nullptr); //Initalises read/write communication
    void end();	 //For shutdown? (Needed?)
    void sleep(); //Enter low power mode
    void wake();  //Leave low power mode (Needed?)
    void requestStatus(); //Request coffee status from network
    void sendStatus(HardwareSerial* serialDebug = nullptr); //Broadcast coffee status to network
    void loop(HardwareSerial* serialDebug = nullptr); //Perform loop tasks
};

#endif /* WIRELESSCOFFEE_H_ */
