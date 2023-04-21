#pragma once

#include <Arduino.h>
#include "FastIO.h"

class Motor {
  public:
    Motor(uint8_t npin_f,uint8_t npin_b) {
      pin_f=npin_f;
      pin_b=npin_b;
      pinMode(pin_f, OUTPUT);
      pinMode(pin_b, OUTPUT);
    }
    void setSpeed(uint16_t nspeed) {
      speed = nspeed;
      if (speed > 0){
       fastWrite(pin_f, HIGH);
       fastWrite(pin_b, LOW);
      }else if(speed<0){
       fastWrite(pin_f, LOW);
       fastWrite(pin_b, HIGH);
      }else{
        fastWrite(pin_f, LOW); 
       fastWrite(pin_b, LOW);
      }
    }
    int16_t getSpeed() {
      return speed;
    }
    
  private:
    uint8_t pin_f = 0;
    uint8_t pin_b = 0;
    volatile int16_t speed = 0;
};
