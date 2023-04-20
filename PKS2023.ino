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

OneWire oneWire(ONE_WIRE_BUS);

DallasTemperature ds_sensors(&oneWire);
DeviceAddress *dsUnique;

Adafruit_MLX90614 mlx = Adafruit_MLX90614();
Adafruit_ADS1115 ads;

MS5611 ms5611;

Servo otr_srv;
Servo otr2_srv;

Motor cam_mtr(MTR_F_1, MTR_B_1); //переименовать)
Centrifuge centri(MAGN_PIN, COUNT_MAG); //пример

uint8_t mode = 0;

int16_t speeds[2] = {0, 0};

int16_t srv_angle[2] = {0, 0}; //поставить начальные углы

uint8_t trn_speed[3] = {0, 0, 0};

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

struct Send_data {
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

struct Read_data {
  uint8_t mode;
  float srv_angle[2];
  int16_t speed_c;
  int16_t speed_m[2];
  byte crc;
};

void SendData() { //функция отправки данных
  static Timer tmr(SEND_DATA_DELAY);
  static Send_data buf;

  if (tmr.ready()) {

    buf.mode = mode;
    for (uint8_t indx = 0; indx < 6; indx++) {
      buf.temp[indx] = ds_sensors.getTempCByIndex(indx);
    }

    buf.tempIK = mlx.readObjectTempC();

    buf.srv_angle[0] = otr_srv.read();
    buf.srv_angle[1] = otr2_srv.read();

    buf.speed_m = cam_mtr.getSpeed();

    buf.prs = ms5611.readPressure();
    buf.alt = ms5611.getAltitude(buf.prs);

    buf.speed_c = centri.getSpeed();
    // 6 ds, 2 угла серв, ИК, давление, скорость центрифуги, обороты мотора, режим работы
    // Сказать Лере дописать эту часть кода (дозаполнить структуру)

    byte crc = crc8((byte*)&buf, sizeof(buf) - 1);
    buf.crc = crc;

    Serial.write((byte*)&buf, sizeof(buf));
    Serial.println("Data_SEND");
  }
}

void Parser() { //парсинг Serial переделать
  static Read_data buf;

  if (Serial.readBytes((byte*)&buf, sizeof(buf))) {

    byte crc = crc8((byte*)&buf, sizeof(buf));

    if (crc == 0) {
      mode = buf.mode;

      // дописать Лере (записать данные в переменые в зависимости от режима)
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

void pinSetup() {
  //pinMode(SRV_PIN_1, OUTPUT);
  //pinMode(SRV_PIN_2, OUTPUT);
  pinMode(2, OUTPUT);
}


void standby() {
  otr_srv.write(START_OTR_ANGL);
  otr2_srv.write(START_OTR2_ANGL);
  cam_mtr.setSpeed(START_SPEED);

}

void self_mode() {
  static Timer tmr(CHNG_MODE_TIME);
  static Timer tmr_2(CHNG_DIR);

  static int8_t dir = 1;
  static uint8_t cntr_move_mode = 0;


  if (tmr.ready()) {
    cntr_move_mode++;
    if (cntr_move_mode >= 3) {
      cntr_move_mode = 0;
    }
  }

  otr2_srv.write(OPEN_OTR2_ANGL);
  otr_srv.write(OPEN_OTR_ANGL);

  switch (cntr_move_mode)
  {
    case 0:
      cam_mtr.setSpeed(CNTR_SPEED);
      break;
    case 1:
      cam_mtr.setSpeed(CNTR_SPEED * -1);
      break;
    case 2:
      if (tmr_2.ready()) dir *= -1;
      cam_mtr.setSpeed(CNTR_SPEED * dir);
      break;
    default:
      break;
  }
}

void hand_mode() {
  Serial.print("ПРописать ");// нужно поработать ручками
}

void setup() {
  delay(1000);
  Serial.begin(SERIAL_SPEED);
  delay(1000);
  Wire.begin();
  Serial.println("Start1");
  delay(1000);
  for (uint8_t c = 0; c < 5; c++) { // Инициализация датчика
    if (mlx.begin()) break;
    Serial.println("MLX NOT START");
    delay(200);
  }
  Serial.println("Start1");

  for (uint8_t c = 0; c < 50; c++) {
    if (ms5611.begin()) break;
    Serial.println("ADS NOT START");
    delay(200);
  }
  Serial.println("Start1");

  DSInit();
  delay(1000);
  Serial.println("Start1");
  ads.setGain(GAIN_TWOTHIRDS);
  for (uint8_t c = 0; c < 50; c++) {
    if (ads.begin()) break;
    Serial.println("ADS NOT START");
    delay(200);
  }
  Serial.println("Start1");
  pinSetup();
  delay(1000);
  otr_srv.attach(A0);
  delay(1000);
  otr2_srv.attach(A1);
  delay(1000);
  Timer2.setFrequency(40000);
  Timer2.enableISR();

  Serial.println("Start");
}

void loop() {
  /*delay(1000);
  static Timer tmr(2000);
  static int16_t dr = 90;
  if (tmr.ready()) {
    dr = abs(dr - 90);
    
    Serial.println(dr);
  }
  otr_srv.write(dr);
  otr2_srv.write(dr);
  delay(500);*/

  //cam_mtr.setSpeed(START_SPEED);
 flt_ads();
  dsGetTemp();
  //Parser();
  //SendData();

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
      hand_mode();
      break;
    default:
      break;
  }
   
}

ISR(TIMER2_A) {
  cam_mtr.newTick();//переименовать)
  centri.newTick();
  // функцию для скорости центрифуги сюда Лере
}
