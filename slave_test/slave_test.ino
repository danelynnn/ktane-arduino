#include <Wire.h>

char status;
int state = 0;

void setup() {
  Serial.begin(9600);
  Wire.begin(9);
  Wire.onRequest(request);
  Wire.onReceive(receive);
}

void loop() {
  char c = Serial.read();
  if (c == '\n') {
    status = 'x';
  }

  Serial.println(state);
  delay(1000);
}

void request(int len) {
  if (status == 's') {
    Wire.write(status);
  } else if (status == 'x') {
    Wire.write(status);
    status = 0;
  } else {
    Wire.write('c');
  }
}

void receive(int len) {
  if (Wire.read() == 's') {
    Serial.println("received start");
    state = 1;
  }
}
