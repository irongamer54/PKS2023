////****    Файл конфигурации   ****////

////////********   Назначение пинов    ********////////
#define ONE_WIRE_BUS 5 //пин OneWire

#define SRV_PIN_1 1 //поставить актуальный
#define SRV_PIN_2 2 //поставить актуальный

#define MTR_F_1 1 //поставить актуальный
#define MTR_B_1 1 //поставить актуальный

#define MTR_F_2 1 //поставить актуальный
#define MTR_B_2 1 //поставить актуальный
#define COUNT_MAG 2 // количество магнитов на центрифуге

#define MAGN_PIN A3 //пин датчика холла
#define COUNT_MAG 1 //количество магнитов

////////********     Интервалы для таймеров    ********////////
#define DS_UPDATE_TIME 2000
#define ADS_UPDATE_TIME 2000
#define SEND_DATA_DELAY 1000


#define COUNT_FLTR 10   //количество фильтраций
#define SERIAL_SPEED 115200 

////////********     Начальные значения    ********////////

#define START_ANGL_1 0
#define START_ANGL_2 0
#define START_SPEED 0

////////********     Начальные значения    ********////////

#define EXP_START_ALTITUDE 15000 // высота начала эксперенмента 
#define EXP_START_TIME  //время начала эксперемента (Поставить верное в секундах)

#define SAFE_START_ALTITUDE 2000 // Режим для защиты от внешних факторов
#define SAFE_START_TIME 3600 // 

#define MAX_EL_TEMP 70 //максимальная температура электроники 