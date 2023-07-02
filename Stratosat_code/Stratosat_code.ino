
#include<Wire.h>

#include <OneWire.h>
#include <DallasTemperature.h>//библиотека для работы с ds18b20 датчиком температуры
#include <MPU6050.h>

#include "timer.h"

#include "config.h"

MPU6050 mpu;

OneWire oneWire(ONE_WIRE_BUS);

DallasTemperature ds_sensors(&oneWire);
DeviceAddress *dsUnique;

//HardwareSerial Serial1(PA3, PA2);//лог
//HardwareSerial Serial3(PB11, PB10);//плата

uint8_t mode = 4;
float ds_t[8] = { 0, 0, 0, 0, 0, 0, 0, 0};

uint16_t foto_r[4]={0,0,0,0};

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

      for(uint8_t indx = 0; indx < 5; indx++){
        ds_t[indx] = buf.temp[indx];
      }

      for(uint8_t indx = 0; indx < 5; indx++){
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

  if (tmr.ready()) {
    if (SERIAL_DBG_MODE) Serial.println(String("////////////////// PACKET START //////////////////\nMillis: "+String(millis())));
      
    buf.mode = mode;

    //buf.srv_angle[0] = otr_srv.read();

    // 6 ds, 2 угла серв, ИК, давление, скорость центрифуги, обороты мотора, режим работы
    // Лере дописать эту часть кода (дозаполнить структуру)

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
    Serial2.print("Mode: ");
    Serial2.print(mode);
    Serial2.print(";");
    for (int8_t i = 0; i < 8; i++) {
      Serial2.print(" DS");
      Serial2.print(i + 1);
      Serial2.print(" ");
      Serial2.print(ds_t[i]);
      Serial2.print(";");
    }

    Serial2.print(" Pres: ");
    Serial2.print(press);
    Serial2.print(";");

    Serial2.print(" Alt: ");
    Serial2.print(altitude);
    Serial2.print(";");

    Serial2.print(" Azim: ");
    Serial2.print(azim);
    Serial2.print(";");



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
    for (int8_t i = 0; i < 8; i++) {
      Serial1.print(" DS");
      Serial1.print(i + 1);
      Serial1.print(" ");
      Serial1.print(ds_t[i]);
      Serial1.print(";");
    }

    Serial1.print(" Pres: ");
    Serial1.print(press);
    Serial1.print(";");

    Serial1.print(" Alt: ");
    Serial1.print(altitude);
    Serial1.print(";");

    Serial1.print(" Azim: ");
    Serial1.print(azim);
    Serial1.print(";");

    Serial1.print(" Mtr: ");
    Serial1.print(ms);
    Serial1.print(";");

    Serial1.print(" MIL: ");
    Serial1.print(millis());
    Serial1.println(";");
  }
}

void thermReg() {
  static Timer tmr(TERM_REG_DELAY);
  if (tmr.ready()) {
    ds_sensors.requestTemperatures();
    ds_t[6] = ds_sensors.getTempCByIndex(0);
    ds_t[7] = ds_sensors.getTempCByIndex(1);
    if (ds_t[6] < MIN_TEMP) {
     // digitalWrite(AKB_HEAT_PIN, 1);
    } else {
     // digitalWrite(AKB_HEAT_PIN, 0);
    }
    if (ds_t[7] < MIN_TEMP) {
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
  delay(200);

  Serial1.begin(LORA_SPEED);
  delay(200);

  Serial2.begin(LOG_SPEED);
  delay(200);
  
  if(GPS_AVAIBLE){
    Serial3.begin(GPS_SPEED);
    delay(200);
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
  //Serial2.println("Started");
  //Serial.println("Started");
  //delay(1000);
  //digitalWrite(PC13,1);
  thermReg();
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
