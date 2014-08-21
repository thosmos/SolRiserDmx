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

void writeRgb(int channel, RgbColor rgb){
    DmxMaster.write(channel,rgb.r * 255);
    DmxMaster.write(channel + 1,rgb.g * 255);
    DmxMaster.write(channel + 2,rgb.b * 255);
}

void writeHsv(int channel, HsvColor hsv){
    RgbColor rgb = HsvToRgb(hsv);
    writeRgb(channel, rgb);
//    DmxMaster.write(channel, rgb.r);
//    DmxMaster.write(channel + 1,rgb.g);
//    DmxMaster.write(channel + 2,rgb.b);
    //Serial.print(rgb.R
}

void setup() {
  // put your setup code here, to run once:
  DmxMaster.maxChannel(NUM_CHANNELS * 3);
  pinMode(ledPin, OUTPUT);
  
  for(int i = 0; i < NUM_CHANNELS; i++){
    //HsvColor c = {i*(360.0/NUM_CHANNELS),1,1};
    channels[i] = {i*(360.0/NUM_CHANNELS),1.0,1.0};
    writeHsv(i * 3 + 1, channels[i]);
  }
  
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

double H = 0.00;

void loop() {

    
//  if (Serial1.available()>0){
//    lastDmx = millis();
//    DmxMaster.write(idx++,Serial1.read());
//    if(idx > MAX_CHANNEL)
//      idx = 1;
//  }
  time = millis();

  if (Serial.available()>0){
    Serial.read();
//    lastDmx = time;
//    DmxMaster.write(idx++,Serial.read());
//    if(idx > MAX_ADDRESS)
//      idx = 1;

    HsvColor hsv = {H++, 1.0, 1.0};
    RgbColor rgb = HsvToRgb(hsv);

    Serial.print("hsv ");
    Serial.print(hsv.h);
    Serial.print(hsv.s);
    Serial.println(hsv.v);
    Serial.print("rgb ");
    Serial.print(rgb.r);
    Serial.print(rgb.g);
    Serial.println(rgb.b);
    
  }
  else if (Serial1.available()>0){
    lastDmx = time;
    DmxMaster.write(idx++,Serial1.read());
    if(idx > MAX_ADDRESS)
      idx = 1;
  }
  else {  
    idx = 1;

  // test to see if we run the default pattern(s)
  if(time - lastDmx > DMX_TIMEOUT){
    if(currentPattern == 1){
      pattern1();
    } else if (currentPattern == 2){
      pattern2();
    } else {
      pattern1();
    }
  }  
    
   blink();
  
  }
  
} // loop()

void pattern2(){
  
//    if( 24 < j < 30 ){
//      //writeRgb(j * 3 + 1, {,200,200});
//      writeHsv(j * 3 + 1, {0.0, 0.0, 0.8});
//      j++;
//      return;
//    }
  
    writeHsv(j * 3 + 1, channels[j]);
    channels[j].h += 1.0;
    if(channels[j].h >= 360.0)
      channels[j].h = 0.0;
    j++;
  
    if(j >= NUM_CHANNELS){
      j = 0;
      //nextPattern();
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
    delay(1);
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

void nextPattern(){
  j = 0;
  currentPattern++;
  if(currentPattern > MAX_PATTERN)
    currentPattern = 1;
}

