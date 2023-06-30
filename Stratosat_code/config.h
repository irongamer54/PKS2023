//////////////************ Конфигурация Стратоспутника ************//////////////
#define LOG_WRITE_DELAY 1000
#define LORA_SEND_DELAY 15000
#define TERM_REG_DELAY 5000

#define ONE_WIRE_BUS PA15 //пин OneWire
#define AKB_HEAT_PIN PB3//пин подогрева акб
#define SRV_HEAT_PIN PB3 //пин подогрева сервы

#define EXP_START_ALTITUDE 15000 // высота начала эксперенмента 
#define EXP_START_TIME  213 //время начала эксперемента (Поставить верное в секундах)

#define SAFE_START_ALTITUDE 2000 // Режим для защиты от внешних факторов
#define SAFE_START_TIME 3600 // vvf

#define MIN_TEMP 20
#define MAX_TEMP 25

#define kP 3.0
#define kI 0.2
#define kD 0.5
