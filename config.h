////****    Файл конфигурации   ****////
////////********   Режим работы    ********////////
#define SERIAL_DBG_MODE 1 //режим работы Serial (0 - битовая отпарвка, 1 - отправка строки)

////////********   Назначение пинов    ********////////
#define ONE_WIRE_BUS 10 //пин OneWire

#define SRV_PIN PC0 //поставить актуальный

#define MTR_F_PIN PD7 //поставить актуальный
#define MTR_B_PIN PD6 //поставить актуальный

#define LED_PIN PD2

////////********     Интервалы для таймеров    ********////////
#define DS_UPDATE_TIME 800
#define ADS_UPDATE_TIME 250
#define SEND_DATA_DELAY 800

#define COUNT_FLTR 5   //количество фильтраций
#define SERIAL_SPEED 57600

////////********     Начальные значения    ********////////

#define START_OTR_ANGL 0 // угл для закрытого отражателя

#define OPEN_OTR_ANGL 40 // угл для открытого отражателя  

////////********     Начальные значения    ********////////

#define EXP_START_ALTITUDE 15000 // высота начала эксперенмента 
#define EXP_START_TIME  213 //время начала эксперемента (Поставить верное в секундах)

#define SAFE_START_ALTITUDE 2000 // Режим для защиты от внешних факторов
#define SAFE_START_TIME 3600 // vvf

#define MAX_EL_TEMP 70 //максимальная температура электроники 
