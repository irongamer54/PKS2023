#include <IS_Bluetooth.h> 
#include <LightSensor.h>

#include <Servo.h> 

#include "my_timer.h" 

using namespace IntroSatLib;

Servo srv_z;

LightSensor light_m(Wire, MASTER_ADDR);
LightSensor light_r(Wire, RIGHT_ADDR);
LightSensor light_d(Wire, DOWN_ADDR);
LightSensor light_l(Wire, LEFT_ADDR);

HardwareSerial Serial2(PA3, PA2);//лог
HardwareSerial Serial3(PB11, PB10);//лора
 
uint8_t mode; 
float ds_t[7] = { 0, 0, 0, 0, 0, 0, 0}; 
float ik = 0; 
int32_t press = 0; 
int16_t altitude = 0; 
int16_t centr_s = 0; 
float azi = 0; 
int16_t servo_a[3] = { 0, 0, 0 }; 
int16_t ms = 0; 
float pwr=0;//мощность на СП


 
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
  Serial.println("data");
  if (Serial2.readBytes((byte *)&buf, sizeof(buf))) { 
    Serial.println(sizeof(buf));
    byte crc = crc8((byte *)&buf, sizeof(buf)); ;
    if (crc == 0) { 
      Serial.println("datatr");
      mode = buf.mode; 
      for (int i = 0; i < 6; i++) ds_t[i] = buf.temp[i]; 
      Serial.println(buf.temp[0]);
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

void Logging(){
  static Timer tmr(LOG_WRITE_DELAY);
  if(tmr.ready()){
    Serial2.print("Mode: ");
    Serial2.print(mode);
    Serial2.print(";");
    for(int8_t i=0; i<7; i++){
      Serial2.print(" DS");
      Serial2.print(i+1);
      Serial2.print(" ");
      Serial2.print(ds_t[i]);
      Serial2.print(";");
    }
    Serial2.print(" IK_Temp: ")
    Serial2.print(ik);
    Serial2.print(";");

    Serial2.print(" Pres: ")
    Serial2.print(press);
    Serial2.print(";");

    Serial2.print(" Alt: ")
    Serial2.print(altitude);
    Serial2.print(";");

    Serial2.print(" Azi: ")
    Serial2.print(azi);
    Serial2.print(";");

    for(uint8_t i=0; i<2; i++){
      Serial2.print(" Srv")
      Serial2.print(i+1);
      Serial2.print(": ")
      Serial2.print(servo_a);
      Serial2.print(";");
    }
    
    Serial2.print(" Mtr: ")
    Serial2.print(ms);
    Serial2.print(";");

    Serial2.print(" Pwr: ")
    Serial2.print(pwr);
    Serial2.print(";");
    
  }
}

void LoRa_Send(){
  static Timer tmr(LORA_SEND_DELAY);
  if(tmr.ready()){
    Serial3.print("Mode: ");
    Serial3.print(mode);
    Serial3.print(";");
    for(int8_t i=0; i<7; i++){
      Serial3.print(" DS");
      Serial3.print(i+1);
      Serial3.print(" ");
      Serial3.print(ds_t[i]);
      Serial3.print(";");
    }
    Serial3.print(" IK_Temp: ")
    Serial3.print(ik);
    Serial3.print(";");

    Serial3.print(" Pres: ")
    Serial3.print(press);
    Serial3.print(";");

    Serial3.print(" Alt: ")
    Serial3.print(altitude);
    Serial3.print(";");

    Serial3.print(" Azi: ")
    Serial3.print(azi);
    Serial3.print(";");

    for(uint8_t i=0; i<2; i++){
      Serial3.print(" Srv")
      Serial3.print(i+1);
      Serial3.print(": ")
      Serial3.print(servo_a);
      Serial3.print(";");
    }
    
    Serial2.print(" Mtr: ")
    Serial2.print(ms);
    Serial2.print(";");

    Serial2.print(" Pwr: ")
    Serial2.print(pwr);
    Serial2.print(";");
    
  }
}

float azimut(){
  uint16_t master_light = light_m.GetLight();
  uint16_t right_light = light_r.GetLight();
  uint16_t down_light = light_d.GetLight();
  uint16_t left_light = light_l.GetLight();
  float azim = atan2((left_light - right_light), (down_light - master_light)) * 180 / M_PI;
  return azim;
}

void setup() { 
  delay(500);

  Serial.begin(57600, SERIAL_8E1); 
  delay(500);

  Serial2.begin(57600);
  delay(500);

  Serial3.begin(57600); 
  delay(1000);

  srv_z.attach(SRV_PIN);
  delay(500);

  Wire.begin();
  delay(500);

  light_m.Init();
  light_r.Init();
  light_d.Init();
  light_l.Init();
  delay(500);
} 

void loop() { 
  Parser(); 
  Logging();
  LoRa_Send();

  azi=azimut();

  switch (mode)
  {
  case 0:
    standby();
    break;
  case 1:
    /* code */
    break;
  case 2:
    /* code */
    break;
  default:
    break;
  }
}
