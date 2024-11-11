
#include<Wire.h>

#include <OneWire.h>
#include <microDS18B20.h>//алт библиотека для работы с ds18b20 датчиком температуры
#include <MPU6050.h>

#include <MS5611.h>

#include <Servo.h>

#include "timer.h"
#include "SunPosition.h"//библиотека для определения положения солнца

#include "config.h"

MPU6050 mpu;
MS5611 ms5611;

SunPosition sun(55.755826, 37.6173, 1688558000);

OneWire oneWire(ONE_WIRE_BUS);

MicroDS18B20<10, DS_ADDR_MODE, DS_SENSOR_AMOUNT> ds18;

Servo otr_srv;
//HardwareSerial Serial1(PA3, PA2);//лог
//HardwareSerial Serial3(PB11, PB10);//плата

uint8_t mode = 0;
uint32_t fg=0;
float ds_t[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
float last_t[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };

float pwr[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
uint16_t foto_r[4] = { 0, 0, 0, 0 };

uint64_t last_time[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
uint64_t l_fg=0;

int32_t press = 0;
int16_t altitude = 0;

int16_t servo_a = 0;

float azim = 0;

int16_t ms = 0;

float pitch(){
  Vector normAccel = mpu.readNormalizeAccel();

  return -(atan2(normAccel.XAxis, sqrt(normAccel.YAxis*normAccel.YAxis + normAccel.ZAxis*normAccel.ZAxis))*180.0)/M_PI;
}

void pinSetup(){
  pinMode(AKB_HEAT_PIN, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(42, OUTPUT);
}

uint8_t DSInit(bool is_init = 0) { //функция инициализации ds18b20
  //delay(1000);
  //uint8_t ds_count = ds_sensors.getDeviceCount();
  //if(SERIAL_DBG_MODE)Serial.println(ds_count);
  if (!is_init) {
      ds18.setAddress((uint8_t*)DS_ADDR);
      ds18.setResolutionAll(12);
  }
  //return ds_count;
}

void dsGetTemp() { //хаха, я оставлю функцию, просто потому-что могу
  static Timer tmr(DS_UPDATE_TIME);
  if(tmr.ready()){
      ds18.requestTempAll();
  
    for(uint8_t indx = 0; indx < DS_SENSOR_AMOUNT; indx++){
      if(ds18.readTemp(indx)){
        ds_t[indx]=ds18.getTemp(indx);
      }
    }
  }
}

#pragma pack(push, 1)
struct Read_data {
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
struct Send_data {
  uint8_t mode;
  float tang;
  byte crc;
};
#pragma pack(pop)

void Parser() {  //парсинг Serial переделать
  static Read_data buf;
  if (Serial.readBytes((byte *)&buf, sizeof(buf))) {
    byte crc = crc8((byte *)&buf, sizeof(buf));
    if (crc == 0) {

      for(uint8_t indx = 0; indx < 4; indx++){
        foto_r[indx] = buf.f_res[indx];
      }

      mode = buf.mode;
      press = buf.prs;
      altitude = buf.alt;
      servo_a = buf.srv_angle;
      azim=buf.azim;
      ms = buf.speed_m;
      fg++;
      l_fg=millis();
    }
  }
}

void SendData() { //функция отправки данных
  static Timer tmr(SEND_DATA_DELAY);
  static Send_data buf;
  //static uint32_t t_l=0;
  if (tmr.ready()) {
    //if (SERIAL_DBG_MODE) Serial.println(String("////////////////// PACKET START //////////////////\nMillis: "+String(millis())));
      
    buf.mode = mode;
    buf.tang = pitch();

    if (SERIAL_DBG_MODE){
      //Serial.println(pitch());
    }

    byte crc = crc8((byte*)&buf, sizeof(buf) - 1);
    buf.crc = crc;
    if (!SERIAL_DBG_MODE)
        Serial.write((byte*)&buf, sizeof(buf));
  }
}

byte crc8(byte *buffer, byte size) {  // функция вычисления crc
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
double prs = 0;
double alt =  0;
void Logging() {
  static Timer tmr(LOG_WRITE_DELAY);
  if (tmr.ready()) {
    Serial2.print(mode);
    Serial2.print("; ");

    for (int8_t indx = 0; indx < DS_SENSOR_AMOUNT; indx++) {
      Serial2.print(ds_t[indx]);
      Serial2.print("; ");
    }

    for (int8_t indx = 0; indx < 4; indx++) {
      Serial2.print(foto_r[indx]);
      Serial2.print("; ");
    }
      Serial2.print(prs);
      Serial2.print("; ");

      Serial2.print(alt);
      Serial2.print("; ");
      Serial2.print(azim);
      Serial2.print("; ");

      Serial2.print(pitch());
      Serial2.print("; ");

      Serial2.print(otr_srv.read());
      Serial2.print("; ");
      Serial2.print(millis());
      Serial2.println();
  }
}


void LoRa_Send() {
  static Timer tmr(LORA_SEND_DELAY);
  //static Send_LoRa buf;
  if (tmr.ready()) {
    if(ds_t[1]<37){
      if(!LORA_DBG_MODE){
        Serial1.print(mode);
        Serial1.print("; ");
        for (int8_t indx = 0; indx < DS_SENSOR_AMOUNT; indx++) {
          pwr[indx] = ((ds_t[indx] - last_t[indx]) * M_PLAST1 * C_PLAST) / ((millis() - last_time[indx]) / 1000);
          last_t[indx] = ds_t[indx];
          last_time[indx] = millis();
          Serial1.print(ds_t[indx]);
          Serial1.print("; ");
        }
        Serial1.print(prs);
        Serial1.print("; ");
  
        Serial1.print(alt);
        Serial1.print("; ");
  
        Serial1.print(azim);
        Serial1.print("; ");
  
        Serial1.print(pitch());
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
  static Timer tmr(TERM_REG_DELAY);
  if (tmr.ready()) {
    if (ds_t[0] < MIN_TEMP) {
      digitalWrite(AKB_HEAT_PIN, 1);
    } else {
      digitalWrite(AKB_HEAT_PIN, 0);
    }
  }
}


////////////////////////////    SETUP    ////////////////////////////
void setup() {  
  Serial.begin(PN_SPEED);
  Serial.setTimeout(800);
  delay(200);

  Serial1.begin(LORA_SPEED);
  delay(200);

  Serial2.begin(LOG_SPEED);
  delay(200);
  
  if(GPS_AVAIBLE){
    Serial3.begin(GPS_SPEED);
    delay(200);
  }
  
  for (uint8_t c = 0; c < 50; c++) {
    if (ms5611.begin()) break;
    if (SERIAL_DBG_MODE) Serial.println("MS NOT START");
    delay(200);
  }
  
  mpu.begin(MPU6050_SCALE_2000DPS, MPU6050_RANGE_2G);
  
  Wire.begin();
  delay(500);

  pinSetup();

  DSInit();
  delay(500);

  //Serial1.println("Started");
  Serial2.println("Started");
  digitalWrite(42,1);
  delay(500);
  otr_srv.attach(2);
  delay(500);
  /*digitalWrite(7,0);
  delay(1000);
  digitalWrite(7,1);
  l_fg=millis();*/
}

void sun_orient(){
  static Timer tmr(SRV_DELAY);
  if (tmr.ready()){
    //float a=sun.altitude()+90+tang;
    float a=90-sun.altitude()-pitch()*0.75;
    a=constrain(a,MIN_SRV_ANGL,MAX_SRV_ANGL);
    otr_srv.write(a);
  }
  
  // прописать пид регулятор для наводки на солнце
}

void standby() {
  static Timer tmr(1000);
  if(tmr.ready()){
    otr_srv.write(START_OTR_ANGL);
  }
  
}

void self_mode() {
    sun_orient();
}

void hand_mode() {
  //Serial.print("ПРописать ");// нужно поработать ручками

}
////////////////////////////    LOOP    ////////////////////////////
void loop() {
  uint32_t l_time=millis();
  static Timer tmr(1000);
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
      /* code*/
      standby();
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

}
