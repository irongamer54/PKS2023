
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);



}

void loop() {
  // put your main code here, to run repeatedly:
  uint32_t t1 = millis();

  int mgn = digitalRead(M);
  while (mgn == 1) {
    mgn = digitalRead(M);
  }
  

  uint32_t t2 = millis();
  double v;
  if (t2-t1!=0) {
   v = 2/((t2-t1)*0.001);

  }
  
  /*Serial.print("millis =");
  Serial.println(millis());

  Serial.print("mgn = ");
  Serial.println(mgn);
  */
  
  Serial.print("v = ");
  Serial.println(v);

  
  delay(10);

}