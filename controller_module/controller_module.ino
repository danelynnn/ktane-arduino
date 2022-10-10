#include <Wire.h>

bool last = false;
const int CONN_COUNT = 11;

char status[11] = {0};
int lives = 3;
int gameState = 0;

void printArray(char arr[], int len) {
  Serial.print(gameState);
  Serial.print(", [");
  for (int i=0; i<len; i++) {
    Serial.print(arr[i]);
    Serial.print(',');
  }
  Serial.print("], ");
  Serial.print(lives);
  Serial.print("\n");
}

void setup() {
  Serial.begin(9600);
  Wire.begin(8);
}

void loop() {
  char c;

  if (gameState == 0) {
    bool ready = true;
    int count = 0;
    for (int i=0; i<CONN_COUNT; i++) {
      Wire.requestFrom(i+9, 1);
      if (Wire.available()) {
        c = Wire.read();
        status[i] = c;

        if (c && c != 'c') {
          ready = false;
        }
        count++;
      }
    }

    if (ready && count) {
      gameState = 1;
    }
  } else if (gameState == 1) {
    for (int i=0; i<CONN_COUNT; i++) {
      if (status[i] != 's') {
        Wire.requestFrom(i+9, 1);
        if (Wire.available()) {
          c = Wire.read();
          
          switch (c) {
            case 'x':
              lives--;
              break;
            default:
              status[i] = c;
              break;
          }
        }
      }
    }
  }

  if (Serial.available() && Serial.read() == '\n') {
    for (int i=0; i<CONN_COUNT; i++) {
      Wire.beginTransmission(i+9);
      Wire.write('s');
      Wire.endTransmission(i+9);
    }

    Serial.println("start signal sent");
  }

  printArray(status, CONN_COUNT);
  delay(10);
}
