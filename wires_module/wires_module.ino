#define RED 1
#define YELLOW 2
#define BLUE 3
#define WHITE 4
#define BLACK 5

#include <Wire.h>

void printArray(int arr[], int len) {
  Serial.print('[');
  for (int i=0; i<len; i++) {
    Serial.print(arr[i]);
    Serial.print(',');
  }
  Serial.print("]\n");
}

const int LEN = 6;
const char initial[] = {YELLOW, WHITE, NULL, NULL, RED, BLACK};
int curr[LEN];
const String SERIAL_NO = "AL5QF2";
int gameState = 0;
int answer;

int find(const char arr[], int len, int index) {
  int cur = -1;
  int i=0;

  do {
    if (arr[i]) cur++;
    Serial.println('r');
    i++;
  } while (cur < index);
  return i;
}

int countWires(const char arr[], int len, int counts[]) {
  int count = 0;

  for (int i=0; i<len; i++)
    if (arr[i]) {
      count++;
      counts[arr[i]]++;
    }
  
  return count;
}

bool serialOdd(String serial) {
  return (serial[serial.length()-1] - '0') % 2 == 1;
}

int solve(const char arr[], int len) {
  int counts[6] = {0};
  int count = countWires(arr, len, counts);
  
  int mapping[count];
  for (int i=0, j=0; i<len; i++)
    if (arr[i]) {
      mapping[j] = i;
      j++;
    }
  
  int answer = -1;
  switch (count) {
    case 3:
      if (counts[RED] == 0)
        answer = mapping[1];
      else if (arr[mapping[count-1]] == WHITE)
        answer = mapping[count-1];
      else if (counts[BLUE] > 1) {
        // find last blue wire
        for (int i=count-1; i>=0; i++) {
          if (arr[mapping[i]] == BLUE)
            answer = mapping[i];
            break;
        }
      } else
        answer = mapping[count-1];
      break;
    case 4:
      if (counts[RED] > 1 && serialOdd(SERIAL_NO)) {
        // find last red wire
        for (int i=count-1; i>=0; i++) {
          if (arr[mapping[i]] == RED)
            answer = mapping[i];
            break;
        }
      } else if (arr[mapping[count-1]] == YELLOW && counts[RED] == 1)
        answer = mapping[0];
      else if (counts[BLUE] == 1)
        answer = mapping[0];
      else if (counts[YELLOW] > 1)
        answer = mapping[count-1];
      else
        answer = mapping[1];
      break;
    case 5:
      if (arr[mapping[count-1]] == BLACK && serialOdd(SERIAL_NO))
        answer = mapping[3];
      else if (counts[RED] == 1 && counts[YELLOW] > 1)
        answer = mapping[0];
      else if (counts[BLACK] == 0)
        answer = mapping[1];
      else
        answer = mapping[0];
      break;
    case 6:
      if (counts[YELLOW] == 0 && serialOdd(SERIAL_NO))
        answer = mapping[2];
      else if (counts[YELLOW] == 1 && counts[WHITE] > 1)
        answer = mapping[3];
      else if (counts[RED] == 1)
        answer = mapping[count-1];
      else
        answer = mapping[3];
      break;
  }
  
  return answer;
}

int change(int prev[], int curr[], int len) {
  int diff = -1;

  for (int i=0; i<len; i++) {
    if (prev[i] != 0 && curr[i] == 0)
      diff = i;
  }

  return diff;
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  
  for (int i=2; i<=7; i++)
    pinMode(i, INPUT);
  pinMode(8, INPUT);

  for (int i=0; i<LEN; i++) {
    curr[i] = initial[i] ? 1 : 0;
  }
  answer = solve(initial, LEN);
  Serial.println(answer);

  Wire.begin(9);
  Wire.onRequest(request);
  Wire.onReceive(receive);
}

int timer = 0;
char signal_in = 0;
char signal_out = 'i';

char getSignalIn() {
  char c = signal_in;
  signal_in = 0;
  return c;
}

char getSignalOut() {
  char c = signal_out;

  if (gameState == 0)
    signal_out = 'i';
  else if (gameState == 1)
    signal_out = 'c';
  return c;
}

void request(int len) {
  Wire.write(getSignalOut());
}

void receive(int len) {
  if (Wire.available())
    signal_in = Wire.read();
}

void loop() {
  // put your main code here, to run repeatedly:
  int state[6];
  for (int i=0; i<LEN; i++)
    state[i] = digitalRead(i+2);
  // printArray(state, LEN);
  
  if (gameState == 0) {
    if (getSignalIn() == 's') {
      Serial.println("start signal received");
      bool check = true;
      for (int i=0; i<LEN; i++)
        if ((state[i] != 0 && initial[i] == 0) || (state[i] == 0 && initial[i] != 0))
          check = false;
      
      if (check) {
        gameState = 1;
        signal_out = 'c';
        Serial.println("yeyeee");
      } else {
        Serial.println("bro ur not ready");
      }
    }
  } else if (gameState == 1) {
    int wire = change(curr, state, LEN);
    if (wire != -1) {
      if (timer < 3) {
        timer++;
      }
    } else
      timer = 0;
    
    if (timer == 3) {
      if (wire == answer) {
        memcpy(curr, state, sizeof(curr));
        gameState = 2;
        signal_out = 's';
      } else {
        memcpy(curr, state, sizeof(curr));
        signal_out = 'x';
      }
    }
  }

  if (gameState == 2)
    digitalWrite(13, true);
  else
    digitalWrite(13, false);
  
  delay(10);
}
