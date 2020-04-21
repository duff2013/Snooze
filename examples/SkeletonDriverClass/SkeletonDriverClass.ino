/********************************************************************
 - Snooze Skelton Driver Example -
 A driver class needs to inherient from the SnoozeBlock class. You must
 override the 'enableDriver', 'disbaleDriver' and optional 'clearIsrFlags'
 virtual functions.

 This example driver class saves the state of the pin, configures
 it as an OUTPUT and blinks the LED before and after going to sleep.

 Supported Micros: T-LC/3.x/4.0
 ********************************************************************/
#include <Snooze.h>
#include <wiring.h>
#include "SnoozeSkeletonDriverClass.h"

// Load drivers
SnoozeSkeletonDriverClass skeleton;
SnoozeTimer timer;

// install drivers to a configuration block
SnoozeBlock config(skeleton, timer);

void setup() {
    pinMode(LED_BUILTIN, INPUT_PULLUP);
    //pinMode(LED_BUILTIN, INPUT);
    // configure the skeleton driver to toggle before and after sleeping
    skeleton.configure(LED_BUILTIN);
    // configure the timer driver to wake in 1000 milliseconds
    timer.setTimer(1000);
    delay(1000);
}

void loop() {
    // sleep now
    Snooze.deepSleep( config );// return module that woke processor
    delay(1000);
}
