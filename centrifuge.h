#pragma once
#include <Arduino.h>
class Centifuge {
    public:
        int16_t  CentriSpeed(){
            return speed;
        Centifuge(uint8_t npin, uint8_t ncount=1) {
            pin=npin;
            count_mag=ncount;
            pinMode(pin, INPUT);
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
        uint32_t last_time=0;
        int16_t speed=0;
}

        float speed=0;
}
