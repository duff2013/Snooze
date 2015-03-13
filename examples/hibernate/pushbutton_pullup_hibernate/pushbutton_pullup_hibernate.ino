/***************************************
 * hibernate with pushbutton w/ pullup.
 * expect IDD of around 15uA for (Teensy 3.x)
 * and IDD of around 4uA (Teensy LC). Must
 * uncomment #define USE_HIBERNATE in Snooze.h.
 *
 * While hibernating you will not be able
 * to reprogram so use deepSleep until you
 * have sleeping working the way you want.
 *
 * If having trouble reprogramming while
 * using hibernate:
 * 1. Unplug USB, and, or disconnect power.
 * 2. While powered down hold reprogram button.
 * 3. Plug in USB, and, or apply power.
 * 4. Release program button, should program now
 ****************************************/

#include <Snooze.h>

SnoozeBlock config;

void setup() {
    pinMode(LED_BUILTIN, OUTPUT);
    /********************************************************
     * Define digital pins for waking the teensy up. This
     * combines pinMode and attachInterrupt in one function.
     *
     * Teensy 3.x
     * Digtal pins: 2,4,6,7,9,10,11,13,16,21,22,26,30,33
     *
     * Teensy LC
     * Digtal pins: 2,6,7,9,10,11,16,21,22
     ********************************************************/
    config.pinMode(21, INPUT_PULLUP, RISING);//pin, mode, type
    config.pinMode(22, INPUT_PULLUP, RISING);//pin, mode, type
    
    /********************************************************
     * Set Low Power Timer wake up in milliseconds.
     ********************************************************/
    config.setTimer(5000);// milliseconds
    
    /********************************************************
     * Teensy 3.x only currently.
     *
     * Set RTC alarm wake up in (hours, minutes, seconds).
     ********************************************************/
#ifdef KINETISK
    //config.setAlarm(0, 0, 10);// hour, min, sec
#endif
    
    /********************************************************
     * Values greater or less than threshold will trigger CMP
     * wakeup. Threshold value is in volts (0-3.3v) using a 64
     * tap resistor ladder network at 0.0515625v per tap.
     *
     * parameter "type": LOW & FALLING are the same.
     * parameter "type": HIGH & RISING are the same.
     *
     * Teensy 3.x/LC
     * Compare pins: 11,12
     * not quite working right yet!!!
     ********************************************************/
    // trigger at threshold values greater than 1.65v
    //config.pinMode(11, CMP, HIGH, 1.65);//pin, mode, type, threshold(v)
    // trigger at threshold values less than 1.65v
    config.pinMode(11, CMP, LOW, 1.65);//pin, mode, type, threshold(v)
    
    /********************************************************
     * Values greater than threshold will trigger TSI wakeup.
     * Threshold value is in capacitance. Only one pin can be
     * used while sleeping.
     *
     * Teensy 3.x
     * Touch Sense pins: 0,1,15,16,17,18,19,22,23,25,32,33
     *
     * Teensy LC
     * Touch Sense pins: 0,1,3,4,15,16,17,18,19,22,23
     ********************************************************/
    config.pinMode(0, TSI, touchRead(0) + 250); // pin, mode, threshold
}


void loop() {
    /********************************************************
     * feed the sleep function its wakeup parameters. Then go
     * to deepSleep.
     ********************************************************/
    Snooze.hibernate( config );
    
    int who = Snooze.source( );// get wakeup source
    
    if (who == 21) { // pin wakeup source is its pin value
        for (int i = 0; i < 1; i++) {
            digitalWrite(LED_BUILTIN, HIGH);
            delay(200);
            digitalWrite(LED_BUILTIN, LOW);
            delay(200);
        }
    }
    
    if (who == 22) { // pin wakeup source is its pin value
        for (int i = 0; i < 2; i++) {
            digitalWrite(LED_BUILTIN, HIGH);
            delay(200);
            digitalWrite(LED_BUILTIN, LOW);
            delay(200);
        }
    }
    
    if (who == 34) { // compare wakeup value
        for (int i = 0; i < 3; i++) {
            digitalWrite(LED_BUILTIN, HIGH);
            delay(200);
            digitalWrite(LED_BUILTIN, LOW);
            delay(200);
        }
    }
    
    if (who == 35) { // rtc wakeup value
        for (int i = 0; i < 4; i++) {
            digitalWrite(LED_BUILTIN, HIGH);
            delay(200);
            digitalWrite(LED_BUILTIN, LOW);
            delay(200);
        }
    }
    
    if (who == 36) { // lptmr wakeup value
        for (int i = 0; i < 5; i++) {
            digitalWrite(LED_BUILTIN, HIGH);
            delay(200);
            digitalWrite(LED_BUILTIN, LOW);
            delay(200);
        }
    }
    
    if (who == 37) { // tsi wakeup value
        for (int i = 0; i < 6; i++) {
            digitalWrite(LED_BUILTIN, HIGH);
            delay(200);
            digitalWrite(LED_BUILTIN, LOW);
            delay(200);
        }
    }
}