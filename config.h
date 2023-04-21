////****    Файл конфигурации   ****////

////////********   Назначение пинов    ********////////
#define ONE_WIRE_BUS 10 //пин OneWire

#define SRV_PIN_1 PC0 //поставить актуальный
#define SRV_PIN_2 PC1 //поставить актуальный

#define MTR_F_1 PD7 //поставить актуальный
#define MTR_B_1 PD6 //поставить актуальный

#define MAGN_PIN PC3//пин датчика холла
#define COUNT_MAG 1 //количество магнитов

////////********     Интервалы для таймеров    ********////////
#define DS_UPDATE_TIME 2000
#define ADS_UPDATE_TIME 2000
#define SEND_DATA_DELAY 2000

#define COUNT_FLTR 10   //количество фильтраций
#define SERIAL_SPEED 9600 

////////********     Начальные значения    ********////////

#define START_OTR_ANGL 0 // угл для закрытого отражателя
#define START_OTR2_ANGL 0 // угл для закрытого 2 отражателя
#define START_SPEED 0

#define OPEN_OTR_ANGL 90 // угл для открытого отражателя 
#define OPEN_OTR2_ANGL 45 // угл для открытого 2 отражателя 

#define CHNG_MODE_TIME 2000 //частота изменения режима работы камеры
#define CHNG_DIR 500 //смена направления камеры

#define CNTR_SPEED 100 //скорость врашения камеры (ШИМ -100 - 100)

////////********     Начальные значения    ********////////

#define EXP_START_ALTITUDE 15000 // высота начала эксперенмента 
#define EXP_START_TIME  213 //время начала эксперемента (Поставить верное в секундах)

#define SAFE_START_ALTITUDE 2000 // Режим для защиты от внешних факторов
#define SAFE_START_TIME 3600 // vvf

#define MAX_EL_TEMP 70 //максимальная температура электроники 
