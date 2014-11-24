/***************************************
 * This shows all the wakeups
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
    /********************************************************
     * Define digital pins for waking the teensy up. This
     * combines pinMode and attachInterrupt in one function.
     *
     * Digtal pins: 2,4,6,7,9,10,11,13,16,21,22,26,30,33
     ********************************************************/
    config.pinMode(21, INPUT_PULLUP, RISING);//pin, mode, type
    config.pinMode(22, INPUT_PULLUP, RISING);//pin, mode, type
    
    /********************************************************
     * Set Low Power Timer wake up in milliseconds.
     ********************************************************/
    //config.setTimer(1000);// milliseconds
    
    /********************************************************
     * Set RTC alarm wake up in (hours, minutes, seconds).
     ********************************************************/
    config.setAlarm(0, 0, 10);// hour, min, sec
    
    /********************************************************
     * Values greater or less than threshold will trigger CMP 
     * wakeup. Threshold value is in volts (0-3.3v) using a 64 
     * tap resistor ladder network at 0.0515625v per tap.
     *
     * parameter "type": LOW & FALLING are the same.
     * parameter "type": HIGH & RISING are the same.
     *
     * Compare pins: 11,12
     ********************************************************/
    // trigger at threshold values greater than 1.65v
    config.pinMode(11, CMP, HIGH, 1.65);//pin, mode, type, threshold(v)
    // trigger at threshold values less than 1.65v
    //config.pinMode(11, CMP, LOW, 1.65);//pin, mode, type, threshold(v)
    
    /********************************************************
     * Values greater than threshold will trigger TSI wakeup.
     * Threshold value is in capacitance. Only one pin can be
     * used while sleeping.
     *
     * Touch Sense pins: 0,1,15,16,17,18,19,22,23,25,32,33
     ********************************************************/
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









