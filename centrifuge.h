#pragma once
#include <Arduino.h>
class Centifuge {
    public:
        int16_t  CentriSpeed(){
            return speed;
        }

    private:
        uint32_t last_time=0;
        int16_t speed=0;
}