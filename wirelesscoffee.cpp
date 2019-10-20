#include <wirelesscoffee.h>


//Max packet size is 31 bytes, 1 byte is stolen for the type header-
RFManager::RFPacket::RFPacket(int size)
{
  if (size > 31) size = 31;
  if (size < 0) size = 0;//0 size packet used as empty packet.
  myData = new char[size](0);
  writePos = 0;
  readPos = 0;
	myBufferSize = size;
}

RFManager::RFPacket::~RFPacket()
{
  delete[] myData;
}

bool RFManager::RFPacket::writeCharArray(char* src, int srcSize)
{
  for(char i=0; i<srcSize; ++i)
  {
    if (writePos >= myBufferSize) return false;
    myData[writePos++] = src[i];
  }
  return true;
}

bool RFManager::RFPacket::writeChar(char _char)
{
  if (writePos >= myBufferSize) return false;
  myData[writePos++] = _char;
  return true;
}

bool RFManager::RFPacket::readCharArray(char* src, int srcSize)
{
  for(char i=0; i<srcSize; ++i)
  {
    if (readPos >= myBufferSize) return false;
    src[readPos++] = myData[i];
  }
  return true;
}
char RFManager::RFPacket::readChar()
{
  if (readPos >= myBufferSize) return false; //This could cause confusing results....
  return myData[readPos++];
}

/*RFManager::RFManager()
{
  coffeeStatus = COFFEE_STATUS::UNKNOWN;
}*/

WirelessCoffee::WirelessCoffee(int CE, int CSN, HardwareSerial* serialDebug = nullptr)
{
  myRFManager.begin(CE, CSN, serialDebug);
  //coffeeStatus = COFFEE_STATUS::IDLE;
  //lastCoffeeStatus = coffeeStatus;
}

void WirelessCoffee::sendNeedCoffee(HardwareSerial* serialDebug = nullptr)
{
  RFManager::RFPacket outPacket;
	outPacket.writeChar(MSG_TYPE::NORMAL_MESSAGE);
  outPacket.writeChar(COFFEE_STATUS::NEEDCOFFEE);
  myRFManager.SendMessage(outPacket, serialDebug);
  //coffeeStatus = COFFEE_STATUS::NEEDCOFFEE;
}

void WirelessCoffee::sendGotCoffee(HardwareSerial* serialDebug = nullptr)
{
  RFManager::RFPacket outPacket;
  outPacket.writeChar(MSG_TYPE::NORMAL_MESSAGE);
	outPacket.writeChar(COFFEE_STATUS::GOTCOFFEE);
  myRFManager.SendMessage(outPacket, serialDebug);
  //coffeeStatus = COFFEE_STATUS::GOTCOFFEE;
}

char WirelessCoffee::listenForStatus(HardwareSerial* serialDebug = nullptr)
{
	/*if (myRFManager.available(serialDebug))
	{
		RFManager::RFPacket inPacket = myRFManager.receive();
		return inPacket.readChar();
	}
	else
		return COFFEE_STATUS::NOSTATUS;*/
	RFManager::RFPacket* inPacket = myRFManager.tryReceive(serialDebug);
	if (inPacket == nullptr)
		return COFFEE_STATUS::NOSTATUS;
	else
		return inPacket->readChar();
}

void RFManager::begin(int CE, int CSN, HardwareSerial* serialDebug = nullptr)
{
  /*
  Arguments
  CE          - Pin on the Arduino for the CE connector
  CSN         - Pin on the Arduino for the CSN connector
  ID          - 1 byte to identify the device (like an IP address) 
  serialDebug - Send output debug to a serial connection
  */
  radio = new RF24(CE, CSN);
  radio->begin();
	radio->enableDynamicPayloads();
  radio->openWritingPipe(broadcastAddress);//Pipe 0 is the only pipe allowed to write as per NRF24L01 spec
  radio->openReadingPipe(1, broadcastAddress);
  radio->setPALevel(RF24_PA_MIN);
  radio->startListening();
  if (serialDebug != nullptr)
  {
    serialDebug->println("Begin done, listening now.");
  }
}

/*RFManager::RFPacket RFManager::receive()
{
	if (waitingPacket == nullptr) return RFManager::RFPacket(0); //User called then when there is no packet, reply with an empty packet.
	RFManager::RFPacket returnPacket = *waitingPacket; //Copy the heap packet to stack packet
	delete waitingPacket; //Destroy the heap packet
	waitingPacket = nullptr; //Reset the pointer
	return returnPacket; //Return the stack packet
}*/

//bool RFManager::available(HardwareSerial* serialDebug = nullptr)
RFManager::RFPacket* RFManager::tryReceive(HardwareSerial* serialDebug = nullptr)
{
	//if (waitingPacket != nullptr) return true; //The user must collect the current packet before reading more
  if (radio->available())
  {
		/*
		1 byte is for the RF Manager to determine what the packet is for.
		
		If the packet type is a reserved type we won't return that data to the user
		*/
    RFManager::RFPacket* inPacket = new RFManager::RFPacket(radio->getDynamicPayloadSize());
    
		//radio->read(&messageType, 1);
		radio->read(inPacket->myData, inPacket->myBufferSize);
		
    char messageType = inPacket->readChar();
		
		if (serialDebug != nullptr)
		{
			serialDebug->print("Received ");
			serialDebug->print(inPacket->myBufferSize, DEC);
			serialDebug->print(" bytes; first is: ");
			serialDebug->print(messageType, DEC);
			serialDebug->println();
		}
    
    switch(messageType)
    {
      /*case MSG_TYPE::ADDRESS_REQUEST:
      {
				RFPacket outPacket;
				outPacket.writeChar()
        inPacket.readCharArray(readAddress, ADDR_SIZE);
				inPacket.readCharArray(writeAddress, ADDR_SIZE);
        return true;
      }
			
			case MSG_TYPE::ADDRESS_RESPONSE:
      {
        inPacket.readCharArray(readAddress, ADDR_SIZE);
				inPacket.readCharArray(writeAddress, ADDR_SIZE);
        return true;
      }*/
			
			case MSG_TYPE::NORMAL_MESSAGE:
      {
				//waitingPacket = inPacket;
				return inPacket;
			}
      
      default:
      {//Unknown message type
        if (serialDebug != nullptr)
        {
          serialDebug->print("Received unknown message type: ");
          serialDebug->println(messageType);
        }
				delete inPacket;
        return nullptr;
      }
    }
  }
}

void RFManager::SendMessage(RFPacket& packet, HardwareSerial* serialDebug = nullptr)
{
	radio->stopListening();
	/*char messageType = MSG_TYPE::NORMAL_MESSAGE;
	char* tbuff = new char[1+packet.writePos];
	tbuff[0] = messageType;
	for(int i=0; i<packet.writePos; ++i)
			tbuff[i+1] = packet.myData[i];
	radio->write(tbuff, sizeof(tbuff));*/
	
	if (serialDebug != nullptr)
	{
		serialDebug->print(packet.writePos, DEC);
		serialDebug->print(" to send: ");
		for(char i=0; i<packet.writePos; ++i)
		{
			serialDebug->print(packet.myData[i],DEC);
			serialDebug->print(" ");
		}
		serialDebug->println();
	}
				
	radio->write(packet.myData, packet.writePos);
  radio->startListening();
}