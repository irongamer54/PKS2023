#include <Wire.h>
#include <Adafruit_INA219.h>

Adafruit_INA219 ina219;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  uint32_t frequency;
  delay(500);
}

void loop() {
  // put your main code here, to run repeatedly:
  float shuntVoltage = 0;
  float busVoltage = 0;
  float current_mA = 0;
  float loadVoltage = 0;
  float power_mW = 0;

  shuntVoltage = ina219.getShuntVoltage_mV();
  busVoltage = ina219.getBusVoltage_V();             // Получение значение напряжения V
  current_mA = ina219.getCurrent_mA();               // Получение значение тока в мА
  power_mW = ina219.getPower_mW();                   // Получение значение мощности
  loadVoltage = busVoltage + (shuntVoltage / 1000);  // Расчет напряжение на нагрузки

  // Поочередно отправляем полученные значение в последовательный порт.
  Serial.print("Bus Voltage:   ");
  Serial.print(busVoltage);
  Serial.println(" V");
  Serial.print("Shunt Voltage: ");
  Serial.print(shuntVoltage);
  Serial.println(" mV");
  Serial.print("Load Voltage:  ");
  Serial.print(loadVoltage);
  Serial.println(" V");
  Serial.print("Current:       ");
  Serial.print(current_mA);
  Serial.println(" mA");
  Serial.print("Power:         ");
  Serial.print(power_mW);
  Serial.println(" mW");
  Serial.println("");

  delay(2000);  // Пауза
}
