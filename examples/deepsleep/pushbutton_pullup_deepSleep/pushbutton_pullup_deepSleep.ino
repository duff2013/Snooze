/***************************************
* deepSleep with pushbutton w/ pullup. 
* expect IDD of around 230uA.
****************************************/
#include <Snooze.h>

/*** Must be global ***/
SnoozeBlock config;

void setup() {
  delay(1000);
  pinMode(LED_BUILTIN, OUTPUT);
  /****************************
  * setup the configuration
  * parameters for waking the
  * teensy up.
  *
  * Only pins:
  * 2,4,6,7,9,10,11,13,16,21,22,26,30,33
  * can be used as wakeup pins during
  * deepSleep.
  *
  * This will configure the pin like
  * the normal pinMode will.
  *****************************/
  config.pinMode(16, INPUT_PULLUP, RISING);
}


void loop() {
  /************************
  * feed the sleep function
  * its wakeup parameters.
  * Since USB is disabled
  * blink the led to indicate
  * a button was pushed and 
  * woke up the mcu.
  *************************/
  Snooze.deepSleep( config );
  digitalWrite(LED_BUILTIN, HIGH);
  delay(1000);
  digitalWrite(LED_BUILTIN, LOW);
}
  

