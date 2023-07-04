////****    Файл конфигурации   ****////
////////********   Режим работы    ********////////
#define SERIAL_DBG_MODE 1 //режим работы Serial (0 - битовая отпарвка, 1 - отправка строки)

////////********   Назначение пинов    ********////////
#define ONE_WIRE_BUS 10 //пин OneWire
#define DS_SENSOR_AMOUNT 3 //количество ds18b20

uint8_t DS_ADDR[][8] = { //адреса ds18b20
  {0x28, 0x0B, 0x03, 0x44, 0xD4, 0xE1, 0x3C, 0x6D},//пласт 1
  {0x28, 0xE3, 0xFE, 0x44, 0xD4, 0xE1, 0x3C, 0xAF},//пласт 2
  {0x28, 0x70, 0xA2, 0x95, 0x74, 0x22, 0x01, 0x29},//встроенный на плате
  {0x28, 0xFF, 0x42, 0x5A, 0x51, 0x17, 0x4, 0xD2},
  {0x28, 0xFF, 0xCD, 0x59, 0x51, 0x17, 0x4, 0xFE},
};

#define SRV_PIN A0 //поставить актуальный

#define MTR_F_PIN PD7 //поставить актуальный
#define MTR_B_PIN PD6 //поставить актуальный

#define LED_PIN PD2

////////********     Интервалы для таймеров    ********////////
#define DS_UPDATE_TIME 800
#define ADS_UPDATE_TIME 250
#define SEND_DATA_DELAY 800

#define SRV_DELAY 100

#define COUNT_FLTR 5   //количество фильтраций
#define SERIAL_SPEED 9600

////////********     Начальные значения    ********////////
#define START_OTR_ANGL 0 // угл для закрытого отражателя 

////////********     Начальные значения    ********////////

#define EXP_START_ALTITUDE 15000 // высота начала эксперенмента 
#define EXP_START_TIME  213 //время начала эксперемента (Поставить верное в секундах)

#define SAFE_START_ALTITUDE 2000 // Режим для защиты от внешних факторов
#define SAFE_START_TIME 3600 // vvf

#define MAX_EL_TEMP 70 //максимальная температура электроники 
