#include <Arduino.h>
#include <TM1637Display.h>

// Module connection pins (Digital Pins)
#define CLK 3
#define DIO 2

// display settings and codes
TM1637Display display(CLK, DIO);

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

uint8_t points = 0x40;
uint8_t no_points = 0x80;

const uint8_t clearDisplay[] = {0x00, 0x00, 0x00, 0x00};

String inputText = "";

void setup()
{
  //set the clock display to max brightness
  display.setBrightness(0xff);
}

void loop()
{
  setClockTime("0842");
}

void setClockTime(String inputText) {
  //input form: "HHMM"
  
  String inputNumber = "";
  for (int i = 0; i < 4; i++) {
    inputNumber += inputText[i];
  }
  int clockTime = inputNumber.toInt();
  
  display.showNumberDecEx(clockTime, points, true);
}
