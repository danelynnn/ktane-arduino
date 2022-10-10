#include <Wire.h>

bool last = false;
const int CONN_COUNT = 11;

void setup() {
  Serial.begin(9600);
  Wire.begin(8);
}

void loop() {
  char c;
  for (int i=0; i<CONN_COUNT; i++) {
    Wire.requestFrom(i+9, 1);
    Serial.print('|');
    if (Wire.available()) {
      c = Wire.read();
      Serial.print(c);
    } else {
      Serial.print(' ');
    }
  }
  Serial.println('|');
  char i = Serial.read();
  if (i == '\n') {
    Serial.println("sending");
    Wire.beginTransmission(9);
    Wire.write('s');
    Wire.endTransmission(9);
  }
  delay(1000);
}
