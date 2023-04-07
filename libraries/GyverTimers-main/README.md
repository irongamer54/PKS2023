[![latest](https://img.shields.io/github/v/release/GyverLibs/GyverTimers.svg?color=brightgreen)](https://github.com/GyverLibs/GyverTimers/releases/latest/download/GyverTimers.zip)
[![Foo](https://img.shields.io/badge/Website-AlexGyver.ru-blue.svg?style=flat-square)](https://alexgyver.ru/)
[![Foo](https://img.shields.io/badge/%E2%82%BD$%E2%82%AC%20%D0%9D%D0%B0%20%D0%BF%D0%B8%D0%B2%D0%BE-%D1%81%20%D1%80%D1%8B%D0%B1%D0%BA%D0%BE%D0%B9-orange.svg?style=flat-square)](https://alexgyver.ru/support_alex/)
[![Foo](https://img.shields.io/badge/README-ENGLISH-blueviolet.svg?style=flat-square)](https://github-com.translate.goog/GyverLibs/GyverTimers?_x_tr_sl=ru&_x_tr_tl=en)  

[![Foo](https://img.shields.io/badge/ПОДПИСАТЬСЯ-НА%20ОБНОВЛЕНИЯ-brightgreen.svg?style=social&logo=telegram&color=blue)](https://t.me/GyverLibs)

# GyverTimers
Настройка и контроль прерываний по аппаратным таймерам ATmega328p, ATmega2560
- Поддерживаются все три таймера на ATmega328 и шесть таймеров на ATmega2560;		
- Настройка периода (мкс) и частоты (Гц) прерываний:
    - 8 бит таймеры: 61 Гц - 1 МГц (16 384 мкс.. 1 мкс);
    - 16 бит таймеры: 0.24 Гц - 1 МГц (4 200 000 мкс.. 1 мкс);
- Автоматическая корректировка настройки периода от частоты тактирования (F_CPU);
- Функция возвращает точный установившийся период/частоту для отладки (частота ограничена разрешением таймера);
- Поддержка многоканального режима работы: один таймер вызывает 2 (ATmega328) или
3 (ATmega2560, таймеры 1, 3, 4, 5) прерывания с настраиваемым сдвигом по фазе 0-360 градусов;
- Настраиваемое действие аппаратного вывода таймера по прерыванию: высокий сигнал, низкий сигнал, переключение. 
Позволяет генерировать меандр (одно- и двухтактный);
- Контроль работы таймера: старт/стоп/пауза/продолжить/инициализация;

### Совместимость
ATmega328p, ATmega2560

### Документация
К библиотеке есть [расширенная документация](https://alexgyver.ru/GyverTimers/)

## Содержание
- [Установка](#install)
- [Инициализация](#init)
- [Использование](#usage)
- [Пример](#example)
- [Версии](#versions)
- [Баги и обратная связь](#feedback)

<a id="install"></a>
## Установка
- Библиотеку можно найти по названию **GyverTimers** и установить через менеджер библиотек в:
    - Arduino IDE
    - Arduino IDE v2
    - PlatformIO
- [Скачать библиотеку](https://github.com/GyverLibs/GyverTimers/archive/refs/heads/main.zip) .zip архивом для ручной установки:
    - Распаковать и положить в *C:\Program Files (x86)\Arduino\libraries* (Windows x64)
    - Распаковать и положить в *C:\Program Files\Arduino\libraries* (Windows x32)
    - Распаковать и положить в *Документы/Arduino/libraries/*
    - (Arduino IDE) автоматическая установка из .zip: *Скетч/Подключить библиотеку/Добавить .ZIP библиотеку…* и указать скачанный архив
- Читай более подробную инструкцию по установке библиотек [здесь](https://alexgyver.ru/arduino-first/#%D0%A3%D1%81%D1%82%D0%B0%D0%BD%D0%BE%D0%B2%D0%BA%D0%B0_%D0%B1%D0%B8%D0%B1%D0%BB%D0%B8%D0%BE%D1%82%D0%B5%D0%BA)
### Обновление
- Рекомендую всегда обновлять библиотеку: в новых версиях исправляются ошибки и баги, а также проводится оптимизация и добавляются новые фичи
- Через менеджер библиотек IDE: найти библиотеку как при установке и нажать "Обновить"
- Вручную: **удалить папку со старой версией**, а затем положить на её место новую. "Замену" делать нельзя: иногда в новых версиях удаляются файлы, которые останутся при замене и могут привести к ошибкам!


<a id="init"></a>
## Инициализация
Нет

<a id="usage"></a>
## Использование
```cpp
Методы для Timer0, Timer1, Timer2...
uint32_t setPeriod(период);              // установка периода в микросекундах и запуск таймера. Возвращает реальный период (точность ограничена разрешением таймера).
uint32_t setFrequency(частота);          // установка частоты в Герцах и запуск таймера. Возвращает реальную частоту (точность ограничена разрешением таймера).
float setFrequencyFloat(частота float);  // установка частоты в Герцах и запуск таймера, разрешены десятичные дроби. Возвращает реальную частоту (точность ограничена разрешением таймера).
void enableISR(источник);                // включить прерывания, канал прерываний CHANNEL_A или CHANNEL_B (+ CHANNEL_C у Mega2560)
void disableISR(источник);               // выключить прерывания, канал CHANNEL_A или CHANNEL_B. Счёт таймера не останавливается (без указания параметров будет выключен канал А).
void pause();                            // приостановить счёт таймера, не сбрасывая счётчик
void resume();                           // продолжить счёт после паузы
void stop();                             // остановить счёт и сбросить счётчик
void restart();                          // перезапустить таймер (сбросить счётчик)
void setDefault();                       // установить параметры таймера по умолчанию ("Ардуино-умолчания")
void outputEnable(канал, режим);         // канал: включить выход таймера CHANNEL_A или CHANNEL_B (+ CHANNEL_C у Mega2560). Режим: TOGGLE_PIN, CLEAR_PIN, SET_PIN (переключить/выключить/включить пин по прерыванию)
void outputDisable(канал);               // отключить выход таймера CHANNEL_A или CHANNEL_B (+ CHANNEL_C у Mega2560, см. такблицу таймеров)
void outputState(канал, состояние);      // сменить состояние канала: HIGH / LOW
void phaseShift(источник, фаза);         // сдвинуть фазу канала на 0-360 градусов (у 8 бит таймеров двигается только канал B)
bool ready(uint8_t channel);             // возвращает true, если произошло прерывание на указанном канале
bool ready();                            // возвращает true, если произошло прерывание на канале A

/*
  Библиотека даёт прямой доступ к прерыванию без “Ардуиновских” attachInterrupt, что позволяет 
  сократить время вызова функции-обработчика прерывания. Прерывание с настроенной частотой 
  будет обрабатываться в блоке вида ISR(канал) {}, пример:
*/
ISR(TIMER1_A) {
  // ваш код
}

ISR(TIMER1_B) {
  // ваш код
}

ISR(TIMER2_B) {
  // ваш код
}

ISR(TIMER0_A) {
  // ваш код
}

/*
-------------------------------- Arduino NANO 16 МГц (ATmega328) ------------------------------------
Таймер	| Разрядность	| Частоты			| Периоды			| Выходы	| Пин Arduino	| Пин МК|
--------|---------------|-------------------|-------------------|-----------|---------------|-------|
Timer0	| 8 бит			| 61 Гц - 1 МГц		| 16 384.. 1 мкс	| CHANNEL_A	| D6			| PD6	|
        | 				| 					| 					| CHANNEL_B	| D5			| PD5	|
--------|---------------|-------------------|-------------------|-----------|---------------|-------|
Timer1	| 16 бит		| 0.24 Гц - 1 МГц	| 4 200 000.. 1 мкс	| CHANNEL_A	| D9			| PB1	|
        | 				| 					| 					| CHANNEL_B	| D10			| PB2	|
--------|---------------|-------------------|-------------------|-----------|---------------|-------|
Timer2	| 8 бит			| 61 Гц - 1 МГц		| 16 384.. 1 мкс	| CHANNEL_A	| D11			| PB3	|
        | 				| 					| 					| CHANNEL_B	| D3			| PD3	|
----------------------------------------------------------------------------------------------------
                        
------------------------------ Arduino MEGA 16 МГц (ATmega2560) -------------------------------------
Таймер	| Разрядность	| Частоты			| Периоды			| Выходы	| Пин Arduino	| Пин МК|
--------|---------------|-------------------|-------------------|-----------|---------------|-------|
Timer0	| 8 бит			| 61 Гц - 1 МГц		| 16 384.. 1 мкс	| CHANNEL_A	| 13			| PB7	|
        | 				| 					| 					| CHANNEL_B	| 4				| PG5	|
--------|---------------|-------------------|-------------------|-----------|---------------|-------|
Timer1	| 16 бит		| 0.24 Гц - 1 МГц	| 4 200 000.. 1 мкс	| CHANNEL_A	| 11			| PB5	|
        | 				| 					| 					| CHANNEL_B	| 12			| PB6	|
        | 				| 					| 					| CHANNEL_C	| 13			| PB7	|
--------|---------------|-------------------|-------------------|-----------|---------------|-------|
Timer2	| 8 бит			| 61 Гц - 1 МГц		| 16 384.. 1 мкс	| CHANNEL_A	| 10			| PB4	|
        | 				| 					| 					| CHANNEL_B	| 9				| PH6	|
--------|---------------|-------------------|-------------------|-----------|---------------|-------|
Timer3	| 16 бит		| 0.24 Гц - 1 МГц	| 4 200 000.. 1 мкс	| CHANNEL_A	| 5				| PE3	|
        | 				| 					| 					| CHANNEL_B	| 2				| PE4	|
        | 				| 					| 					| CHANNEL_C	| 3				| PE5	|
--------|---------------|-------------------|-------------------|-----------|---------------|-------|
Timer4	| 16 бит		| 0.24 Гц - 1 МГц	| 4 200 000.. 1 мкс	| CHANNEL_A	| 6				| PH3	|
        | 				| 					| 					| CHANNEL_B	| 7				| PH4	|
        | 				| 					| 					| CHANNEL_C	| 8				| PH5	|
--------|---------------|-------------------|-------------------|-----------|---------------|-------|
Timer5	| 16 бит		| 0.24 Гц - 1 МГц	| 4 200 000.. 1 мкс	| CHANNEL_A	| 46			| PL3	|
        | 				| 					| 					| CHANNEL_B	| 45			| PL4	|
        | 				| 					| 					| CHANNEL_C	| 44			| PL5	|
----------------------------------------------------------------------------------------------------
*/
```

<a id="example"></a>
## Пример
Остальные примеры смотри в **examples**!
```cpp
// Демонстрация всех функций библиотеки

#include <GyverTimers.h>

void setup() {
  // Перенастроить таймер и задать ему период или частоту
  // Все функции возвращают реальный период / частоту, которые могут отличаться от введенных
  Timer2.setPeriod(1000);           // Задать конкретный период 1000 мкс (~ 1000 гц), вернет реальный период в мкс
  Timer0.setFrequency(250);         // Задать частоту прерываний таймера в Гц, вернет реальную частоту в герцах
  Timer1.setFrequencyFloat(50.20);  // Задать частоту более точно, в дробных числах, актуально для низких частот и таймера 1
  // С этого момента таймер уже перенастроен и гоняет с выьранной частотой / периодом

  // Подключить прерывание таймера, с этого момента прерывания начнут вызываться
  Timer0.enableISR();               // Подключить стандартное прерывание, канал А, без сдига фаз
  Timer2.enableISR(CHANNEL_B); // Подключить прерывание таймера 2, канал B
  Timer1.enableISR(CHANNEL_A);  // Подключить прерывание канала А
  Timer1.enableISR(CHANNEL_B); // Подключить второе прерывание таймера 1
  // Прерывание уже начнет вызываться

  // Если вдруг прерывание нужно отключить, не останавливая таймер
  Timer1.disableISR(CHANNEL_B);
  // С этого момента прерывание B больше не будет вызываться

  // Если нужно приостановить таймер ПОЛНОСТЬЮ, аппаратно
  Timer2.pause();
  // С этого момента таймер стоит на месте, содержимое счетчика остается нетронутым

  // Теперь таймер можно вернуть в строй
  Timer2.resume();
  // Таймер продолжил считать с того же места

  // Если нужно полностью остановить таймер и сбросить содержимое счетчика
  Timer1.stop();
  // Таймер стоит, счетчик сброшен

  // Возвращаем таймер в строй
  Timer1.restart();
  // Таймер перезапущен, начал считать с начала

  // Если нужно вернуть стандартные Arduino - настройки таймера
  Timer0.setDefault();
  // Теперь таймер работает в станлартном режиме
}

// векторы прерываний
ISR(TIMER1_A) {
}

ISR(TIMER1_B) {
}

ISR(TIMER2_B) {
}

ISR(TIMER0_A) {
}

void loop() {

}
```

<a id="versions"></a>
## Версии
- v1.1 - исправлена ошибка в расчёте периодов
- v1.2 - код разбит на h и cpp
- v1.3 - поправлен незначительный баг
- v1.4 - исправлена таблица частот и периодов
- v1.5 - исправлен restart и resume
- v1.6 - phase shift вынесен отдельным методом
- v1.7 - поправлена документация
- v1.8 - исправлен баг с макс периодом
- v1.9 - исправлен баг с возвращаемым 2х периодом
- v1.10 - добавлен флаг ready 

<a id="feedback"></a>
## Баги и обратная связь
При нахождении багов создавайте **Issue**, а лучше сразу пишите на почту [alex@alexgyver.ru](mailto:alex@alexgyver.ru)  
Библиотека открыта для доработки и ваших **Pull Request**'ов!


При сообщении о багах или некорректной работе библиотеки нужно обязательно указывать:
- Версия библиотеки
- Какой используется МК
- Версия SDK (для ESP)
- Версия Arduino IDE
- Корректно ли работают ли встроенные примеры, в которых используются функции и конструкции, приводящие к багу в вашем коде
- Какой код загружался, какая работа от него ожидалась и как он работает в реальности
- В идеале приложить минимальный код, в котором наблюдается баг. Не полотно из тысячи строк, а минимальный код