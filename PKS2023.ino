//основной код (он будет тут (когда нибуть(но это не точно)))
#include <Wire.h>
#include <OneWire.h>

#include <TimeLib.h>
#include "timer.h" //библиотека таймера
#include "SunPosition.h"//библиотека для определения положения солнца

#include <MS5611.h>//библиотека для работы с ms5611 барометром https://www.yourduino.ru/blogs/blog/GY63urok
#include <Adafruit_MLX90614.h> //библиотека для работы с MLX90614 ИК датчиком температуры
#include <DallasTemperature.h>//библиотека для работы с ds18b20 датчиком температуры
#include <Adafruit_ADS1015.h>//библиотека для работы с ADS1115 АЦП 

#define ONE_WIRE_BUS 5
#define COUNT_FLTR 10

SunPosition pos;

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature ds_sensors(&oneWire);
Adafruit_MLX90614 mlx = Adafruit_MLX90614();
Adafruit_ADS1115 ads;

//Timer rndTimer(2000); пример иниц таймера

/*
  Барометр
  Магнитометр
  фотодиоды
  термометры
  тд
*/


void flt_ads(uint8_t pin=0){
  static Timer tmr;
  static int16_t sum[4] = {0, 0, 0, 0};
  static uint8_t count = 0;
  static int16_t last_zn[4] = {0, 0, 0, 0};
  if (tmr.ready()){
    for(uint8_t i = 0; i < 4; i++){
      sum[i]+=ads.readADC_SingleEnded(i);
    }
    count++;
  } 
  if (count>=COUNT_FLTR-1){
    for(uint8_t i = 0; i < 4; i++){
      last_zn[i]=sum[i]/count;
    }
    count=0;
  }
  return last_zn[pin];
}

void setup() {
  Serial.begin(9600);

  for (uint8_t c = 0; c < 50; c++){
    if (mlx.begin()) break;
    delay(200);
  } 

  for (uint8_t c = 0; c < 50; c++){
    if (ms5611.begin()) break;
    delay(200);
  }

  ds_sensors.getDeviceCount()

  ads.setGain(GAIN_TWOTHIRDS);
  for (uint8_t c = 0; c < 50; c++){
    if (ads.begin()) break;
    delay(200);
  }

}

void loop() {
  flt_ads();
}
