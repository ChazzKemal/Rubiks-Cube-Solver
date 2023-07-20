// Pin Definitions
#define Xpin A0
#define Ypin A1
#define Spin 2

#define enB A4
#define stepPinB 9

#define enL 5
#define stepPinL 10

#define enF 3
#define stepPinF 8

#define enR 6
#define stepPinR 11

#define enD 4
#define stepPinD 12

#define enU A5
#define stepPinU 7

// Constants
#define STEPS_PER_REVOLUTION 200
#define SPEED_VARIABLE 1350

// Global Variables
String commands;
int Xval;
int Yval;
int Sval = 1;
char rec;
int L;
int bookmark = 0;
volatile bool interrupted = false;
const String samp = "L3 U1 B1 R2 F3 L1 F3 U2 L1 U3 B3 U2 B1 L2 F1 U2 R2 L2 B2 (19f)\n"; // TEST

void setup() {
  // Serial communication setup
  Serial.begin(9600);
  Serial.println("");

  // Pin mode setup
  pinMode(Xpin, INPUT);
  pinMode(Ypin, INPUT);
  pinMode(Spin, INPUT);
  digitalWrite(Spin, HIGH);
  pinMode(13, OUTPUT); // TEST: LED connected to pin 13

  // Motor pin mode setup
  pinMode(stepPinL, OUTPUT);
  pinMode(enL, OUTPUT);
  pinMode(stepPinR, OUTPUT);
  pinMode(enR, OUTPUT);
  pinMode(stepPinD, OUTPUT);
  pinMode(enD, OUTPUT);
  pinMode(stepPinU, OUTPUT);
  pinMode(enU, OUTPUT);
  pinMode(stepPinF, OUTPUT);
  pinMode(enF, OUTPUT);
  pinMode(stepPinB, OUTPUT);
  pinMode(enB, OUTPUT);

  // Interrupt setup
  attachInterrupt(digitalPinToInterrupt(Spin), myISR, FALLING);

  // Motor enable pin setup
  digitalWrite(enL, HIGH);
  digitalWrite(enR, HIGH);
  digitalWrite(enD, HIGH);
  digitalWrite(enU, HIGH);
  digitalWrite(enF, HIGH);
  digitalWrite(enB, HIGH);
}

void loop() {
  digitalWrite(13, LOW); // TEST

  // Serial communication
  if (Serial.available() > 0) {
    while (Serial.available() > 0) {
      delay(10);
      rec = Serial.read();
      commands += rec;
      if (rec == '\n') {
        if (commands.equals(samp)) {
          digitalWrite(13, HIGH); // TEST
          delay(2000); // TEST
          digitalWrite(13, LOW); // TEST
        }
      }
    }
    bookmark = 0; // Reset bookmark when a new string is received
    delay(1000);
  }

  // If there is a string to work with, run the algorithm
  if (commands.length() > 1) {
    interrupted = false; // Default to no interruption
    L = commands.length() / 3; // Index of the last command in the string

    // Slow mode
    while (!interrupted) {
      Xval = analogRead(Xpin);
      Yval = analogRead(Ypin);
      Sval = digitalRead(Spin);
      if (Yval > 900 && bookmark < L) {
        onemove();
        bookmark++;
        Serial.println(bookmark); // TEST
      }
      else if (Yval < 25 && bookmark > 0) {
        undo();
        bookmark--;
        Serial.println(bookmark); // TEST
      }
    }

    // Wait until the button is released
    while (digitalRead(Spin) == 1) {
      delay(1);
    }
    delay(1000);
    interrupted = false;  // Interrupt triggered, undo this effect

    // Rapid mode
    while (bookmark < L && !interrupted) {
      onemove();
      bookmark++;
    }
    delay(1000);
  }
}

void onemove() {
  char a = commands[bookmark * 3];
  int b = commands[bookmark * 3 + 1] - '0';
  digitalWrite(13, HIGH); // TEST
  startmotor(a, b);
  digitalWrite(13, LOW); // TEST
  delay(100); // TEST
}

void undo() {
  char a = commands[bookmark * 3 - 3];
  int b = 4 - (commands[bookmark * 3 - 2] - '0');
  digitalWrite(13, HIGH); // TEST
  startmotor(a, b);
  digitalWrite(13, LOW); // TEST
  delay(300); // TEST
}

void startmotor(char which, int delta) {
  switch (which) {
    case 'L':
      runmotor(stepPinL, enL, delta);
      break;
    case 'R':
      runmotor(stepPinR, enR, delta);
      break;
    case 'D':
      runmotor(stepPinD, enD, delta);
      break;
    case 'U':
      runmotor(stepPinU, enU, delta);
      break;
    case 'F':
      runmotor(stepPinF, enF, delta);
      break;
    case 'B':
      runmotor(stepPinB, enB, delta);
      break;
    default:
      break;
  }
}

void runmotor(int pin, int enpin, int delta) {
  digitalWrite(enpin, LOW);
  delay(100);
  for (int i = 0; i < delta * (STEPS_PER_REVOLUTION / 4); i++) {
    digitalWrite(pin, HIGH);
    delayMicroseconds(SPEED_VARIABLE);
    digitalWrite(pin, LOW);
    delayMicroseconds(SPEED_VARIABLE);
  }
  Serial.print("Motor run: "); // TEST
  Serial.println(pin); // TEST
  Serial.print("Amount: "); // TEST
  Serial.println(delta); // TEST
  digitalWrite(enpin, HIGH);
}

void myISR() {
  interrupted = true;
}
