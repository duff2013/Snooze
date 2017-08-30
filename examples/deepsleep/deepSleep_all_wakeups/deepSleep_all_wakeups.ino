/***************************************
 This shows all the wakeups for deepSleep
 Expect IDD of  around 230uA (Teensy 3.x)
 and IDD of around 150uA for (Teensy LC).
 Teensy 3.5/3.6 are yet to be determined.
 ****************************************/
#include <Snooze.h>
// Load drivers
SnoozeDigital digital;
SnoozeCompare compare;
SnoozeTimer timer;
SnoozeTouch touch;
SnoozeAlarm	alarm;
// configures the lc's 5v data buffer (OUTPUT, LOW) for low power
Snoozelc5vBuffer  lc5vBuffer;
/***********************************************************
 Teensy 3.6/LC can't use Timer Driver with either Touch or
 Compare Drivers and Touch can't be used with Compare.
 
 Teensy 3.5 does not touch interface.
 
 Teensy LC does not have a rtc so Alarm driver can't be
 used as of yet.
 
 Teensy 3.2 can use any Core Drivers together.
 ***********************************************************/
#if defined(__MK66FX1M0__)
SnoozeBlock config_teensy36(touch, digital, alarm);
#elif defined(__MK64FX512__)
SnoozeBlock config_teensy35(digital, timer, compare);
#elif defined(__MK20DX256__)
SnoozeBlock config_teensy32(touch, digital, timer, compare);
#elif defined(__MK20DX128__)
SnoozeBlock config_teensy30(touch, digital, timer, compare);
#elif defined(__MKL26Z64__)
SnoozeBlock config_teensyLC(digital, timer, lc5vBuffer);
#endif

void setup() {
    pinMode(LED_BUILTIN, OUTPUT);
    /********************************************************
     Define digital pins for waking the teensy up. This
     combines pinMode and attachInterrupt in one function.
     
     Teensy 3.x
     Digtal pins: 2,4,6,7,9,10,11,13,16,21,22,26,30,33
     
     Teensy LC
     Digtal pins: 2,6,7,9,10,11,16,21,22
     ********************************************************/
    digital.pinMode(21, INPUT_PULLUP, RISING);//pin, mode, type
    digital.pinMode(22, INPUT_PULLUP, RISING);//pin, mode, type
    
    /********************************************************
     Teensy 3.x only currently.
     
     Set RTC alarm wake up in (hours, minutes, seconds).
     ********************************************************/
    alarm.setRtcTimer(0, 0, 10);// hour, min, sec
    
    /********************************************************
     Set Low Power Timer wake up in milliseconds.
     ********************************************************/
    timer.setTimer(5000);// milliseconds
    
    /********************************************************
     In deepSleep the Compare module works by setting the
     internal 6 bit DAC to a volatge threshold and monitors
     the pin volatge for a voltage crossing. The internal
     DAC uses a 64 tap resistor ladder network supplied by
     VOUT33 at 0.0515625v per tap (VOUT33/64). Thus the
     possible threshold voltages are 0.0515625*(0-64).
     
     parameter "type": LOW & FALLING are the same and have no effect.
     parameter "type": HIGH & RISING are the same and have no effect.
     
     Teensy 3.x
     Compare pins: 11,9,4
     
     Teensy LC
     Compare pins: 11
     ********************************************************/
    // trigger at threshold values greater than 1.65v
    //compare.pinMode(11, HIGH, 1.65);//pin, type, threshold(v)
    // trigger at threshold values less than 1.65v
    compare.pinMode(11, LOW, 1.65);//pin, type, threshold(v)
    
    /********************************************************
     Values greater than threshold will trigger TSI wakeup.
     Threshold value is in capacitance. Only one pin can be
     used while sleeping.
     
     Teensy 3.x
     Touch Sense pins: 0,1,15,16,17,18,19,22,23,25,32,33
     
     Teensy LC
     Touch Sense pins: 0,1,3,4,15,16,17,18,19,22,23
     ********************************************************/
    touch.pinMode(0, touchRead(0) + 250); // pin, threshold
}

void loop() {
    int who;
    /********************************************************
     feed the sleep function its wakeup parameters. Then go
     to deepSleep.
     ********************************************************/
#if defined(__MK66FX1M0__)
    who = Snooze.deepSleep( config_teensy36 );// return module that woke processor
#elif defined(__MK64FX512__)
    who = Snooze.deepSleep( config_teensy35 );// return module that woke processor
#elif defined(__MK20DX256__)
    who = Snooze.deepSleep( config_teensy32 );// return module that woke processor
#elif defined(__MK20DX128__)
    who = Snooze.deepSleep( config_teensy30 );// return module that woke processor
#elif defined(__MKL26Z64__)
    who = Snooze.deepSleep( config_teensyLC );// return module that woke processor
#endif
    
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
