#include<Wire.h>

#include <OneWire.h>
#include <microDS18B20.h>//алт библиотека для работы с ds18b20 датчиком температуры
#include <MPU6050.h>

#include "timer.h"

#include "config.h"

MPU6050 mpu;

OneWire oneWire(ONE_WIRE_BUS);

MicroDS18B20<10, DS_ADDR_MODE, DS_SENSOR_AMOUNT> ds18;

//HardwareSerial Serial1(PA3, PA2);//лог
//HardwareSerial Serial3(PB11, PB10);//плата

uint8_t mode = 4;
float ds_t[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
float last_t[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
float pwr[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
uint16_t foto_r[4] = { 0, 0, 0, 0 };

uint64_t last_time[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };

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
  pinMode(SRV_HEAT_PIN, OUTPUT);
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
    for (int i = 0; i < DS_SENSOR_AMOUNT; i++) {
      ds18.requestTempAll();
    }
  }
  for(uint8_t indx = 0; indx < DS_SENSOR_AMOUNT; i++){
    if(ds18.readTemp(indx)){
      ds_t[indx]=ds18.getTemp(indx);
      pwr[indx] = ((ds_t[indx] - last_t[indx]) * M_PLAST1 * C_PLAST) / ((millis() - last_time[indx]) / 1000);
      last_t[indx] = ds_t[indx];
      last_time[indx] = millis();
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
  int8_t tang;
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
    }
  }
}

void SendData() { //функция отправки данных
  static Timer tmr(SEND_DATA_DELAY);
  static Send_data buf;
  //static uint32_t t_l=0;
  if (tmr.ready()) {
    if (SERIAL_DBG_MODE) Serial.println(String("////////////////// PACKET START //////////////////\nMillis: "+String(millis())));
      
    buf.mode = mode;
    buf.tang = pitch();

    if (SERIAL_DBG_MODE){
      Serial.println(pitch());
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

void Logging() {
  static Timer tmr(LOG_WRITE_DELAY);
  if (tmr.ready()) {
    Serial2.print(mode);
    Serial2.print("; ");

    for (int8_t indx = 0; indx < 8; indx++) {
      Serial2.print(ds_t[indx]);
      Serial2.print("; ");
      Serial2.print(pwr[indx]);
      Serial2.print("; ");
    }

    Serial2.print(press);
    Serial2.print("; ");

    Serial2.print(altitude);
    Serial2.print("; ");

    Serial2.print(azim);
    Serial2.print("; ");

    Serial2.print(pitch());
    Serial2.print("; ");

    Serial2.print(servo_a);
    Serial2.print("; ");

    Serial2.print(ms);
    Serial2.print("; ");

    Serial2.print(millis());
    Serial2.println("; ");
  }
}

void LoRa_Send() {
  static Timer tmr(LORA_SEND_DELAY);
  if (tmr.ready()) {
    Serial1.print(mode);
    Serial1.print("; ");
    for (int8_t indx = 0; indx < 8; indx++) {
      Serial1.print(ds_t[indx]);
      Serial1.print("; ");
      Serial1.print(pwr[indx]);
      Serial1.print("; ");
    }

    Serial1.print(press);
    Serial1.print("; ");

    Serial1.print(altitude);
    Serial1.print("; ");

    Serial1.print(azim);
    Serial1.print("; ");

    Serial1.print(pitch());
    Serial1.print("; ");

    Serial1.print(servo_a);
    Serial1.print("; ");

    Serial1.print(ms);
    Serial1.print("; ");

    Serial1.print(millis());
    Serial1.println("; ");
  }
}

void thermReg() {
  static Timer tmr(TERM_REG_DELAY);
  if (tmr.ready()) {
    if (ds_t[0] < MIN_TEMP) {
     // digitalWrite(AKB_HEAT_PIN, 1);
    } else {
     // digitalWrite(AKB_HEAT_PIN, 0);
    }
    if (ds_t[1] < MIN_TEMP) {
    //  digitalWrite(SRV_HEAT_PIN, 1);
    } else  {
    //  digitalWrite(SRV_HEAT_PIN, 0);
    }
  }
}

void standby() {
}

void selfMode() {
}

void handMode() {
  // расписать подачу данных напрямую

}

////////////////////////////    SETUP    ////////////////////////////
void setup() {  
  Serial.begin(PN_SPEED);
  Serial.setTimeout(100);
  delay(200);

  Serial1.begin(LORA_SPEED);
  delay(200);

  Serial2.begin(LOG_SPEED);
  delay(200);
  
  if(GPS_AVAIBLE){
    Serial3.begin(GPS_SPEED);
    delay(200);
  }

  while(!mpu.begin(MPU6050_SCALE_2000DPS, MPU6050_RANGE_2G))
  {
    Serial1.println("Could not find a valid MPU6050 sensor, check wiring!");
    delay(500);
  }
  
  Wire.begin();
  delay(500);

  pinSetup();

  DSInit();
  delay(500);

  Serial1.println("Started");
  Serial2.println("Started");
}

////////////////////////////    LOOP    ////////////////////////////
void loop() {
  uint64_t l_time=millis();
  dsGetTemp();
  Serial.println(millis()-l_time);

  thermReg();

  SendData();
  Parser();

  Logging();
  LoRa_Send();

  switch (mode)
  {
    case 0:
      standby();
      break;
    case 1:
      selfMode();
      break;
    case 2:
      handMode();
      break;
    default:
      break;
  }
  Serial.println(millis()-l_time);
}
