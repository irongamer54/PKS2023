#pragma once

#include <Arduino.h>

class Centifuge {// функция для вычисления скорости врашения камеры
    public:
        Centifuge(uint8_t npin, uint8_t ncount=1) {
            pin=npin;
            count_mag=ncount;
        }

        int16_t  getSpeed(){
            if(delta_time!=0){
                speed=2/(delta_time*0.001)
            }
            return speed
        }

        void newTick(){ //
            if (digitalRead(pin)){
                if(state==0){
                    delta_time=millis()-last_time;
                    last_time=millis();
                    state=1;
                }
            } else state=0;
        }

    private:
        uint8_t pin;
        uint8_t count_mag=1;

        bool state=0;
        uint16_t delta_time=1;
        uint32_t last_time=0;

        float speed=0;
}
/*
//начало кода на частоту
  uint32_t t1 = millis();

  int mgn = digitalRead(M);
  while (mgn == 1) {
    mgn = digitalRead(M);
  }
  

  uint32_t t2 = millis();
  double v;
  if (t2-t1!=0) {
   v = 2/((t2-t1)*0.001);

  }
  
  /*Serial.print("millis =");
  Serial.println(millis());

  Serial.print("mgn = ");
  Serial.println(mgn);
  
  
  Serial.print("v = ");
  Serial.println(v);

  
  delay(10);
  //конец кода на частоту
  */