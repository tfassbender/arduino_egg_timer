const int buzzer = 9; //buzzer to arduino pin 9


void setup(){
 
  pinMode(buzzer, OUTPUT); // Set buzzer - pin 9 as an output

}

void loop(){
  int length = 50;
  int pause = 100;
  int long_pause = 1000;
  int freq = 2000;
  
  tone(buzzer, freq); // Send 1KHz sound signal...
  delay(length);        // ...for 1 sec
  noTone(buzzer);     // Stop sound...
  delay(pause);        // ...for 1sec
  
  tone(buzzer, freq); // Send 1KHz sound signal...
  delay(length);        // ...for 1 sec
  noTone(buzzer);     // Stop sound...
  delay(pause);        // ...for 1sec
  
  tone(buzzer, freq); // Send 1KHz sound signal...
  delay(length);        // ...for 1 sec
  noTone(buzzer);     // Stop sound...
  delay(pause);        // ...for 1sec
  
  tone(buzzer, freq); // Send 1KHz sound signal...
  delay(length);        // ...for 1 sec
  noTone(buzzer);     // Stop sound...
  delay(pause);        // ...for 1sec
  
  delay(long_pause);
}
