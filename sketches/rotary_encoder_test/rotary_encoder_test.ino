#include <Arduino.h>
#include <TM1637Display.h>

//7_SEGMENT_CLOCK
#define CLK_CLOCK 8
#define DIO_CLOCK 9

TM1637Display display(CLK_CLOCK, DIO_CLOCK);

uint8_t digit_0 = 0x3f;
uint8_t digit_1 = 0x06;
uint8_t digit_2 = 0x5b;
uint8_t digit_3 = 0x4f;
uint8_t digit_4 = 0x66;
uint8_t digit_5 = 0x6d;
uint8_t digit_6 = 0x7d;
uint8_t digit_7 = 0x07;
uint8_t digit_8 = 0x7f;
uint8_t digit_9 = 0x6f;
uint8_t segment_code_line = digit_8 - digit_0;

uint8_t points = 0x40;
uint8_t no_points = 0x80;

uint8_t clearDisplay[] = {0x00, 0x00, 0x00, 0x00};

//ROTARY ENCODER
//pins
#define DT_ROTARY_ENCODER 6
#define CLK_ROTARY_ENCODER 5
#define SW_ROTARY_ENCODER 7
//variables
int rotaryEncoderPos = 0;
int lastPos = LOW;
int n = LOW;
int button = LOW;
int lastButton = LOW;

void setup() {
  //set the rotary encoder inputs
  pinMode(CLK_ROTARY_ENCODER, INPUT_PULLUP);
  pinMode(DT_ROTARY_ENCODER, INPUT_PULLUP);
  pinMode(SW_ROTARY_ENCODER, INPUT_PULLUP);

  //set the clock display to max brightness
  display.setBrightness(0xff);
}

void loop() {
  //time encoded by rotary encoder
  getTimeSetterSignal();
  
  //low delay for the rotary encoder (greater delays won't work good)
  delay(1);
}

void getTimeSetterSignal() {
  n = digitalRead(CLK_ROTARY_ENCODER);
  button = !digitalRead(SW_ROTARY_ENCODER);
  
  if (button != lastButton) {
    lastButton = button;
  }

  boolean updateClock = false;
  if ((lastPos == LOW) && (n == HIGH)) {
    if (digitalRead(DT_ROTARY_ENCODER) == LOW) {
      rotaryEncoderPos++;
      updateClock = true;
    }
    else {
      rotaryEncoderPos--;
      updateClock = true;
    }
  }
  lastPos = n;
  
  if (updateClock) {
    updateClock = false;
    updateClockDisplay();
  }
}

//calculate the current time setting by the encoder position
int getTimeSet(int encoderPos) {
  const int maxTime = 24 * 60;
  int timeSet = encoderPos * 5;//5 minute steps
  
  //stay in 24 hour range
  while (timeSet < 0) {
    timeSet += maxTime;
  }
  timeSet = timeSet % maxTime;
  
  int hours = timeSet / 60;
  int minutes = timeSet % 60;

  //convert to string-like clock code
  timeSet = hours * 100 + minutes;

  return timeSet;
}

void updateClockDisplay() {
  int timeSet = getTimeSet(rotaryEncoderPos);
  display.showNumberDecEx(timeSet, points, true);//set the time (with clock points)
}
