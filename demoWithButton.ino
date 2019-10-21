#include <wirelesscoffee.h>

WirelessCoffee* coffee;

int switchPin = 5;
int switchState = HIGH;
int switchReading;
int switchPrevious = LOW;
long switchTime = 0;
long switchDebounce = 200;
int greenLedPin = 2;
int yellowLedPin = 3;
int redLedPin = 4;
int myCoffeeStatus = WirelessCoffee::COFFEE_STATUS::NOSTATUS;
int lastMyCoffeeStatus = -1;

void setup() {

  Serial.begin(9600);
  coffee = new WirelessCoffee(7,8, &Serial);
  
  Serial.println("Setup complete, send Y to request coffee, N to say you have coffee");
  
  pinMode(greenLedPin, OUTPUT);
  pinMode(yellowLedPin, OUTPUT);
  pinMode(redLedPin, OUTPUT);
  pinMode(switchPin, INPUT_PULLUP);
}
void loop() {
  if (Serial.available() > 0) 
  {
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

  switchReading = digitalRead(switchPin);

  if (switchReading == HIGH && switchPrevious == LOW && (millis() - switchTime) > switchDebounce)
  {
    if (switchState == HIGH)
    {
      switchState = LOW;
      coffee->sendGotCoffee(&Serial);
      myCoffeeStatus = WirelessCoffee::COFFEE_STATUS::GOTCOFFEE;
    }
    else
    {
      coffee->sendNeedCoffee(&Serial);
      switchState = HIGH;
      myCoffeeStatus = WirelessCoffee::COFFEE_STATUS::NEEDCOFFEE;
    }

    switchTime = millis();
  }

  switchPrevious = switchReading;
  
  char coffeeStatus = coffee->listenForStatus(&Serial);
  if (coffeeStatus != WirelessCoffee::COFFEE_STATUS::NOSTATUS)
    myCoffeeStatus = coffeeStatus;
    
  if (myCoffeeStatus != lastMyCoffeeStatus)
  {
    switch(myCoffeeStatus)
    {
      case WirelessCoffee::COFFEE_STATUS::NEEDCOFFEE:
        Serial.println("Someone needs coffee");
        digitalWrite(greenLedPin,HIGH);
        digitalWrite(yellowLedPin,LOW);
        digitalWrite(redLedPin,LOW);
      break;
      case WirelessCoffee::COFFEE_STATUS::GOTCOFFEE:
        Serial.println("Someone has coffee");
        digitalWrite(greenLedPin,LOW);
        digitalWrite(yellowLedPin,HIGH);
        digitalWrite(redLedPin,LOW);
      break;
      default:
        Serial.println("I dunno what I received");
        digitalWrite(greenLedPin,LOW);
        digitalWrite(yellowLedPin,LOW);
        digitalWrite(redLedPin,HIGH);
      break;
    }
  }
  lastMyCoffeeStatus = myCoffeeStatus;
}
