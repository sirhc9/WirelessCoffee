/*
  Todo: Quit wasting time and roast some beans
*/
#ifndef WIRELESSCOFFEE_H_
#define WIRELESSCOFFEE_H_

#define ADDR_SIZE 5

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

const byte broadcastAddress[ADDR_SIZE+1] = "BROAD"; //+1 to allow for the null terminator, this gets ignored later

class RFManager {
	
	//struct RFPacket;
	protected:
		
		struct RFPacket
		{
			char* myData;
			char writePos,readPos,myBufferSize;
			RFPacket(int size = 31);
			~RFPacket();
			bool writeCharArray(char* src, int srcSize);
			bool writeChar(char _char);
			bool readCharArray(char* dest, int sizeToRead);
			char readChar();
			RFPacket& operator = (const RFPacket& otherPacket) 
			{
				if (myBufferSize != otherPacket.myBufferSize)
				{
					delete[] myData;
					myBufferSize = otherPacket.myBufferSize;
					myData = new char[myBufferSize];//Don't worry about 0 initalisation, it's going to be overwritten
				}	
				
				for(int i=0; i<myBufferSize; ++i)
					myData[i] = otherPacket.myData[i];
				
				writePos = otherPacket.writePos;
				readPos = otherPacket.readPos;
					
				return *this;
			}
		};
		
	private:
  
		RF24* radio;
		RFPacket* waitingPacket;
		
  public:
  
    struct MSG_TYPE
    {
			static const char UNUSED = 0;
			static const char NORMAL_MESSAGE = 1;
      static const char ADDRESS_REQUEST = 2;
      static const char ADDRESS_RESPONSE = 3;
    };
    
    
    
    //RFManager();
    void begin(int CE, int CSN, HardwareSerial* serialDebug = nullptr); //Initalises read/write communication
    void SendMessage(RFPacket& packet, HardwareSerial* serialDebug = nullptr);
    //bool available(HardwareSerial* serialDebug = nullptr);
    RFPacket* tryReceive(HardwareSerial* serialDebug = nullptr);
		//RFPacket receive();
};

class WirelessCoffee : protected RFManager {
  //char coffeeStatus, lastCoffeeStatus;
  RFManager myRFManager;
  
  public:
  
	  struct COFFEE_STATUS
	  {
			static const char NOSTATUS = 0;
			static const char GOTCOFFEE = 1;
			static const char NEEDCOFFEE = 2;
	  };
  
    WirelessCoffee(int CE, int CSN, HardwareSerial* serialDebug = nullptr);
    void sendNeedCoffee(HardwareSerial* serialDebug = nullptr);
    void sendGotCoffee(HardwareSerial* serialDebug = nullptr);
    char listenForStatus(HardwareSerial* serialDebug = nullptr);
};

#endif /* WIRELESSCOFFEE_H_ */