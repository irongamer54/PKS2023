//////////////************ Конфигурация Стратоспутника ************//////////////

////////********   Режим работы    ********////////
#define SERIAL_DBG_MODE 0 //режим работы Serial (0 - битовая отпарвка, 1 - отправка строки)
#define GPS_AVAIBLE 0

////////********     Интервалы для таймеров    ********////////
#define LOG_WRITE_DELAY 1000
#define LORA_SEND_DELAY 10000
#define TERM_REG_DELAY 5000
#define SEND_DATA_DELAY 800

///////////********* Настройки Serial *********///////////
#define LORA_SPEED 9600
#define LOG_SPEED 115200
#define PN_SPEED 57600
#define GPS_SPEED 57600

///////////********* Назначение пинов  *********///////////
#define ONE_WIRE_BUS 10 //пин OneWire

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

///////////********* PID коэфиценты  *********///////////
#define kP 3.0
#define kI 0.2
#define kD 0.5
