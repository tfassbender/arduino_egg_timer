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
uint8_t zeroDisplay[] = {0x3f, 0x3f, 0x3f, 0x3f};

//ROTARY ENCODER
//pins
#define DT_ROTARY_ENCODER 6
#define CLK_ROTARY_ENCODER 5
#define SW_ROTARY_ENCODER 7
//variables
int rotaryEncoderPos = 1;
int lastPos = LOW;
int n = LOW;
int button = LOW;
int lastButton = LOW;

//PASSIVE BUZZER
#define BUZZER_PIN 4
const int buzzer_sound_length = 50;
const int buzzer_sound_pause_short = 100;
const int buzzer_sound_pause_long = 1000;
const int buzzer_sound_repetitions = 4;
const int buzzer_frequency = 2000;

unsigned long previousMillis = 0;
int buzzer_interval = 0;
int buzzer_state = 0;//states: 0 -> before start; 1 -> sound on (first time); 2 -> sound off (first time); 3 -> sound on (second time); ...; 9 -> sound off long

//CONTROLL BUTTONS (start/stop, reset)
#define START_STOP_BUTTON_PIN 2
#define RESET_BUTTON_PIN 3
#define DEBOUNCE_TIME 750
unsigned long start_stop_debounce;
unsigned long reset_debounce;

//VARIABLES for controlling the countdown clock
boolean counting = false;
boolean alarm_playing = false;
unsigned long time = 0;//the current time in seconds
unsigned long start_time = 0;//the start time of the counter (changes during countdown)
const unsigned long time_count_interval = 1000;//inverval of 1 second 

void setup() {
  //set the rotary encoder inputs
  pinMode(CLK_ROTARY_ENCODER, INPUT_PULLUP);
  pinMode(DT_ROTARY_ENCODER, INPUT_PULLUP);
  pinMode(SW_ROTARY_ENCODER, INPUT_PULLUP);
  
  //set the buzzer pin as output
  pinMode(BUZZER_PIN, OUTPUT);

  //set the clock display to max brightness
  display.setBrightness(0xff);
  
  //set the button pins to input
  pinMode(START_STOP_BUTTON_PIN, INPUT);
  pinMode(RESET_BUTTON_PIN, INPUT);
}

void loop() {
  //time encoded by rotary encoder
  getTimeSetterSignal();
  
  //check for buttons pressed
  handleButtons();
  
  //count down the time (if the timer is running)
  handleTimer();
  
  //handle the alarm sound
  handleAlarm();
  
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
  
  if (!counting && !alarm_playing && updateClock) {
    updateClock = false;
    updateClockDisplay();
  }
}

void handleButtons() {
  if (digitalRead(START_STOP_BUTTON_PIN) == HIGH && millis() - start_stop_debounce >= DEBOUNCE_TIME) {
    if (alarm_playing) {
      stopAlarm();
    }
    else if (time > 0 || rotaryEncoderPos > 0) {
      counting = !counting;
      if (counting) {
        start_time = millis();
        if (time == 0 && rotaryEncoderPos > 0) {
          time = getTimeSeconds(rotaryEncoderPos);
        }
      }
    }
    
    start_stop_debounce = millis();
  }
  
  if (digitalRead(RESET_BUTTON_PIN) == HIGH && millis() - reset_debounce >= DEBOUNCE_TIME) {
    if (alarm_playing) {
      stopAlarm();
    }
    
    if (!counting) {
      resetRotaryEncoder();
      time = 0;
    }
    
    reset_debounce = millis();
  }
}

void handleTimer() {
  if (counting) {
    if (millis() - start_time >= time_count_interval) {
      start_time += time_count_interval;
      time--;
      
      updateDisplayedTime(time);
    }
  }
}

void handleAlarm() {
  if (counting) {
    if (time <= 0) {
      counting = false;
      alarm_playing = true;
    }
  }
  
  if (alarm_playing) {
    playAlarmSound();
  }
}

void playAlarmSound() {
  unsigned long currentMillis = millis();
  
  if (currentMillis - previousMillis >= buzzer_interval) {
    previousMillis = currentMillis;
    
    //states: 0 -> before start; 1 -> sound on (first time); 2 -> sound off (first time); 3 -> sound on (second time); ...; 9 -> sound off long
    switch (buzzer_state) {
      case 0://before start
      case 2://off (1)
      case 4://off (2)
      case 6://off (3)
        tone(BUZZER_PIN, buzzer_frequency);
        buzzer_interval = buzzer_sound_length;
        break;
      case 1://on (1)
      case 3://on (2)
      case 5://on (3)
      case 7://on (4)
        noTone(BUZZER_PIN);
        buzzer_interval = buzzer_sound_pause_short;
        break;
      case 8://off (4)
        noTone(BUZZER_PIN);
        buzzer_interval = buzzer_sound_pause_long;
        
        //turn off the display
        display.setSegments(clearDisplay);
        break;
      case 9://off long
        buzzer_interval = 0;
        buzzer_state = -1;
        
        //turn on the display
        display.setSegments(zeroDisplay);
        break;
    }
    
    buzzer_state++;

  }
}

int getTimeSeconds(int encoderPos) {
  // time (minutes)  -  step size  -  step count
  // 0 - 5           -  10s        -  0 - 30
  // 5 - 10          -  30s        -  30 - 40
  // 10 - 99         -  60s        -  40 - 129
  
  const int steps = 130;
  while (encoderPos < 0) {
    encoderPos += steps;
  }
  encoderPos = encoderPos % steps;
  
  int timeSet;
  if (encoderPos <= 30) {
    timeSet = encoderPos * 10;
  }
  else if (encoderPos <= 40) {
    timeSet = 300 + (encoderPos - 30) * 30;
  }
  else {
    timeSet = 600 + (encoderPos - 40) * 60;
  }
  
  return timeSet;
}

//calculate the current time setting by the encoder position
int getTimeSet(int encoderPos) {
  int timeSet = getTimeSeconds(encoderPos);
  
  int minutes = timeSet / 60;
  int seconds = timeSet % 60;

  //convert to string-like clock code
  timeSet = minutes * 100 + seconds;

  return timeSet;
}

void resetRotaryEncoder() {
  rotaryEncoderPos = 0;
  updateClockDisplay();
}

void stopAlarm() {
  alarm_playing = false;
  noTone(BUZZER_PIN);
  pinMode(BUZZER_PIN, INPUT_PULLUP);//set to input pullup, because otherwhise the delay generates a frequency
  
  resetRotaryEncoder();
}

void updateDisplayedTime(int time) {
  int minutes = time / 60;
  int seconds = time % 60;
  
  int timeDisplayed = minutes * 100 + seconds;
  
  display.showNumberDecEx(timeDisplayed, points, true);//set the time (with clock points)
}

void updateClockDisplay() {
  int timeSet = getTimeSet(rotaryEncoderPos);
  display.showNumberDecEx(timeSet, points, true);//set the time (with clock points)
}
