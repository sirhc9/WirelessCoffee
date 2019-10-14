#include <wirelesscoffee.h>

WirelessCoffee::WirelessCoffee()
{
  
}

void WirelessCoffee::begin(int CE, int CSN, HardwareSerial* serialDebug = nullptr)
{
  const byte address[6] = "00001";
  radio = new RF24(CE, CSN);
  radio->begin();
  radio->openReadingPipe(0, address);
  radio->setPALevel(RF24_PA_MIN);
  radio->startListening();
  if (serialDebug != nullptr)
    Serial.print("Done!");
}

void WirelessCoffee::loop()
{
  if (radio->available()) {
    char text[32] = "";
    radio->read(&text, sizeof(text));
    //if (text == "Hello World")
      Serial.println(text);
  }
}
