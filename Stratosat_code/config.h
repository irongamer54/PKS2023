//////////////************ Конфигурация Стратоспутника ************//////////////

////////********   Режим работы    ********////////
#define SERIAL_DBG_MODE 0 //режим работы Serial (0 - битовая отпарвка, 1 - отправка строки)
#define LORA_DBG_MODE 0 //режим работы LoRa (1 - битовая отпарвка, 0 - отправка строки)
#define GPS_AVAIBLE 0

////////********     Интервалы для таймеров    ********////////
#define LOG_WRITE_DELAY 2000
#define LORA_SEND_DELAY 10000
#define TERM_REG_DELAY 5000
#define DS_UPDATE_TIME 800
#define SEND_DATA_DELAY 250
#define SRV_DELAY 100

///////////********* Настройки Serial *********///////////
#define LORA_SPEED 9600
#define LOG_SPEED 9600
#define PN_SPEED 9600
#define GPS_SPEED 57600

///////////********* Назначение пинов  *********///////////
#define ONE_WIRE_BUS 10 //пин OneWire

#define MIN_SRV_ANGL 35
#define MAX_SRV_ANGL 140
#define START_OTR_ANGL 140

#define DS_SENSOR_AMOUNT 6 //количество ds18b20

uint8_t DS_ADDR[][8] = { //адреса ds18b20
    {0x28, 0xFF, 0x54, 0x7E, 0x00, 0x17, 0x03, 0x1F },//АКБ прописать
    {0x28, 0xB6, 0x61, 0x68, 0xA1, 0x22, 0x09, 0x6A },//элек стратоспутника
    {0x28, 0x0B, 0x03, 0x44, 0xD4, 0xE1, 0x3C, 0x6D },//пласт 1
    {0x28, 0xE3, 0xFE, 0x44, 0xD4, 0xE1, 0x3C, 0xAF},//пласт 2
    {0x28, 0xD4, 0x98, 0x44, 0xD4, 0xE1, 0x3C, 0x4A },//внеш прописать
    {0x28, 0x1D, 0x03, 0x44, 0xD4, 0xE1, 0x3C, 0x84 },//окр_серда прописать
};

#define AKB_HEAT_PIN 11//пин подогрева акб
#define SRV_HEAT_PIN PB3 //пин подогрева сервы (поставить актуальный)

////////********     Начальные значения    ********////////
#define EXP_START_ALTITUDE 15000 // высота начала эксперенмента 
#define EXP_START_TIME  213 //время начала эксперемента (Поставить верное в секундах)

#define SAFE_START_ALTITUDE 2000 // Режим для защиты от внешних факторов
#define SAFE_START_TIME 3600 // vvf

///////////********* Настройка температурного режима  *********///////////
#define MIN_TEMP 20
#define MAX_TEMP 25

///////////********* Настройка температурного режима  *********///////////

#define M_PLAST1 0.01 //масса пластинки №1 [кг]
#define M_PLAST2 0.01 //масса пластинки №2 [кг]
#define C_PLAST 3000 //удельная теплоёмкость пластинок [Дж / ( кг * С )]


///////////********* PID коэфиценты  *********///////////
#define kP 3.0
#define kI 0.2
#define kD 0.5
