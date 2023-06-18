#include <IS_Bluetooth.h>
#include <LightSensor.h>
#include<Wire.h>

#include <OneWire.h>
#include <DallasTemperature.h>//библиотека для работы с ds18b20 датчиком температуры

#define PID_INTEGER
#include <GyverPID.h>

#include <AmperkaServo.h>

#include <INA219_WE.h> //https://compacttool.ru/datchik-napryazheniya-i-toka-na-chipe-ina219
#include "my_timer.h"

#include "config.h"

using namespace IntroSatLib;

AmperkaServo srv_z;

OneWire oneWire(ONE_WIRE_BUS);

DallasTemperature ds_sensors(&oneWire);
DeviceAddress *dsUnique;

INA219_WE ina219(INA_ADDR);

LightSensor light_m(Wire, MASTER_ADDR);
LightSensor light_r(Wire, RIGHT_ADDR);

LightSensor light_d(Wire, DOWN_ADDR);
LightSensor light_l(Wire, LEFT_ADDR);

HardwareSerial Serial2(PA3, PA2);//лог
HardwareSerial Serial3(PB11, PB10);//плата

uint8_t mode = 4;
float ds_t[7] = { 0, 0, 0, 0, 0, 0, 0};
float ik = 0;
int32_t press = 0;
int16_t altitude = 0;
int16_t centr_s = 0;
float azi = 0;
int16_t servo_a[3] = { 0, 0, 0 };
int16_t ms = 0;
float pwr = 0; //мощность на СП

GyverPID regulator(kP, kI, kD, 200);

uint8_t DSInit(bool is_init = 0) { //функция инициализации ds18b20
  delay(1000);
  uint8_t ds_count = ds_sensors.getDeviceCount();
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

#pragma pack(push, 1)
struct Read_data {
  uint8_t mode;
  float temp[6];
  float tempIK;
  int32_t prs;
  int16_t alt;
  int16_t speed_c;
  int16_t srv_angle[2];
  int16_t speed_m;
  byte crc;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct Send_data {
  uint8_t mode;
  float srv_angle[2];
  int16_t speed_m;
  byte crc;
};
#pragma pack(pop)

void Parser() {  //парсинг Serial переделать
  static Read_data buf;
  //Serial.println("data");
  if (Serial3.readBytes((byte *)&buf, sizeof(buf))) {
    //Serial.println(sizeof(buf));
    byte crc = crc8((byte *)&buf, sizeof(buf)); ;
    if (crc == 0) {
      //Serial.println("datatr");
      mode = buf.mode;
      for (int i = 0; i < 6; i++) ds_t[i] = buf.temp[i];
      //Serial.println(buf.temp[0]);
      ik = buf.tempIK;
      press = buf.prs;
      altitude = buf.alt;
      centr_s = buf.speed_c;
      for (int i = 0; i < 2; i++) servo_a[i] = buf.srv_angle[i];
      ms = buf.speed_m;
    }
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
    Serial2.print("Mode: ");
    Serial2.print(mode);
    Serial2.print(";");
    for (int8_t i = 0; i < 7; i++) {
      Serial2.print(" DS");
      Serial2.print(i + 1);
      Serial2.print(" ");
      Serial2.print(ds_t[i]);
      Serial2.print(";");
    }
    Serial2.print(" IK_Temp: ");
    Serial2.print(ik);
    Serial2.print(";");

    Serial2.print(" Pres: ");
    Serial2.print(press);
    Serial2.print(";");

    Serial2.print(" Alt: ");
    Serial2.print(altitude);
    Serial2.print(";");

    Serial2.print(" Azi: ");
    Serial2.print(azi);
    Serial2.print(";");

    for (uint8_t i = 0; i < 2; i++) {
      Serial2.print(" Srv");
      Serial2.print(i + 1);
      Serial2.print(": ");
      Serial2.print(servo_a[i]);
      Serial2.print(";");
    }

    Serial2.print(" Mtr: ");
    Serial2.print(ms);
    Serial2.print(";");

    Serial2.print(" ML: ");
    Serial2.print(light_m.GetLight());
    Serial2.println(";");

    Serial2.print(" RL: ");
    Serial2.print(light_r.GetLight());
    Serial2.println(";");

    Serial2.print(" DL: ");
    Serial2.print(light_d.GetLight());
    Serial2.println(";");

    Serial2.print(" LL: ");
    Serial2.print(light_l.GetLight());
    Serial2.println(";");

    Serial2.print(" MIL: ");
    Serial2.print(millis());
    Serial2.println(";");

  }
}

void LoRa_Send() {
  static Timer tmr(LORA_SEND_DELAY);
  if (tmr.ready()) {
    Serial.print("Mode: ");
    Serial.print(mode);
    Serial.print(";");
    for (int8_t i = 0; i < 7; i++) {
      Serial.print(" DS");
      Serial.print(i + 1);
      Serial.print(" ");
      Serial.print(ds_t[i]);
      Serial.print(";");
    }
    Serial.print(" IK_Temp: ");
    Serial.print(ik);
    Serial.print(";");

    Serial.print(" Pres: ");
    Serial.print(press);
    Serial.print(";");

    Serial.print(" Alt: ");
    Serial.print(altitude);
    Serial.print(";");

    Serial.print(" Azi: ");
    Serial.print(azi);
    Serial.print(";");

    for (uint8_t i = 0; i < 2; i++) {
      Serial.print(" Srv");
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.print(servo_a[i]);
      Serial.print(";");
    }

    Serial.print(" Mtr: ");
    Serial.print(ms);
    Serial.print(";");

    Serial.print(" ML: ");
    Serial.print(light_m.GetLight());
    Serial.print(";");

    Serial.print(" RL: ");
    Serial.print(light_r.GetLight());
    Serial.print(";");

    Serial.print(" DL: ");
    Serial.print(light_d.GetLight());
    Serial.print(";");

    Serial.print(" LL: ");
    Serial.print(light_l.GetLight());
    Serial.print(";");

    Serial.print(" MIL: ");
    Serial.print(millis());
    Serial.println(";");
  }
}

float azimut() {
  uint16_t master_light = light_m.GetLight();
  uint16_t right_light = light_r.GetLight();
  uint16_t down_light = light_d.GetLight();
  uint16_t left_light = light_l.GetLight();
  float azim = atan2((left_light - right_light), (down_light - master_light)) * 180 / M_PI;
  return azim;
}

void sun_orient() {
  static Timer tmr(200);
  if (tmr.ready()) {
    regulator.input = azimut();
    srv_z.writeSpeed(regulator.getResultTimer());
  }
}

void standby() {
  srv_z.write(0);
}

void selfMode() {
  sun_orient();
}

void thermReg() {
  //написать лере подогрев акб
  static Timer tmr(5000);
  if (tmr.ready()) {
    ds_sensors.requestTemperatures();
    ds_t[6] = ds_sensors.getTempCByIndex(0);
    float tmp = ds_t[6];
    if (tmp < MIN_TEMP) {
      digitalWrite(HEAT_PIN, 1);
    } else {
      digitalWrite(HEAT_PIN, 0);
    }
  }
}

void handMode() {
  // расписать подачу данных напрямую

}

////////////////////////////    SETUP    ////////////////////////////
void setup() {  
  Serial.begin(57600);
  delay(500);

  Serial2.begin(57600);
  delay(500);

  Serial3.begin(9600);
  delay(1000);

  //srv_z.attach(SRV_PIN, 544, 2400);
  delay(500);
  Wire.begin();
  delay(500);

  pinMode(PC13, OUTPUT);
  DSInit();
  /*for (uint8_t c = 0; c < 5; c++) { // Инициализация датчика
    if (ina219.init()) break;
    Serial.println("INA NOT START");
    Serial2.println("INA NOT START");
    delay(200);
    }*/
  //ina219.setADCMode(SAMPLE_MODE_64); // 128 выборок для усреднения измерений по току
  //ina219.setPGain(PG_160); // измеряем ток в пределах 400 мА
  //ina219.setBusRange(BRNG_16); // напряжение до 16 В

  light_m.Init();
  light_r.Init();
  light_d.Init();
  light_l.Init();
  delay(500);
  regulator.setDirection(NORMAL);
  regulator.setLimits(0, 255);
  regulator.setpoint = 0;
  Serial.println("Started");
  Serial2.println("Started");
}

////////////////////////////    LOOP    ////////////////////////////
void loop() {
  //Serial2.println("Started");
  //Serial.println("Started");
  //delay(1000);
  //digitalWrite(PC13,1);
  thermReg();
  Parser();
  Logging();
  LoRa_Send();

  //pwr=ina219.getBusPower();

  azi = azimut();

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
}
