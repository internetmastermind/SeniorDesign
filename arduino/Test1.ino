
// Pin connections:
int cardReaderIn = 41;  // Digital In
int limitSwitchIn = 40; // Digital In
int enableButton = 36;  // Digital In
int disableButton = 37; // Digital In
int relayOut = 44;      // Digital Out
int led1 = 30;          // Digital Out
int led2 = 31;          // Digital Out
int led3 = 32;          // Digital Out

// NOTE: Transceiver pins connected to SPI bus

void setup() {
  Serial.begin(9600);

  // Assign input pins
  pinMode(cardReaderIn,INPUT);
  pinMode(limitSwitchIn,INPUT_PULLUP);
  pinMode(enableButton,INPUT_PULLUP);
  pinMode(disableButton,INPUT_PULLUP);

  // Assign output pins
  pinMode(relayOut,OUTPUT);
  pinMode(led1,OUTPUT);
  pinMode(led2,OUTPUT);
  pinMode(led3,OUTPUT);
  
  Serial.print("\n\nHello c:\n");
  
}

void loop() {
  
  if((!digitalRead(cardReaderIn)) && (!digitalRead(limitSwitchIn))){ // If signal recieved from card reader
    digitalWrite(led1,HIGH);     // Signal LED
    Serial.print("\nCard reader signal detected\n");

    while(!digitalRead(limitSwitchIn)) { // While card inserted
      
      if(!digitalRead(enableButton)) { // If enable pressed
        digitalWrite(led2,HIGH);
        Serial.print("\nEnable button pressed\n");
        delay(1000);
      }
      else if(!digitalRead(disableButton)) { // If disable pressed
        digitalWrite(led2,LOW);
        Serial.print("\nDisable button pressed\n");
        delay(1000);
      }
      
    }
    delay(5000);
  }
  else {
    digitalWrite(led1,LOW);
    digitalWrite(led2,LOW);
    Serial.print("\n...nothin' detected...\n");
    delay(5000);
  }
  
}
