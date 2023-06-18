HardwareSerial Serial2(PA3, PA2);//лог
HardwareSerial Serial3(PB11, PB10);//лора
void setup() {
  // put your setup code here, to run once:
  delay(500);

  Serial.begin(57600);
  delay(500);

  Serial2.begin(57600);
  delay(500);

  Serial3.begin(9600);
  delay(1000);
  Serial2.println("INA NOT START1");
  Serial3.println("INA NOT START1");
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(PC13, 0);
  delay(1000);
  digitalWrite(PC13,1);
  Serial2.println("INA NOT START1");
  Serial.println("INA NOT START1");
  delay(1000);
}
