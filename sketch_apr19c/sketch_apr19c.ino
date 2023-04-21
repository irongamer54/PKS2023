//передача
#include <SoftwareSerial.h>
#include <IS_Bluetooth.h>


#define TX 1
#define RX 0

SoftwareSerial loraSerial(TX, RX);

uint8_t mode;
float ds_t[7] = { 20, 21, 22, 23, 24, 25, 26};
float ik = 28.2;
int32_t press = 100000;
int16_t altitude = 4000;
int16_t centr_s = 15;
float azi = 250;
int16_t servo_a[3] = { 15, 60, 120 };
int16_t ms = 15;



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

struct Lora_Send {
  uint8_t mode;
  float temp[7];
  float tempIK;
  int32_t prs;
  int16_t alt;
  float azi;
  int16_t speed_c;
  int16_t srv_angle[3];
  int16_t speed_m;
  byte crc;
};

void Parser() {  //парсинг Serial переделать
  static Read_data buf;

  if (Serial.readBytes((byte *)&buf, sizeof(buf))) {

    byte crc = crc8((byte *)&buf, sizeof(buf));

    if (crc == 0) {
      mode = buf.mode;
      for (int i = 0; i < 6; i++) ds_t[i] = buf.temp[i];
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

void LoRaRead() {
}

void LoRaSend() {
  static Lora_Send buf;
  buf.mode = mode;
  for (int i = 0; i < 6; i++) buf.temp[i] = ds_t[i];
  buf.tempIK = ik;
  buf.prs = press;
  buf.alt = altitude;
  buf.speed_c = centr_s;
  buf.azi = azi;
  for (int i = 0; i < 2; i++) buf.srv_angle[i] = servo_a[i];
  buf.speed_m = ms;
  for (int i = 0; i < 7; i++) {
    loraSerial.print("DS");
    loraSerial.print(i + 1);
    loraSerial.print(": ");
    loraSerial.println(ds_t[i]);
  }
  loraSerial.print("IK: ");
  loraSerial.print(ik);
  loraSerial.print("Azmuth: ");
  loraSerial.print(azi);
  loraSerial.print("Pressure: ");
  loraSerial.print(press);
  loraSerial.print("Сеntrifuge speed: ");
  loraSerial.print(centr_s);
  for (int i = 0; i < 3; i++) {
    loraSerial.print("SERVO");
    loraSerial.print(i + 1);
    loraSerial.print(": ");
    loraSerial.println(servo_a[i]);
  }
  loraSerial.print("Motor speed: ");
  loraSerial.print(ms);
  loraSerial.print("ModeSystem: ");
  loraSerial.print(mode);
}
void setup() {
  // put your setup code here, to run once:
  Serial.begin(57600);
  loraSerial.begin(57600);
}

void loop() {
  LoRaSend();
}
