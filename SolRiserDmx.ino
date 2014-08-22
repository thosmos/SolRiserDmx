/*

SolRiser Burning Man art car lighting control

written by:
Thomas Spellman <thos37@gmail.com>

*/

#include <DmxMaster.h>
#include "colors.h"

#define BLINK_INTERVAL 500
#define DMX_TIMEOUT 1000
#define MAX_ADDRESS 93
#define NUM_CHANNELS 31
#define MAX_PATTERN 2

int ledPin = 13;
unsigned long time = 0;
unsigned long lastBlink = 0;
unsigned long lastDmx = 0;
boolean blinkState = false;
byte readIn = 0;
int idx = 0;
int i = 0;
int j = 0; 
int currentPattern = 2;

RgbColor lastRgb;

HsvColor channels[NUM_CHANNELS];

// assumes a zero indexed channel #
void writeRgb(int channel, RgbColor rgb){
    DmxMaster.write(channel * 3 + 1,rgb.r * 255);
    DmxMaster.write(channel * 3 + 2,rgb.g * 255);
    DmxMaster.write(channel * 3 + 3,rgb.b * 255);
}

void writeHsv(int channel, HsvColor hsv){
    RgbColor rgb = HsvToRgb(hsv);
    writeRgb(channel, rgb);
}

void setup() {
  // put your setup code here, to run once:
  DmxMaster.maxChannel(MAX_ADDRESS);
  pinMode(ledPin, OUTPUT);
  
  for(int i = 0; i < NUM_CHANNELS; i++){
    channels[i] = {i*(360.0/NUM_CHANNELS),1.0,1.0};
    writeHsv(i, channels[i]);
  }
  
  Serial.begin(115200);
  Serial1.begin(115200); // Set the baud.
  
}

double H = 0.00;

void loop() {

  time = millis();

  // if there's DMX coming from python on either the host or linux ports, use that
  
  if (Serial.available()>0){ // check for DMX data from host
  
    //update the timestamp to test for a timeout
    lastDmx = time;
    
    // read a byte of data from the USB and write it to the DMX network, incrementing the DMX address counter in the process
    DmxMaster.write(idx++,Serial.read());
    
    // in case we're streaming random amounts of data, beging at the beginning again
    if(idx > MAX_ADDRESS)
      idx = 1;
      
  }
  else if (Serial1.available()>0){ //check for DMX data from linux

    //update the timestamp to test for a timeout
    lastDmx = time;
    
    // read a byte of data from the USB and write it to the DMX network, incrementing the DMX address counter in the process
    DmxMaster.write(idx++,Serial1.read());
    
    // in case we're streaming random amounts of data, beging at the beginning again
    if(idx > MAX_ADDRESS)
      idx = 1;
      
  }
  else {  
    
    // if no DMX data, then continue the loop
    // between each batch of DMX data from host or linux, this will run

    // reset the DMX data address counter    
    idx = 1;

    // test to see if the external DMX sources have timed out
    // if so, then run the default pattern(s)
    if(time - lastDmx > DMX_TIMEOUT){
      if(currentPattern == 1){
        pattern1();
      } else if (currentPattern == 2){
        pattern2();
      } else {
        pattern1();
      }
    }  
    
    // do the LED13 blink
    blink();
  
  }
  
} // loop()

void pattern2(){
  
    // set channels 25 - 27 && 30 to white (railings and stairs)
    if( 24 < j && j < 28 || j == 30 ){
      //writeRgb(j * 3 + 1, {,200,200});
      writeHsv(j, {0.0, 0.0, 0.8});
      j++;
      return;
    }
  
    // write the value in the channels array
    writeHsv(j, channels[j]);
    // increment hue by one
    channels[j].h += 1.0;
    //test for reset
    if(channels[j].h >= 360.0)
      channels[j].h = 0.0;
    j++;

    //startover  
    if(j >= NUM_CHANNELS){
      j = 0;
      return;
    }
    
    //delay(1);
}

void pattern1(){
    if(j < 1)
      j = 1;
      
    DmxMaster.write(j, i++);
    if(i == 256){
      i = 0;
      DmxMaster.write(j, 0);
      j++;
      
      if(j > MAX_ADDRESS){
        j = 1;
        //nextPattern();
        return;
      }
    }
    //delay(1);
}

void blink(){

  if(time - lastBlink > BLINK_INTERVAL){
    
    lastBlink = time;
    
    if(blinkState)
      digitalWrite(ledPin, LOW);
    else
      digitalWrite(ledPin, HIGH);
      
    blinkState = !blinkState;
    
  }
  
}

void nextPattern(){
  j = 0;
  currentPattern++;
  if(currentPattern > MAX_PATTERN)
    currentPattern = 1;
}

