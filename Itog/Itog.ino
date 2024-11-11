
#include <OneWire.h>
#include <microDS18B20.h>//алт библиотека для работы с ds18b20 датчиком температуры
#include <MPU6050.h>

#include <MS5611.h>

#include <Servo.h>

#include "timer.h"

MPU6050 mpu;
MS5611 ms5611;

MicroDS18B20<10, DS_ADDR_MODE, 6> ds18;

Servo otr_srv;

uint8_t mode = 0;
#define LORA_DBG_MODE 0
#define SERIAL_DBG_MODE 0 
uint8_t DS_ADDR[][8] = { //адреса ds18b20
    {0x28, 0xFF, 0x54, 0x7E, 0x00, 0x17, 0x03, 0x1F },//АКБ прописать
    {0x28, 0xB6, 0x61, 0x68, 0xA1, 0x22, 0x09, 0x6A },//элек стратоспутника
    {0x28, 0x0B, 0x03, 0x44, 0xD4, 0xE1, 0x3C, 0x6D },//пласт 1
    {0x28, 0xE3, 0xFE, 0x44, 0xD4, 0xE1, 0x3C, 0xAF},//пласт 2
    {0x28, 0xD4, 0x98, 0x44, 0xD4, 0xE1, 0x3C, 0x4A },//внеш прописать
    {0x28, 0x1D, 0x03, 0x44, 0xD4, 0xE1, 0x3C, 0x84 },//окр_серда прописать
};

float ds_t[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
void dsGetTemp() { //хаха, я оставлю функцию, просто потому-что могу
  static Timer tmr(1000);
  if(tmr.ready()){
      ds18.requestTempAll();
  
    for(uint8_t indx = 0; indx < 6; indx++){
      if(ds18.readTemp(indx)){
        ds_t[indx]=ds18.getTemp(indx);
      }
    }
  }
}
double prs = 0;
double alt =  0;
void Logging() {
  static Timer tmr(5000);
  if (tmr.ready()) {
    Serial2.print(mode);
    Serial2.print("; ");

    for (int8_t indx = 0; indx < 6; indx++) {
      Serial2.print(ds_t[indx]);
      Serial2.print("; ");
    }

      Serial2.print(prs);
      Serial2.print("; ");

      Serial2.print(alt);
      Serial2.print("; ");
      
      Serial2.print(otr_srv.read());
      Serial2.print("; ");
      Serial2.print(millis());
      Serial2.println();
  }
}


void LoRa_Send() {
  static Timer tmr(60000);
  //static Send_LoRa buf;
  if (tmr.ready()) {
    if(ds_t[1]<37){
      if(!LORA_DBG_MODE){
        Serial1.print(mode);
        Serial1.print("; ");
        for (int8_t indx = 0; indx < 6; indx++) {
          Serial1.print(ds_t[indx]);
          Serial1.print("; ");
        }
        Serial1.print(prs);
        Serial1.print("; ");
  
        Serial1.print(alt);
        Serial1.print("; ");
  
        Serial1.print(otr_srv.read());
        Serial1.print("; ");
  
        Serial1.print(millis());
        Serial1.println();
      }
    }
  }
}

void thermReg() {
  static Timer tmr(2000);
  if (tmr.ready()) {
    if (ds_t[0] < 20) {
      digitalWrite(11, 1);
    } else {
      digitalWrite(11, 0);
    }
  }
}

void setup() {
  // put your setup code here, to run once:
  pinMode(11, OUTPUT);
  pinMode(37, OUTPUT);
  pinMode(42, OUTPUT);
  ds18.setAddress((uint8_t*)DS_ADDR);
  ds18.setResolutionAll(12);
   Serial1.begin(9600);
  delay(200);

  Serial2.begin(9600);
  delay(200);
  ms5611.begin();
  delay(500);
  digitalWrite(42,1);
  delay(500);
  otr_srv.attach(2);
  delay(500);
}

void loop() {
  // put your main code here, to run repeatedly:
  static Timer tmr(5000);
  static bool lo=0;
  if(tmr.ready()){
    digitalWrite(37,lo);
    lo=!lo;
    prs = ms5611.readPressure();
    alt =  ms5611.getAltitude(prs);
  }
  
  dsGetTemp();
  //Serial.println(millis()-l_time);

  thermReg();

  //SendData();
  //Parser();

  Logging();
  LoRa_Send();

  
  if(alt>10000){
    mode=1;
  }else{
    mode=0;
  }
  switch (mode)
  {
    case 0:
      if(lo){
        otr_srv.write(150);
      }else{
        otr_srv.write(50);
      }
      break;
    case 1:
      otr_srv.write(90-52);
      break;
    default:
      break;
  }
}
