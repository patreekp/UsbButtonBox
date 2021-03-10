
#include <Joystick.h>

#define ENABLE_PULLUPS
#define NUMROTARIES 1

struct rotariesdef {
  byte pin1;
  byte pin2;
  int ccwchar;
  int cwchar;
  volatile unsigned char state;
};

rotariesdef rotaries[NUMROTARIES] {
  {2, 3, 0, 1, 0}
};

#define DIR_CCW 0x10
#define DIR_CW 0x20
#define R_START 0x0

#define R_CW_FINAL 0x1
#define R_CW_BEGIN 0x2
#define R_CW_NEXT 0x3
#define R_CCW_BEGIN 0x4
#define R_CCW_FINAL 0x5
#define R_CCW_NEXT 0x6
Joystick_ Joystick;
const unsigned char ttable[7][4] = {
  // R_START
  {R_START,    R_CW_BEGIN,  R_CCW_BEGIN, R_START},
  // R_CW_FINAL
  {R_CW_NEXT,  R_START,     R_CW_FINAL,  R_START | DIR_CW},
  // R_CW_BEGIN
  {R_CW_NEXT,  R_CW_BEGIN,  R_START,     R_START},
  // R_CW_NEXT
  {R_CW_NEXT,  R_CW_BEGIN,  R_CW_FINAL,  R_START},
  // R_CCW_BEGIN
  {R_CCW_NEXT, R_START,     R_CCW_BEGIN, R_START},
  // R_CCW_FINAL
  {R_CCW_NEXT, R_CCW_FINAL, R_START,     R_START | DIR_CCW},
  // R_CCW_NEXT
  {R_CCW_NEXT, R_CCW_FINAL, R_CCW_BEGIN, R_START},
};



static long baud = 9600;
static long newBaud = baud;

void lineCodingEvent(long baud, byte databits, byte parity, byte charFormat)
{
  newBaud = baud;
}

void rotary_init() {
  for (int i = 0; i < NUMROTARIES; i++) {
    pinMode(rotaries[i].pin1, INPUT);
    pinMode(rotaries[i].pin2, INPUT);
#ifdef ENABLE_PULLUPS
    digitalWrite(rotaries[i].pin1, HIGH);
    digitalWrite(rotaries[i].pin2, HIGH);
#endif
  }
}
void WriteToComputer() {
  while (Serial1.available()) {
    char c = (char)Serial1.read();
    Serial.write(c);
  }
}

unsigned char rotary_process(int _i) {
  unsigned char pinstate = (digitalRead(rotaries[_i].pin2) << 1) | digitalRead(rotaries[_i].pin1);
  rotaries[_i].state = ttable[rotaries[_i].state & 0xf][pinstate];
  return (rotaries[_i].state & 0x30);
}

void CheckAllEncoders(void) {
  for (int i = 0; i < NUMROTARIES; i++) {
    unsigned char result = rotary_process(i);
    if (result == DIR_CCW) {
      Joystick.setButton(rotaries[i].ccwchar, 1); delay(20); Joystick.setButton(rotaries[i].ccwchar, 0);
    };
    if (result == DIR_CW) {
      Joystick.setButton(rotaries[i].cwchar, 1); delay(20); Joystick.setButton(rotaries[i].cwchar, 0);
    };
  }
}

void setup() {

  pinMode(4, INPUT_PULLUP);
  pinMode(5, INPUT_PULLUP);
  pinMode(6, INPUT_PULLUP);
  pinMode(7, INPUT_PULLUP);
  pinMode(8, INPUT_PULLUP);
  pinMode(9, INPUT_PULLUP);
  pinMode(10, INPUT_PULLUP);
  pinMode(11, INPUT_PULLUP);
  pinMode(12, INPUT_PULLUP);
  pinMode(13, INPUT_PULLUP);
  
  Joystick.begin();
  rotary_init();

  //pinMode(LED_BUILTIN, OUTPUT);

  Serial.begin(baud);
  Serial1.begin(baud);
}

int readSize = 0;
int endofOutcomingMessageCount = 0;
int messageend = 0;
String command = "";

void UpdateBaudRate() {
  // Update baudrate if required
  newBaud = Serial.baud();
  if (newBaud != baud) {
    baud = newBaud;
    Serial1.end();
    Serial1.begin(baud);
  }
}


// Constant that maps the phyical pin to the joystick button.
const int pinToButtonMap = 2;

// Last state of the button
int lastButtonState[12] = {0,0,0,0,0,0,0,0,0,0,0,0};
void loop() {

  // Read pin values
  for (int index = 2; index < 12; index++)
  {
    int currentButtonState = !digitalRead(index + pinToButtonMap);
    if (currentButtonState != lastButtonState[index])
    {
      Joystick.setButton(index, currentButtonState);
      lastButtonState[index] = currentButtonState;
    }
  }
  CheckAllEncoders();

UpdateBaudRate();

  while (Serial.available()) {
    WriteToComputer();
        char c = (char)Serial.read();
    Serial1.write(c);}
}
