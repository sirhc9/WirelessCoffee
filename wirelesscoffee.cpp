#include <wirelesscoffee.h>

const byte WirelessCoffee::readAddress[6] = "_READ";
const byte WirelessCoffee::writeAddress[6] = "WRITE";

/*
PACKET STRUCTURE
 - Note max size is 32 bytes

All packets have this pre-pended:
  TYPE    BYTE[1] //Packet type, see below

Type 1 packets: (Coffee Status broadcast)
  SENDER  BYTE[5]
  STATUS  BYTE[1]
*/

WirelessCoffee::WirelessCoffee()
{
  coffeeStatus = COFFEE_STATUS::UNKNOWN;
}

void WirelessCoffee::begin(int CE, int CSN, char ID[6], HardwareSerial* serialDebug = nullptr)
{
  /*
  Arguments
  CE          - Pin on the Arduino for the CE connector
  CSN         - Pin on the Arduino for the CSN connector
  ID          - 5 bytes of whatever to identify you + a null terminator
                  Why 5 bytes? Because human names are often 5 letters long
  serialDebug - Send output debug to a serial connection
  */
  myID = ID;
  radio = new RF24(CE, CSN);
  radio->begin();
  radio->openReadingPipe(0, readAddress);
  radio->openWritingPipe(writeAddress);
  radio->setPALevel(RF24_PA_MIN);
  radio->startListening();
  if (serialDebug != nullptr)
  {
    Serial.print("Begin done, listening now.");
  }
}

void WirelessCoffee::loop(HardwareSerial* serialDebug = nullptr)
{
  if (radio->available())
  {
    char text[32] = "";
    radio->read(&text, sizeof(text));

    if (serialDebug != nullptr)
    {
      Serial.print("Received: ");
      Serial.println(text);
    }
  }
}

void WirelessCoffee::sendStatus(HardwareSerial* serialDebug = nullptr)
{
  const char text[] = "Hello World";
  
  int totalMessageSize = sizeof(MSG_TYPE) 
                            + sizeof(myID) 
                            + sizeof(coffeeStatus);
  
  char* message = new char[totalMessageSize];
                            
  //Compose the messsage
  int mPos = 0;                      
  message[mPos++] = MSG_TYPE::STATUS_BROADCAST;
  
  for(int i=0; i<sizeof(myID); ++i)
  {
    message[mPos++] = myID[i];
  }
  message[mPos++] = 0; //Null terminator
  message[mPos++] = coffeeStatus;
  
  if (totalMessageSize != mPos)
  {
    if (serialDebug != nullptr)
    {
      serialDebug->println("Composed message size isn't what was expected!");
    }
  }
  else
  {
    radio->stopListening();
    radio->write(message, mPos);
    delete message;
    radio->startListening();
    if (serialDebug != nullptr)
    {
      serialDebug->println("Message sent successfully");
    }
  }
}
