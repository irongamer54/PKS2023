//основной код (он будет тут (когда нибуть(но это не точно)))
#include <Wire.h>
#include <OneWire.h>
#include <Servo.h>
#include "motor.h"

#include <GyverTimers.h>// прерывания 

#include "mString.h"// библиотека быстрого String автор кода ленивый
#include <GParser.h>//парсинг Serial
#include <AsyncStream.h>

#include <TimeLib.h>
#include "timer.h" //библиотека таймера
#include "SunPosition.h"//библиотека для определения положения солнца

#include <MS5611.h>//библиотека для работы с ms5611 барометром https://www.yourduino.ru/blogs/blog/GY63urok
#include <Adafruit_MLX90614.h> //библиотека для работы с MLX90614 ИК датчиком температуры
#include <DallasTemperature.h>//библиотека для работы с ds18b20 датчиком температуры
#include <Adafruit_ADS1X15.h>//библиотека для работы с ADS1115 АЦП 

#include "config.h"

//using namespace IntroSatLib; // интросас https://github.com/Goldfor/IntroSatLib

SunPosition pos;

AsyncStream<100> serial(&Serial, ';');

OneWire oneWire(ONE_WIRE_BUS);

DallasTemperature ds_sensors(&oneWire);
DeviceAddress *dsUnique;

Adafruit_MLX90614 mlx = Adafruit_MLX90614();
Adafruit_ADS1115 ads;

MS5611 ms5611;

Servo otr_srv;
Servo angl_srv;

Motor mtr1(MTR_F_1,MTR_B_1);//переименовать)
Motor mtr2(MTR_F_2,MTR_B_2);//переименовать)

float cords[2] = {0,0};

uint32_t unix = 0; //поставить дату запуска

uint8_t mode = 0;

int16_t speeds[2] = {0,0};

int16_t srv_angle[2]={0,0}; //поставить начальные углы

uint8_t trn_speed[3]={0,0,0};

//Timer dsUpdateTmr(2000);

/*
  Барометр
  Магнитометр
  фотодиоды
  термометры
  тд
*/

void printAddress(DeviceAddress deviceAddress){ // функция вывода адресов ds18b20
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

void dsGetTemp(){//хаха, я оставлю функцию, просто потому-что могу 
  static Timer tmr(DS_UPDATE_TIME);
  if (tmr.ready()) ds_sensors.requestTemperatures();
}

int16_t flt_ads(uint8_t pin=0){ // функция фильтрации значений с ацп 
  static Timer tmr(ADS_UPDATE_TIME/COUNT_FLTR); //А вот эту функцию трогать не буду)))
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
        sum[i]=0;
      }
      count=1;
    }
  } 
  return last_zn[pin];
}

struct Str {
  float temp[6];
  float tempIK;
  int16_t alfa;
  int16_t azim;
  float prs;
  int16_t speed_c;
  int16_t angle[2];
  int16_t speed_m[2];
  uint8_t mos[3];
  int16_t ads[4];
  byte crc;
};

void SendData(){ //функция отправки данных
  Str buf;
  mString<50> dataStr;
  
  dataStr+="n,t,"
  for(uint8_t indx = 0; i < 6; i++){
    buf.temp[indx]=ds_sensors.getTempCByIndex(indx);
    dataStr+=buf.temp[indx];
  }

  dataStr+="i,"
  buf.tempIK=ds_sensors.getTempCByIndex(indx);
  dataStr+=buf.tempIK;

  dataStr+=",b,"
  buf.alfa=pos.altitude(indx);

  // Сказать Лере дописать эту часть кода (дозаполнить структуру)

  byte crc = crc8((byte*)&buf, sizeof(buf) - 1);
  buf.crc = crc;

  Serial.write((byte*)&buf, sizeof(buf))
}

void Parser(){  //парсинг Serial переделать
  GParser data(serial.buf, ',');
  uint8_t dt_len=data.split();
  if (data[0]=='f'){
    for(uint8_t i=1;i<dt_len;i++){
      char sim = data[i];
      switch (sim)
      {
      case 'g':
        cords[0]=data.getFloat(i+1);
        cords[1]=data.getFloat(i+2);
        i+=2;
        break;
      case 't':
        unix=data.getInt(i+1);
        i+=1;
        break;
      case 'm':
        mode=data.getInt(i+1);
        i+=1;
        break;
      case 's':
        speeds[0]=data.getInt(i+1);
        speeds[1]=data.getInt(i+2);
        i+=2;
        break;
      case 'a':
        srv_angle[0]=data.getInt(i+1);
        srv_angle[1]=data.getInt(i+2);
        i+=2;
        break;
      case 'r':
        trn_speed[0]=data.getInt(i+1);
        trn_speed[1]=data.getInt(i+2);
        trn_speed[2]=data.getInt(i+3);
        i+=3;
        break;     
      }
    }
  }
}

byte crc8(byte *buffer, byte size) { // функция вычисления crc
  byte crc = 0;
  for (byte i = 0; i < size; i++) {
    byte data = buffer[i];
    for (int j = 8; j > 0; j--) {
      crc = ((crc ^ data) & 1) ? (crc >> 1) ^ 0x8C : (crc >> 1);
      data >>= 1;
    }
  }
  return crc;
}

int16_t  Centri_speed(){
  static uint32_t last_time=0 // дописать Лере
}

void setup() {
  Serial.begin(SERIAL_SPEED);

  Wire.begin(); 

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

  otr_srv.attach(SRV_PIN_1);
  angl_srv.attach(SRV_PIN_2);

  Timer0.setFrequency(40000);    
  Timer0.enableISR();            
}

void loop() {
  flt_ads();
  dsGetTemp();
  if (serial.available()) { 
    Parser();
  }
}

ISR(TIMER2_A) {
  mtr1.newTick();//переименовать)
  mtr2.newTick();//переименовать)
  // функцию для скорости центрифуги сюда Лере
}
