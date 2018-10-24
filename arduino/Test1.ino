#include <SPI.h>
#include <RF24.h>

// Pin connections:
int cardReaderIn = 41;  // Digital In, active low
int limitSwitchIn = 40; // Digital In, active low
int enableButton = 36;  // Digital In, active low
int disableButton = 37; // Digital In, active low
int relayOut = 44;      // Digital Out
int led1 = 30;          // Digital Out, indicates if student has access
int led2 = 31;          // Digital Out, indicates if machine enabled
int led3 = 32;          // Digital Out, indicates if card removed
int speaker = 26;       // Tone pin

// NOTE: Transceiver pins connected to SPI bus

RF24 radio(9,53);

int machineState = 0; // data to be sent to pi; 1:machine enabled, 0:machine disabled
int count = 0; // used when card removed while machine enabled

void setup() {
  Serial.begin(9600);
  radio.begin();

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

  radio.openWritingPipe(0xF0F0F0F0E1);
  radio.stopListening();
  
  Serial.print("\n\nHello c:\n");
  
}

void loop() {
  
  // If signal recieved from card reader and card inserted
  if( (!digitalRead(cardReaderIn)) && (!digitalRead(limitSwitchIn)) ){ 
        
    digitalWrite(led1,HIGH);     // Signal LED, student has access and has card inserted
    Serial.print("\nCard reader signal detected\n");
    delay(5); // short delay for next limit switch check
    
    while(!digitalRead(limitSwitchIn)) { // While card inserted
      
      if(!digitalRead(enableButton)) { // If enable pressed
        digitalWrite(led2,HIGH);
        Serial.print("\nEnable button pressed\n");

        Serial.print("\nWriting to Pi");
        machineState = 1;
        //radio.write(&machineState,sizeof(message));
        
        delay(500);
      }
      
      else if(!digitalRead(disableButton)) { // If disable pressed
        digitalWrite(led2,LOW);
        Serial.print("\nDisable button pressed\n");

        Serial.print("\nWriting to Pi");
        machineState = 0;
        //radio.write(&machineState,sizeof(message));
        
        delay(500);
      }
      
    }

  }
  
  else {
    digitalWrite(led1,LOW);
    digitalWrite(led2,LOW);
    //Serial.print("\n...nothin' detected...\n");
  }

  // if student removed card while machine enabled
  if(machineState) {
    count = 0;

    // while card not inserted 
    while(digitalRead(limitSwitchIn) && (count < 15)) {
      count++;
      Serial.print("\nCount:\t");
      Serial.print(count);

      tone(speaker, 2000, 250); // play 1000Hz tone for 250ms
      delay(1500); // wait for 1.5 seconds
    }

    // if max count reached w/o inserting card
    if(count == 15) {
      machineState = 0; 
      digitalWrite(led2,LOW);
      Serial.print("\nTurning off machine\n");

      //radio.write(&machineState,sizeof(message));
    }
  }

  //delay(500);
  
}
