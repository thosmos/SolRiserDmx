/*

SolRiser Burning Man art car lighting control

written by:
Thomas Spellman <thos37@gmail.com>

TODO: figure out why rainbow fade seems to move backward from high channels to low

* 0.1: fixed flicker on data coming from yun's linux
* 0.2: added DmxMaster files to Arduino project to make it easier to get up and running (might need to remove it from Arduino/libraries folder)

*/

#include "DmxMaster.h"
#include "colors.h"

#define BLINK_INTERVAL 500
#define DMX_TIMEOUT 1000
#define MAX_ADDRESS 90
#define NUM_CHANNELS 30
#define MAX_PATTERN 2
#define LED_PIN 13

unsigned long time = 0;
unsigned long lastBlink = 0;
unsigned long lastDmx = 0;
boolean blinkState = false;

// DMX address counter
int idx = 0;  

// misc indexers
int i = 0;
int j = 0; 

// current pattern running
int currentPattern = 2;

// pattern specific variables
boolean pattern2initted = false;
boolean p3_init = false;

// array to store state of lights across loops
HsvColor channels[NUM_CHANNELS];

// assumes a zero indexed channel # and a float RGB value from 0.0 to 1.0
void writeRgb(int channel, RgbColor rgb){
    DmxMaster.write(channel * 3 + 1, rgb.r * 255);
    DmxMaster.write(channel * 3 + 2, rgb.g * 255);
    DmxMaster.write(channel * 3 + 3, rgb.b * 255);
}

void writeHsv(int channel, HsvColor hsv){
    RgbColor rgb = HsvToRgb(hsv);
    writeRgb(channel, rgb);
}

void setup() {
  DmxMaster.maxChannel(MAX_ADDRESS);
  pinMode(LED_PIN, OUTPUT);

  Serial.begin(115200);
  Serial1.begin(115200); // Set the baud.
  
}

void loop() {
  
  /* 
     It's critically important to do a minimum amount of work per loop, and let it come around again.  
     Holding the loop for too long, such as with an inner while loop, will cause the DMX output to get faily.
  */

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

    if(time - lastDmx > 5){
      // if it's been a little while (but well less than one 40hz frame or 25ms)
      // reset the DMX data address counter    
      // this fixes some flicker problems caused by the serial.available() returning false in the middle of a serial transfer 
      idx = 1;
    }

    // test to see if the external DMX sources have timed out
    // if so, then run the default pattern(s)
    if(time - lastDmx > DMX_TIMEOUT){
      if(currentPattern == 1){
        pattern1();
      } else if (currentPattern == 2){
        pattern2();
      } else if (currentPattern == 3){
        pattern3();
      } else {
        pattern1();
      }
    }  
    
    // do the LED13 blink
    blink();
  
  }
  
} // loop()

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

void pattern2(){
  
    if(!pattern2initted){
      // setup the initial rainbow  
      for(int i = NUM_CHANNELS - 1; i >= 0; i--){
        channels[i] = {i*(60.0/NUM_CHANNELS),1.0,1.0};
        writeHsv(i, channels[i]);
      }
      pattern2initted = true;
    }
  
    // set channels 25 - 27 && 30 to white (railings and stairs)
    // TODO change the stairs DMX address to 28 to make this smoother for coding
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
    
    delay(1);
}

void pattern3(){
  
    if(!p3_init){
      // setup the initial rainbow  
//      for(int i = NUM_CHANNELS - 1; i >= 0; i--){
//        channels[i] = {i*(60.0/NUM_CHANNELS),1.0,1.0};
//        writeHsv(i, channels[i]);
//      }
//      j = NUM_CHANNELS - 1;
      for(int i = 0 - 1; i < NUM_CHANNELS; i++){
        channels[i] = {i*(60.0/NUM_CHANNELS),1.0,1.0};
        writeHsv(i, channels[i]);
      }
      p3_init = true;
    }
  
//    // set channels 25 - 27 && 30 to white (railings and stairs)
//    // TODO change the stairs DMX address to 28 to make this smoother for coding
//    if( 24 < j && j < 28 || j == 30 ){
//      //writeRgb(j * 3 + 1, {,200,200});
//      writeHsv(j, {0.0, 0.0, 0.8});
//      j--;
//      return;
//    }
  
    // write the value in the channels array
    writeHsv(j, channels[j]);
    // increment hue by one
    channels[j].h += 1.0;
    //test for reset
    if(channels[j].h >= 360.0)
      channels[j].h = 0.0;
    j--;

    //startover  
    if(j < 0){
      j = NUM_CHANNELS - 1;
      return;
    }
    
    delay(1);
}

void blink(){

  if(time - lastBlink > BLINK_INTERVAL){
    
    lastBlink = time;
    
    if(blinkState)
      digitalWrite(LED_PIN, LOW);
    else
      digitalWrite(LED_PIN, HIGH);
      
    blinkState = !blinkState;
    
  }
  
}

void nextPattern(){
  j = 0;
  currentPattern++;
  if(currentPattern > MAX_PATTERN)
    currentPattern = 1;
}

