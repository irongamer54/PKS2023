#pragma once
#include <Arduino.h>
class Centifuge {// функция для вычисления скорости врашения камеры
    public:
        int16_t  CentriSpeed(){
            return speed;
        }
         // дописать Лере
    private:
        uint32_t last_time=0;
        int16_t speed=0;
}