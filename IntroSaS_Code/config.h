#define LOG_WRITE_DELAY 2000
#define LORA_SEND_DELAY 15000

#define SRV_PIN PB5//прописать актуальный
#define SRV_GRAT 2 // передаточное число

#define ONE_WIRE_BUS PA15 //пин OneWire
#define HEAT_PIN PB3//dawdw

#define NAG_RES 100 // сопротивление нагрузки

#define INA_ADDR 0x40

#define MASTER_ADDR 0x51 //адреса датчиков
#define RIGHT_ADDR 0x50
#define DOWN_ADDR 0x53
#define LEFT_ADDR 0x52

#define EXP_START_ALTITUDE 15000 // высота начала эксперенмента 
#define EXP_START_TIME  213 //время начала эксперемента (Поставить верное в секундах)

#define SAFE_START_ALTITUDE 2000 // Режим для защиты от внешних факторов
#define SAFE_START_TIME 3600 // vvf

#define MIN_TEMP 20
#define MAX_TEMP 25

#define kP 3.0
#define kI 0.2
#define kD 0.5
