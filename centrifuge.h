#pragma once

#include <Arduino.h>

class Centrifuge {// функция для вычисления скорости врашения камеры
  public:
    Centrifuge(uint8_t npin, uint8_t ncount = 1){
      pin = npin;
      count_mag = ncount;
      pinMode(pin, OUTPUT);
    }

    int16_t  getSpeed() {
      if (delta_time != 0) {
        speed = 2 / (delta_time * 0.001);
      }
      return speed;
    }

    void newTick() {
      if (digitalRead(pin)) {
        if (state == 0) {
          delta_time = millis() - last_time;
          last_time = millis();
          state = 1;
        }
      } else{
        state = 0;
      }
    }

  private:
    uint8_t pin=0;
    uint8_t count_mag = 1;

    bool state = 0;
    uint16_t delta_time = 1;
    uint32_t last_time = 0;
    
    float speed = 0;
};
