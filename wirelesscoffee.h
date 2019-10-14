/*
  Todo: Quit wasting time and roast some beans
*/
#ifndef WIRELESSCOFFEE_H_
#define WIRELESSCOFFEE_H_

class WirelessCoffee {
  public:
    begin(); //Initalises read/write communication
    end();	 //For shutdown? (Needed?)
    sleep(); //Enter low power mode
    wake();  //Leave low power mode (Needed?)
    requestStatus(); //Request coffee status from network
    sendStatus(); //Broadcast coffee status to network
};

#endif /* WIRELESSCOFFEE_H_ */
