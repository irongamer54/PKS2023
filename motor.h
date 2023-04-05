#pragma once
#include <Arduino.h>
class Motor {
  public:
    Motor(uint8_t npin_f,uint8_t npin_b) {
      pin_f=npin_f;
      pin_b=npin_b;
    }
    void setSpeed(uint16_t nspeed) {
      speed = nspeed; //дописать аппаратный шим
    }
    int16_t getSpeed() {
      return speed;
    }
  private:
    uint8_t pin_f = 0;
    uint8_t pin_b = 0;
    int16_t speed = 0;
};