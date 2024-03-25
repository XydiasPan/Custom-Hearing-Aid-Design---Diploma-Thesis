#include <math.h>
#include "LowPower.h"


const float Vbias = 0.9;
int countSamplingTime = 0; // crystall works on 16MHz and 44100 is typical audio sampling time so 16MHz/44100 = 363 loops approximately
int countBiasCrossings = 0;
float fundamentalF = 0;
float sigIn = 0.0;
int countWorkingTime = 0;

void setup() {
  // initialize I/O
  //Program Interrupt and Analog Input
  pinMode(PD2,INPUT);
  pinMode(PC2,INPUT);
  //Program BPF1
  pinMode(PB3,OUTPUT);
  pinMode(PB4,OUTPUT);
  pinMode(PB5,OUTPUT); 
  //Program BPF2
  pinMode(PB0,OUTPUT);
  pinMode(PB1,OUTPUT);
  pinMode(PB2,OUTPUT);
 //Program BPF3
  pinMode(PD5,OUTPUT);
  pinMode(PD6,OUTPUT);
  pinMode(PD7,OUTPUT);
  //Program BPF3
  pinMode(PC0,OUTPUT);
  pinMode(PD3,OUTPUT);
  pinMode(PD4,OUTPUT);

  // initialize gains for BPFs' signals according to audiogram
  digitalWrite(PB3,HIGH);
  digitalWrite(PB4,LOW);
  digitalWrite(PB5,HIGH);
  
  digitalWrite(PB0,HIGH);
  digitalWrite(PB1,LOW);
  digitalWrite(PB2,HIGH);
  
  digitalWrite(PD5,HIGH);
  digitalWrite(PD6,LOW);
  digitalWrite(PD7,HIGH);
  
  digitalWrite(PC0,HIGH);
  digitalWrite(PD3,LOW);
  digitalWrite(PD4,HIGH);
 
  // attach interrupt on pin PD2 to wake up
  attachInterrupt(digitalPinToInterrupt(PD2),wakeUpCallback,LOW);
}

void loop() {
  float voltageIn = (analogRead(PC2))*(5/1023.0);
  countSamplingTime ++; 
  if(countSamplingTime >= 363){
    fundamentalF = (countBiasCrossings/363.0)*22050.0;
    countBiasCrossings = 0;
    countSamplingTime = 0;
    if(fundamentalF <= 500 && abs(voltageIn-Vbias) >= 0.5) LowerAudio(0);
    else if(fundamentalF <= 500 && abs(voltageIn-Vbias) <= 0.1) UpperAudio(0);
    else if(fundamentalF > 500 && fundamentalF <= 1000 && abs(voltageIn-Vbias) >= 1) LowerAudio(1);
    else if(fundamentalF > 500 && fundamentalF <= 1000 && abs(voltageIn-Vbias) <= 0.1) UpperAudio(1);
    else if(fundamentalF > 1000 && fundamentalF <= 4000 && abs(voltageIn-Vbias) >= 1) LowerAudio(2);
    else if(fundamentalF > 1000 && fundamentalF <= 4000 && abs(voltageIn-Vbias) <= 0.1) UpperAudio(2);
    else if(fundamentalF > 4000 && abs(voltageIn-Vbias) >= 0.5) LowerAudio(3);
    else if(fundamentalF > 4000 && abs(voltageIn-Vbias)) UpperAudio(3);
  }
  if((voltageIn - Vbias)>= 0.2 && (sigIn-Vbias) <= 0.2 || (sigIn - Vbias)>= 0.2 && (voltageIn-Vbias) <= 0.2){
    countBiasCrossings ++;
  }
  // check if module is working without reason
  // let signal threshold be +- 0.2 V
  if( abs(voltageIn - sigIn) <= 0.2) {
      countWorkingTime++;
  }
  else if (countWorkingTime >= 500 && abs(voltageIn - sigIn) > 0.2){
    countWorkingTime = 0; // reset timer in case there is a speech ongoing 
  }
  sigIn = voltageIn;
  if(countWorkingTime >= 10000){
    // enter low power mode and wake up when digital pin 3 gets HIGH value
    LowPower.powerDown(SLEEP_FOREVER,ADC_OFF,BOD_OFF);
  }
}

void wakeUpCallback(){
  countWorkingTime = 0;
   attachInterrupt(digitalPinToInterrupt(PD2),wakeUpCallback,LOW);
}

void LowerAudio(int filter){
  if(filter == 0){
    // decrease volume in BPF1
    digitalWrite(PB3,HIGH);
    digitalWrite(PB4,HIGH);
    digitalWrite(PB5,LOW);
  }
  else if (filter == 1){
    // decrease volume in BPF2
    digitalWrite(PB0,HIGH);
    digitalWrite(PB1,HIGH);
    digitalWrite(PB2,LOW);
  }
  else if (filter == 2){
    // decrease volume in BPF3
    digitalWrite(PD5,HIGH);
    digitalWrite(PD6,HIGH);
    digitalWrite(PD7,LOW);
  }
  else if (filter == 3){
    // decrease volume in BPF4
    digitalWrite(PC0,HIGH);
    digitalWrite(PD3,HIGH);
    digitalWrite(PD4,LOW);
  }
}

void UpperAudio(int filter){
  if(filter == 0){
    // increase volume in BPF1
    digitalWrite(PB3,LOW);
    digitalWrite(PB4,HIGH);
    digitalWrite(PB5,HIGH);
  }
  else if (filter == 1){
    // increase volume in BPF2
    digitalWrite(PB0,LOW);
    digitalWrite(PB1,HIGH);
    digitalWrite(PB2,HIGH);
  }
  else if (filter == 2){
    // increase volume in BPF3
    digitalWrite(PD5,LOW);
    digitalWrite(PD6,HIGH);
    digitalWrite(PD7,HIGH);
  }
  else if (filter == 3){
    // increase volume in BPF4
    digitalWrite(PC0,LOW);
    digitalWrite(PD3,HIGH);
    digitalWrite(PD4,HIGH);
  }
}
