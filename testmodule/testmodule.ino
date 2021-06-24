
int startPin = 8;
int endPin = 9;
int sample1Pin = 2;
int sample2Pin = 3;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("start successful");
  pinMode(startPin, OUTPUT);
  pinMode(endPin, OUTPUT);
//  attachInterrupt(digitalPinToInterrupt(sample1Pin), startInt, CHANGE);
//  attachInterrupt(digitalPinToInterrupt(sample2Pin), endInt, CHANGE);
}
void startInt() {
  int a = digitalRead(sample1Pin);
  if (a == 0) {
    Serial.print(micros());
    Serial.println("startPin's level change from 1 to 0");
  } else {
    Serial.print(micros());
    Serial.println("startPin's level change from 0 to 1");
  }
}
void endInt() {
  int a = digitalRead(sample2Pin);
  if (a == 0) {
    Serial.print(micros());
    Serial.println("endPin's level change from 1 to 0");
  } else {
    Serial.print(micros());
    Serial.println("endPin's level change from 0 to 1");
  }
}
void loop() {
  // put your main code here, to run repeatedly:
  delay(2000);
  
  digitalWrite(startPin, HIGH);
  digitalWrite(endPin, HIGH);
  
  delayMicroseconds(10);
  Serial.print(micros());
  Serial.println("startPin output HIGH");
  delayMicroseconds(10);
  digitalWrite(startPin, LOW);
  Serial.print(micros());
  Serial.println("startPin output LOW");

  
  
  delayMicroseconds(10);
  Serial.print(micros());
  Serial.println("endPin output HIGH");
  delayMicroseconds(5);
  digitalWrite(endPin, LOW);
  Serial.println("endPin output LOW");

  digitalWrite(startPin, HIGH);
  digitalWrite(endPin, HIGH);

}
