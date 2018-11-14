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

// Card reader connections: 
// RED:   N/O
// WHITE: N/C
// BLACK: COM

// Initialize Radio
RF24 radio(9,53);

String machine_name = "Machine1"	// Machine name
int machineState = 0;		// 1:machine enabled, 0:machine disabled
int count = 0; 						// used when card removed while machine enabled

struct PAY_LOAD {
	char name[];	//machine
	int state;	//state (on/off)(1/0)
};

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
    digitalWrite(led3,LOW);
    Serial.print("\nCard reader signal detected\n");
    delay(5); // short delay for next limit switch check
    
    while(!digitalRead(limitSwitchIn)) { // While card inserted
      
      if(!digitalRead(enableButton)) { // If enable pressed
        digitalWrite(relayOut,HIGH);
        digitalWrite(led2,HIGH);
        Serial.print("\nEnable button pressed\n");

        Serial.print("\nWriting to Pi");
        machineState = 1;
        
        if(sendPayLoad(machineState)){
        	//sent good
          Serial.print("sent payload");
        }
        else
        {
        	//failed
        }
        
        //radio.write(message, message_size);
        
        delay(500);
      }
      
      else if(!digitalRead(disableButton)) { // If disable pressed
        digitalWrite(relayOut,LOW);
        digitalWrite(led2,LOW);
        Serial.print("\nDisable button pressed\n");

        Serial.print("\nWriting to Pi");
        machineState = 0;
		
		
		if(sendPayLoad(machineState)){
      //sent good
      Serial.print("sent payload");
    }
    else
    {
      //failed
    }
        
    delay(500);
    }
      
  }

}
  
  else {
    digitalWrite(relayOut,LOW);
    digitalWrite(led1,LOW);
    digitalWrite(led2,LOW);
    digitalWrite(led3,LOW);

    machineState = 0;
    //Serial.print("\n...nothin' detected...\n");
  }

  // if student removed card while machine enabled
  if(machineState) {
    count = 0;
    digitalWrite(led3,HIGH);

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
      	digitalWrite(relayOut,LOW);
      	digitalWrite(led2,LOW);
      	digitalWrite(led3,LOW);
      	Serial.print("\nTurning off machine\n");
      
      	if(sendPayLoad(machineState)){
        	//sent good
          Serial.print("sent payload");
        }
        else
        {
        	//failed
        }
        
    	//radio.write(message, message_size);
    }
  }

  //delay(500);
  
}

boolean sendPayLoad(boolean val)
{
	// Create struct
	PAY_LOAD data;
	data.name = machine_name
	data.state = val
	
  return(radio.write(data, sizeof(PAY_LOAD)));
}