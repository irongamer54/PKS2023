//основной код (он будет тут (когда нибуть(но это не точно)))
#include <Wire.h>
#include <OneWire.h>

#include <GyverTimers.h> // прерывания 

//#include "mString.h"// библиотека быстрого String автор кода ленивый
//#include <GParser.h>//парсинг Serial
//#include <AsyncStream.h>

#include <TimeLib.h>
#include "timer.h" //библиотека таймера
//#include "SunPosition.h"//библиотека для определения положения солнца

#include <MS5611.h>//библиотека для работы с ms5611 барометром https://www.yourduino.ru/blogs/blog/GY63urok
#include <Adafruit_MLX90614.h> //библиотека для работы с MLX90614 ИК датчиком температуры
#include <DallasTemperature.h>//библиотека для работы с ds18b20 датчиком температуры
#include <Adafruit_ADS1X15.h>//библиотека для работы с ADS1115 АЦП 

#include <Servo.h>

#include "motor.h"
#include "centrifuge.h"
#include "config.h"
//using namespace IntroSatLib;  интросас https://github.com/Goldfor/IntroSatLib https://github.com/stm32duino/BoardManagerFiles/raw/main/package_stmicroelectronics_index.json

//SunPosition pos ;

//AsyncStream<100> serial(&Serial, ';');

OneWire oneWire(10);

DallasTemperature ds_sensors(&oneWire);
DeviceAddress *dsUnique;

Adafruit_MLX90614 mlx = Adafruit_MLX90614();
Adafruit_ADS1115 ads;

MS5611 ms5611;

Servo otr_srv;

Motor cam_mtr(MTR_F_PIN, MTR_B_PIN); //переименовать)
//Centrifuge centri(MAGN_PIN, COUNT_MAG); //пример

uint8_t mode = 0;

float tang = 0;

double referencePressure;//давление

//Timer dsUpdateTmr(2000);

/*
  Барометр
  Магнитометр
  фотодиоды
  термометры
  тд
*/

void printAddress(DeviceAddress deviceAddress) { // функция вывода адресов ds18b20
  for (uint8_t i = 0; i < 8; i++) {
    if (deviceAddress[i] < 16) Serial.print("0");
    Serial.print(deviceAddress[i], HEX);
  }
}

uint8_t DSInit(bool is_init = 0) { //функция инициализации ds18b20
  delay(1000);
  uint8_t ds_count = ds_sensors.getDeviceCount();
  if(SERIAL_DBG_MODE)Serial.println(ds_count);
  if (!is_init) {
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

void dsGetTemp() { //хаха, я оставлю функцию, просто потому-что могу
  static Timer tmr(DS_UPDATE_TIME);
  if (tmr.ready()) ds_sensors.requestTemperatures();
}

int16_t flt_ads(uint8_t pin = 0) { // функция фильтрации значений с ацп

  static Timer tmr(ADS_UPDATE_TIME / COUNT_FLTR); //А вот эту функцию трогать не буду)))
  static int16_t sum[4] = {0, 0, 0, 0};
  static uint8_t count = 1;
  static int16_t last_zn[4] = {0, 0, 0, 0};
  if (tmr.ready()) {
    for (uint8_t i = 0; i < 4; i++) {
      sum[i] += ads.readADC_SingleEnded(i);
    }
    count++;
    if (count >= COUNT_FLTR) {
      for (uint8_t i = 0; i < 4; i++) {
        last_zn[i] = sum[i] / (count);
        sum[i] = 0;
      }
      count = 1;
    }
  }
  return last_zn[pin];
}

float azim(){
  int32_t x=flt_ads(0)-flt_ads(3);
  int32_t y=flt_ads(1)-flt_ads(2);
  return atan2(x,y);
}

#pragma pack(push, 1)
struct Send_data {
  float temp[6];
  uint16_t f_res[4];
  uint8_t mode;
  double prs;
  double alt;
  int16_t srv_angle;
  float azim;
  int16_t speed_m;
  byte crc;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct Read_data {
  uint8_t mode;
  float tang;
  byte crc;
};
#pragma pack(pop)

void SendData() { //функция отправки данных
  static Timer tmr(SEND_DATA_DELAY);
  static Send_data buf;

  if (tmr.ready()) {
    if (SERIAL_DBG_MODE) Serial.println(String("////////////////// PACKET START //////////////////\nMillis: "+String(millis())));

    for (uint8_t indx = 0; indx < 6; indx++) {
      buf.temp[indx] = ds_sensors.getTempCByIndex(indx);
      if (SERIAL_DBG_MODE) Serial.println(String("Temp " + String(indx) + ": " + String(buf.temp[indx])));
    }

    for (uint8_t indx = 0; indx < 4; indx++) {
      buf.f_res[indx] = flt_ads(indx);
      if (SERIAL_DBG_MODE) Serial.println(String("Foto " + String(indx) + ": " + String(buf.f_res[indx])));
    }

    buf.mode = mode;
  
    buf.prs = ms5611.readPressure();
    buf.alt = ms5611.getAltitude(buf.prs);

    buf.srv_angle = otr_srv.read();

    buf.azim=azim();

    buf.speed_m = cam_mtr.getSpeed();

    byte crc = crc8((byte*)&buf, sizeof(buf) - 1);
    buf.crc = crc;
    if (!SERIAL_DBG_MODE)
        Serial.write((byte*)&buf, sizeof(buf));
    if (SERIAL_DBG_MODE){
      Serial.println(String("Mode: " + String(buf.mode)));
      Serial.println(String("Pressure: " + String(buf.prs)));
      Serial.println(String("Altitude: " + String(buf.alt)));
      Serial.println(String("Srv angle: " + String(buf.srv_angle)));
      Serial.println(String("Azim: " + String(buf.azim)));
      Serial.println(String("Mtr speed: " + String(buf.speed_m)));
      Serial.println(String("CRC: " + String(buf.crc)));
      Serial.println("////////////////// PACKET END //////////////////\n");
    }
  }
}

void Parser() { //парсинг Serial переделать
  static Read_data buf;

  if (Serial.readBytes((byte*)&buf, sizeof(buf))) {

    byte crc = crc8((byte*)&buf, sizeof(buf));

    if (crc == 0) {
      //mode = buf.mode;
      tang = buf.tang;
      //speed_m = buf.speed_m;
    } else {
      //запросить повтор пакета
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

void sun_orient(){
  //otr_srv.write(sun.altitude()-tang);
  // прописать пид регулятор для наводки на солнце
}

void pinSetup() {
  pinMode(SRV_PIN, OUTPUT);

  pinMode(MTR_F_PIN, OUTPUT);
  pinMode(MTR_B_PIN, OUTPUT);

 // pinMode(LED_PIN, OUTPUT);
}

void standby() {
  otr_srv.write(START_OTR_ANGL);
}

void self_mode() {

}

void hand_mode() {
  Serial.print("ПРописать ");// нужно поработать ручками

}

////////////////////////////    SETUP    ////////////////////////////
void setup() {
  Serial.begin(SERIAL_SPEED);
  delay(200);
  Wire.begin();
  delay(200);

  for (uint8_t c = 0; c < 50; c++) {
    if (ms5611.begin()) break;
    if (SERIAL_DBG_MODE) Serial.println("MS NOT START");
    delay(200);
  }

  DSInit();
  
  delay(500);

  ads.setGain(GAIN_TWOTHIRDS);
  for (uint8_t c = 0; c < 50; c++) {
    if (ads.begin()) break;
    if (SERIAL_DBG_MODE) Serial.println("ADS NOT START");
    delay(200);
  }

  pinSetup();
  delay(500);
  otr_srv.attach(SRV_PIN);
  delay(500);

  if (SERIAL_DBG_MODE) Serial.println("Start");
}


////////////////////////////    LOOP    ////////////////////////////
void loop() {

  flt_ads();
  dsGetTemp();
  Parser();
  SendData();

  double prs = ms5611.readPressure();
  double alt =  ms5611.getAltitude(prs);
  if(alt>10000){
    mode=1;
  }else{
    mode=0;
  }
  switch (mode)
  {
    case 0:
      /* code*/
     // standby();
      break;
    case 1:
      self_mode();
      break;
    case 2:
     // hand_mode();
      break;
    default:
      break;
  }
  //centri.newTick();
}
