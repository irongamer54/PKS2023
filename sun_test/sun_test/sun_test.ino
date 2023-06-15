#define F_PIN D5
#define B_PIN D6

#define SRV_PIN D7


void setup() {
  // put your setup code here, to run once:
  pinMode(F_PIN, OUTPUT);
  pinMode(B_PIN, OUTPUT);
  Serial.begin(115200);
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println("loop");
  digitalWrite(F_PIN,1);
  digitalWrite(B_PIN,0);
  delay(5000);
  digitalWrite(F_PIN,0);
  digitalWrite(B_PIN,0);
  delay(2000);
  digitalWrite(F_PIN,0);
  digitalWrite(B_PIN,1);
  delay(5000);
  digitalWrite(F_PIN,0);
  digitalWrite(B_PIN,0);
  delay(2000);
}
