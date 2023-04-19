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
  float prs;
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
    //mString<50> dataStr;
    buf.mode = mode;
    //dataStr+="n,t,"
    for (uint8_t indx = 0; indx < 6; indx++) {
      buf.temp[indx] = ds_sensors.getTempCByIndex(indx);
      //dataStr+=buf.temp[indx];
    }

    //dataStr+="i,"
    buf.tempIK = mlx.readObjectTempC();
    //dataStr+=buf.tempIK;

    //dataStr+=",b,"
    //buf.alfa=pos.altitude();

    // Сказать Лере дописать эту часть кода (дозаполнить структуру)

    byte crc = crc8((byte*)&buf, sizeof(buf) - 1);
    buf.crc = crc;

    Serial.write((byte*)&buf, sizeof(buf));
  }
}

void Parser() { //парсинг Serial переделать
  static Read_data buf;

  if (Serial.readBytes((byte*)&buf, sizeof(buf))) {

    byte crc = crc8((byte*)&buf, sizeof(buf));

    if (crc == 0) {

      mode = buf.mode;

      //for (uint8_t i =0; i<2;i++) cords[i]=buf.cords[i];

      //unix=buf.unix;

      // дописать Лере (записать данные в переменые в зависимости от режима)
    } else {
      //запросить повтор пакета
    }
  }
  /*uint8_t dt_len=data.split();
    if (data[0]=='f'){
    for(uint8_t i=1;i<dt_len;i++){
      char sim = data[i];
      switch (sim)
      {
      case 'g':
        cords[0]=data.getFloat(i+1);
        cords[1]=data.getFloat(i+2);
        i+=2;
        break;
      case 't':
        unix=data.getInt(i+1);
        i+=1;
        break;
      case 'm':
        mode=data.getInt(i+1);
        i+=1;
        break;
      case 's':
        speeds[0]=data.getInt(i+1);
        speeds[1]=data.getInt(i+2);
        i+=2;
        break;
      case 'a':
        srv_angle[0]=data.getInt(i+1);
        srv_angle[1]=data.getInt(i+2);
        i+=2;
        break;
      case 'r':
        trn_speed[0]=data.getInt(i+1);
        trn_speed[1]=data.getInt(i+2);
        trn_speed[2]=data.getInt(i+3);
        i+=3;
        break;
      }
    }
    }*/
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
  pinMode(SRV_PIN_1, OUTPUT);
  pinMode(SRV_PIN_1, OUTPUT);
}

void setup() {
  Serial.begin(SERIAL_SPEED);

  Wire.begin();

  for (uint8_t c = 0; c < 50; c++) { // Инициализация датчика
    if (mlx.begin()) break;
    delay(200);
  }

  for (uint8_t c = 0; c < 50; c++) {
    if (ms5611.begin()) break;
    delay(200);
  }

  DSInit();

  ads.setGain(GAIN_TWOTHIRDS);
  for (uint8_t c = 0; c < 50; c++) {
    if (ads.begin()) break;
    delay(200);
  }

  pinSetup();

  otr_srv.attach(SRV_PIN_1);
  otr2_srv.attach(SRV_PIN_2);

  Timer0.setFrequency(40000);
  Timer0.enableISR();
}

/*
  void xz_chto_ito(){

  // Read true temperature & Pressure
  double realTemperature = ms5611.readTemperature();
  long realPressure = ms5611.readPressure();

  // Calculate altitude
  float absoluteAltitude = ms5611.getAltitude(realPressure);
  float relativeAltitude = ms5611.getAltitude(realPressure, referencePressure);

  Serial.println("--");

  Serial.print(", realTemp = ");
  Serial.print(realTemperature);
  Serial.println(" *C");

  Serial.print(", realPressure = ");
  Serial.print(realPressure);
  Serial.println(" Pa");

  Serial.print(" absoluteAltitude = ");
  Serial.print(absoluteAltitude);
  Serial.print(" m, relativeAltitude = ");
  Serial.print(relativeAltitude);
  Serial.println(" m");

  delay(1000);

  }

  void checkSettings()
  {
  Serial.print("Oversampling: ");
  Serial.println(ms5611.getOversampling());
  }
*/

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

void hand_mode(){
  Serial.print("ПРописать ");// нужно поработать ручками 
}

void loop() {

  flt_ads();
  dsGetTemp();
  Parser();
  SendData();

  switch (mode)
  {
    case 0:
      /* code */
      standby();
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
