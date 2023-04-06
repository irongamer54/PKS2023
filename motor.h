#pragma once

#include <Arduino.h>
#include "FastIO.h"

#define PWM_DEPTH 100

class Motor {
  public:
    Motor(uint8_t npin_f,uint8_t npin_b) {
      pin_f=npin_f;
      pin_b=npin_b;
      pinMode(pin_f, OUTPUT);
      pinMode(pin_b, OUTPUT);
    }
    void setSpeed(uint16_t nspeed) {
      speed = nspeed; //дописать аппаратный шим
    }
    int16_t getSpeed() {
      return speed;
    }
    void newTick(){
      static volatile uint8_t counter = 0;  // Счетчик

      if (counter > PWM_DEPTH) {
        // Переполнение счетчика - все каналы ШИМ устанавливаются в HIGH
        if (speed > 0) fastWrite(pin_f, HIGH); // Устанавливаем HIGH, только если заполнение >0 (быстрый digitalWrite)
        if (speed < 0) fastWrite(pin_b, HIGH); // Решает проблему слабого свечения LED при заполнении = 0
        counter = 0;                // Обнуляем счетчик ВРУЧНУЮ
      }
      if(speed>=0){
        if (counter == abs (speed)) fastWrite(pin_f, LOW); // Проверяем все каналы на совпадение счетчика со значением заполнения
      }else{
        if (counter == abs (speed)) fastWrite(pin_b, LOW); // При совпадении переводим канал в состояние LOW
      }
      counter++;
    }
    
  private:
    uint8_t pin_f = 0;
    uint8_t pin_b = 0;
    volatile int16_t speed = 0;
};
