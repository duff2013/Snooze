/***************************************
* Sleep with pushbutton w/ pullup. 
* expect IDD of around 900uA.
****************************************/
#include <Snooze.h>
 
SnoozeBlock config;

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  /****************************
  * define the configuration 
  * parameters for waking the
  * teensy up. 
  *****************************/
  config.pinMode(0, INPUT_PULLUP, RISING);
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
  Snooze.sleep( config );
  digitalWrite(LED_BUILTIN, HIGH);
  delay(100);
  digitalWrite(LED_BUILTIN, LOW);
}
  

