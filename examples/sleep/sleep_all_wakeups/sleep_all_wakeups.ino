/***************************************
 This shows all the wakeups for sleep
 Expect IDD of  around 1.2mA (Teensy 3.x)
 and IDD of around 900uA for (Teensy LC).
 
 Sleep is the most flexable and any
 interrupt can wake the processor.
 
 Touch interface does not work in sleep
 mode.
 ****************************************/
#include <Snooze.h>
// Load drivers
SnoozeDigital digital;
SnoozeCompare compare;
SnoozeTimer timer;
SnoozeAlarm  alarm;
// configures the lc's 5v data buffer (OUTPUT, LOW) for low power
Snoozelc5vBuffer  lc5vBuffer;
/***********************************************************
 Teensy 3.6/LC can't use Timer Driver with either Touch or
 Compare Drivers and Touch can't be used with Compare.
 
 Teensy 3.x/LC touch interface does not work with sleep.
 
 Teensy LC does not have a rtc so Alarm driver can't be
 used as of yet.
 
 Teensy 3.2 can use any Core Drivers together.
 ***********************************************************/
#if defined(__MK66FX1M0__)
SnoozeBlock config_teensy36(digital, alarm, compare);
#elif defined(__MK64FX512__)
SnoozeBlock config_teensy35(digital, timer, compare);
#elif defined(__MK20DX256__)
SnoozeBlock config_teensy32(digital, timer, compare);
#elif defined(__MK20DX128__)
SnoozeBlock config_teensy30(digital, timer, compare);
#elif defined(__MKL26Z64__)
SnoozeBlock config_teensyLC(digital, timer, lc5vBuffer);
#endif

void setup() {
    pinMode(LED_BUILTIN, OUTPUT);
    /********************************************************
     Define digital pins for waking the teensy up. This
     combines pinMode and attachInterrupt in one function.
     
     Teensy 3.x
     Digtal pins: all pins
     
     Teensy LC
     Digtal pins: all interrupt able pins
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
     Values greater or less than threshold will trigger CMP
     wakeup. Threshold value is in volts (0-3.3v) using a 64
     tap resistor ladder network at 0.0515625v per tap.
     
     parameter "type": LOW & FALLING are the same.
     parameter "type": HIGH & RISING are the same.
     
     Teensy 3.x/LC
     Compare pins: 11,12
     ********************************************************/
    // trigger at threshold values greater than 1.65v
    //compare.pinMode(11, HIGH, 1.65);//pin, type, threshold(v)
    // trigger at threshold values less than 1.65v
    compare.pinMode(11, LOW, 1.65);//pin, type, threshold(v)
}

void loop() {
    int who;
    /********************************************************
     feed the sleep function its wakeup parameters. Then go
     to deepSleep.
     ********************************************************/
#if defined(__MK66FX1M0__)
    who = Snooze.sleep( config_teensy36 );// return module that woke processor
#elif defined(__MK64FX512__)
    who = Snooze.sleep( config_teensy35 );// return module that woke processor
#elif defined(__MK20DX256__)
    who = Snooze.sleep( config_teensy32 );// return module that woke processor
#elif defined(__MK20DX128__)
    who = Snooze.sleep( config_teensy30 );// return module that woke processor
#elif defined(__MKL26Z64__)
    who = Snooze.sleep( config_teensyLC );// return module that woke processor
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
}
