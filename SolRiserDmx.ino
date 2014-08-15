#include <DmxMaster.h>

#define BLINK_INTERVAL 500
#define DMX_TIMEOUT 1000
#define MAX_CHANNEL 12

int ledPin = 13;
unsigned long time = 0;
unsigned long lastBlink = 0;
unsigned long lastDmx = 0;
boolean blinkState = false;
byte readIn = 0;
int idx = 0;
int i = 0;
int j = 0; 

void setup() {
  // put your setup code here, to run once:
  DmxMaster.maxChannel(MAX_CHANNEL);
  pinMode(ledPin, OUTPUT);
  
  //delay(2500); //The bare minimum needed to be able to reboot both linino and leonardo.
                          // if reboot fails try increasing this number
                          // The more you run on linux the higher this number should be
  
  Serial.begin(115200);
  Serial1.begin(115200); // Set the baud.

//  // Wait for U-boot to finish startup.  Consume all bytes until we are done.
//  do {
//     while (Serial1.available() > 0) {
//        Serial1.read();
//     }
//     delay(1000);
//  } while (Serial1.available()>0);
  
}


void loop() {

    
//  if (Serial1.available()>0){
//    lastDmx = millis();
//    DmxMaster.write(idx++,Serial1.read());
//    if(idx > MAX_CHANNEL)
//      idx = 1;
//  }
  time = millis();

  if (Serial.available()>0){
    lastDmx = time;
    DmxMaster.write(idx++,Serial.read());
    if(idx > MAX_CHANNEL)
      idx = 1;
  }
  else {  
    idx = 1;
    
    pattern();
    blink();
  
  }
  
} // loop()

void pattern(){
  if(time - lastDmx > DMX_TIMEOUT){

    DmxMaster.write(j, i++);
    if(i == 256){
      i = 0;
      DmxMaster.write(j, 0);
      j++;
      if(j > MAX_CHANNEL)
        j = 1;
    }
    
    delay(1);
  }
}

void blink(){

  if(time - lastBlink > BLINK_INTERVAL){
    
    lastBlink = time;
    
    if(blinkState)
      digitalWrite(ledPin, LOW);
    else
      digitalWrite(ledPin, HIGH);
      
    blinkState = !blinkState;
    
    //Serial.println("blink");
    
  }
  
}
