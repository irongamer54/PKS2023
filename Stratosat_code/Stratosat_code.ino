
#include<Wire.h>

#include <OneWire.h>
#include <DallasTemperature.h>//библиотека для работы с ds18b20 датчиком температуры

#include "timer.h"

#include "config.h"


OneWire oneWire(ONE_WIRE_BUS);

DallasTemperature ds_sensors(&oneWire);
DeviceAddress *dsUnique;

//HardwareSerial Serial1(PA3, PA2);//лог
//HardwareSerial Serial3(PB11, PB10);//плата

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
  if (Serial.readBytes((byte *)&buf, sizeof(buf))) {
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

    Serial2.print(" MIL: ");
    Serial2.print(millis());
    Serial2.println(";");

  }
}

void LoRa_Send() {
  static Timer tmr(LORA_SEND_DELAY);
  if (tmr.ready()) {
    Serial1.print("Mode: ");
    Serial1.print(mode);
    Serial1.print(";");
    for (int8_t i = 0; i < 7; i++) {
      Serial1.print(" DS");
      Serial1.print(i + 1);
      Serial1.print(" ");
      Serial1.print(ds_t[i]);
      Serial1.print(";");
    }
    Serial1.print(" IK_Temp: ");
    Serial1.print(ik);
    Serial1.print(";");

    Serial1.print(" Pres: ");
    Serial1.print(press);
    Serial1.print(";");

    Serial1.print(" Alt: ");
    Serial1.print(altitude);
    Serial1.print(";");

    Serial1.print(" Azi: ");
    Serial1.print(azi);
    Serial1.print(";");

    for (uint8_t i = 0; i < 2; i++) {
      Serial1.print(" Srv");
      Serial1.print(i + 1);
      Serial1.print(": ");
      Serial1.print(servo_a[i]);
      Serial1.print(";");
    }

    Serial1.print(" Mtr: ");
    Serial1.print(ms);
    Serial1.print(";");

    Serial1.print(" MIL: ");
    Serial1.print(millis());
    Serial1.println(";");
  }
}

void standby() {
  srv_z.write(0);
}

void selfMode() {

}

void thermReg() {
  //написать Лере подогрев акб
  static Timer tmr(5000);
  if (tmr.ready()) {
    ds_sensors.requestTemperatures();
    ds_t[6] = ds_sensors.getTempCByIndex(0);
    ds_t[7] = ds_sensors.getTempCByIndex(1);
    if (ds_t[6] < MIN_TEMP) {
      digitalWrite(AKB_HEAT_PIN, 1);
    } else {
      digitalWrite(AKB_HEAT_PIN, 0);
    }
    if (ds_t[7] < MIN_TEMP) {
      digitalWrite(SRV_HEAT_PIN, 1);
    } else  {
      digitalWrite(SRV_HEAT_PIN, 0);
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

  Serial1.begin(9600);
  delay(500);

  Serial2.begin(9600);
  delay(1000);

  delay(500);
  Wire.begin();
  delay(500);

  pinMode(PC13, OUTPUT);
  DSInit();
  delay(500);

  Serial.println("Started");
  Serial2.println("Started");
}

////////////////////////////    LOOP    ////////////////////////////
void loop() {
  //Serial2.println("Started");
  //Serial.println("Started");
  //delay(1000);
  //digitalWrite(PC13,1);
  //thermReg();
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
}
