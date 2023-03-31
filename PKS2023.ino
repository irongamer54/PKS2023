//основной код (он будет тут (когда нибуть(но это не точно)))
#include <Wire.h>
#include <OneWire.h>

#include <GParser.h>//парсинг Serial
#include <AsyncStream.h>

#include <TimeLib.h>
#include "timer.h" //библиотека таймера
#include "SunPosition.h"//библиотека для определения положения солнца

#include <MS5611.h>//библиотека для работы с ms5611 барометром https://www.yourduino.ru/blogs/blog/GY63urok
#include <Adafruit_MLX90614.h> //библиотека для работы с MLX90614 ИК датчиком температуры
#include <DallasTemperature.h>//библиотека для работы с ds18b20 датчиком температуры
#include <Adafruit_ADS1X15.h>//библиотека для работы с ADS1115 АЦП 

#define ONE_WIRE_BUS 5
#define COUNT_FLTR 10

#define DS_UPDATE_TIME 2000
#define ADS_UPDATE_TIME 2000

SunPosition pos;

AsyncStream<100> serial(&Serial, ';');

OneWire oneWire(ONE_WIRE_BUS);

DallasTemperature ds_sensors(&oneWire);
DeviceAddress *dsUnique;

Adafruit_MLX90614 mlx = Adafruit_MLX90614();
Adafruit_ADS1115 ads;

MS5611 ms5611;

//Timer dsUpdateTmr(2000);

/*
  Барометр
  Магнитометр
  фотодиоды
  термометры
  тд
*/

void printAddress(DeviceAddress deviceAddress){// функция вывода адресов ds18b20
  for (uint8_t i = 0; i < 8; i++){
    if (deviceAddress[i] < 16) Serial.print("0");
    Serial.print(deviceAddress[i], HEX);
  }
}

uint8_t DSInit(bool is_init = 0){ //функция инициализации ds18b20
  uint8_t ds_count=ds_sensors.getDeviceCount();
  if(!is_init){
    dsUnique = new DeviceAddress[ds_count];
    for (int i = 0; i < ds_count; i++) {
      ds_sensors.getAddress(dsUnique[i], i);
    }
    for (int i = 0; i < ds_count; i++) {
      ds_sensors.setResolution(dsUnique[i], 12);
    }
  }
  return ds_count;
}

float dsGetTemp(uint8_t indx=0){//получение температуры с ds18b20 
  static Timer tmr(DS_UPDATE_TIME);
  static float temperature[10];
  if (tmr.ready()){
    ds_sensors.requestTemperatures();
    for (int i = 0; i < DSInit(1); i++) {
      temperature[i] = ds_sensors.getTempCByIndex(i);
    }
  }
  return temperature[indx];
}

int16_t flt_ads(uint8_t pin=0){ // функция фильтрации значений с ацп 
  static Timer tmr(ADS_UPDATE_TIME/COUNT_FLTR);
  static int16_t sum[4] = {0, 0, 0, 0};
  static uint8_t count = 1;
  static int16_t last_zn[4] = {0, 0, 0, 0};
  if (tmr.ready()){
    for(uint8_t i = 0; i < 4; i++){
      sum[i]+=ads.readADC_SingleEnded(i);
    }
    count++;
    if (count>=COUNT_FLTR){
      for(uint8_t i = 0; i < 4; i++){
        last_zn[i]=sum[i]/(count);
      }
      count=1;
    }
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

  DSInit();
  
  ads.setGain(GAIN_TWOTHIRDS);
  for (uint8_t c = 0; c < 50; c++){
    if (ads.begin()) break;
    delay(200);
  }

}

void loop() {
  flt_ads();
  dsGetTemp();

   if (serial.available()) { 
    Serial.println(serial.buf);    
  }
  
}
