/***************************************
 * this shows all the wakeups
 * for deepSleep & hibernate. 
 * expect IDD of around 230uA.
 ****************************************/
#include <Snooze.h>

SnoozeBlock config;

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(1000);
  digitalWrite(LED_BUILTIN, LOW);
  /****************************
   * define the configuration 
   * parameters for waking the
   * teensy up.
   * Digtal pins: 2,4,6,7,9,10,11,13,16,21,22,26,30,33
   * Touch Sense pins: 0,1,15,16,17,18,19,22,23,25,32,33
   * Compare pins: 11,12
   *****************************/
  config.pinMode(21, INPUT_PULLUP, RISING);//pin, mode, type
  config.pinMode(22, INPUT_PULLUP, RISING);//pin, mode, type
  //config.setTimer(1000);// milliseconds
  config.setAlarm(0, 0, 10);// hour, min, sec
  config.pinMode(12, CMP, RISING, 1.123);//pin, mode, type, threshold
  config.pinMode(0, TSI, touchRead(0)+250);//pin, mode, threshold
}


void loop() {
  /************************
   * feed the sleep function
   * its wakeup parameters.
   *************************/
  Snooze.deepSleep( config );
  int who = Snooze.source( );// get wakeup source 
  
  if(who == 21) {// pin wakeup source is its pin value
    for(int i = 0; i < 1; i++) {
      digitalWrite(LED_BUILTIN, HIGH);
      delay(200);
      digitalWrite(LED_BUILTIN, LOW);
      delay(200);
    }
  }

  if(who == 22) {// pin wakeup source is its pin value
    for(int i = 0; i < 2; i++) {
      digitalWrite(LED_BUILTIN, HIGH);
      delay(200);
      digitalWrite(LED_BUILTIN, LOW);
      delay(200);
    }
  }

  if(who == 34) { // compare wakeup value
    for(int i = 0; i < 3; i++) {
      digitalWrite(LED_BUILTIN, HIGH);
      delay(200);
      digitalWrite(LED_BUILTIN, LOW);
      delay(200);
    }
  }

  if(who == 35) {// rtc wakeup value
    for(int i = 0; i < 4; i++) {
      digitalWrite(LED_BUILTIN, HIGH);
      delay(200);
      digitalWrite(LED_BUILTIN, LOW);
      delay(200);
    }
  }

  if(who == 36) {// lptmr wakeup value
    for(int i = 0; i < 5; i++) {
      digitalWrite(LED_BUILTIN, HIGH);
      delay(200);
      digitalWrite(LED_BUILTIN, LOW);
      delay(200);
    }
  }

  if(who == 37) {// tsi wakeup value
    for(int i = 0; i < 6; i++) {
      digitalWrite(LED_BUILTIN, HIGH);
      delay(200);
      digitalWrite(LED_BUILTIN, LOW);
      delay(200);
    }
  }
}









