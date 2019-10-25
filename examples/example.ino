#include <wirelesscoffee.h>

WirelessCoffee* coffee;

void setup() {

  Serial.begin(9600);
  coffee = new WirelessCoffee(7,8, &Serial);
  
  Serial.println("Setup complete, send Y to request coffee, N to say you have coffee");
}

void loop() {
  if (Serial.available() > 0) {
    // read the incoming byte:
    char incomingByte = Serial.read();
    if (incomingByte == 'Y' || incomingByte == 'N')
    {  
      // say what you got:
      Serial.print("I received: ");
      Serial.print(incomingByte);
      Serial.print(" (");
      Serial.print(incomingByte, DEC);
      Serial.println(")");
      if (incomingByte == 'Y')
      {
        Serial.println("Sending want coffee");
        coffee->sendNeedCoffee(&Serial);
      }
      if (incomingByte == 'N')
      {
        Serial.println("Sending don't need coffee");
        coffee->sendGotCoffee(&Serial);
      }
    }
  }

  char coffeeStatus = coffee->listenForStatus(&Serial);
  if (coffeeStatus)
  {
    switch(coffeeStatus)
    {
      case WirelessCoffee::COFFEE_STATUS::NEEDCOFFEE:
        Serial.println("Need coffee bro");
      break;
      
      case WirelessCoffee::COFFEE_STATUS::GOTCOFFEE:
        Serial.println("Nah I got coffee");
      break;
      
      default:
        Serial.println("Duh what");
      break;
    }
  }
}
