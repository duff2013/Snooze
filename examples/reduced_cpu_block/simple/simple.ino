/***************************************
 * REDUCED_CPU_BLOCK allows you to run
 * code at 2 MHz (VLPR). See micro_lp and
 * delay_lp to see how to convert core
 * functionality at F_CPU to work at 2 MHz.

 Supported Micros: T-LC/3.x
 ****************************************/
#include <Snooze.h>
#include "delay.h"

// REDUCED_CPU_BLOCK needs a SnoozeBlock passed to it
// so we pass a dummy SnoozeBlock with no Drivers installed.
SnoozeBlock dummyConfig;

void setup() {
    pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
    SOS(); // Blink sos at F_CPU

    REDUCED_CPU_BLOCK(dummyConfig) {
        /*
         * code inside this macro
         * will run at 2 MHz cpu.
         * code outside will run
         * at whatever speed you
         * compiled at.
         */
        SOS_LP();// Blink sos at 2 MHz
    }
}
// SOS from http://www.thejamesjones.com/blog/make-your-arduino-blink-sos
void SOS() {
    digitalWrite(LED_BUILTIN, HIGH);   // turn the LED_BUILTIN on (HIGH is the voltage level)
    delay(0250);               // wait for a second
    digitalWrite(LED_BUILTIN, LOW);    // turn the LED_BUILTIN off by making the voltage LOW
    delay(0250);               // wait for a second

    digitalWrite(LED_BUILTIN, HIGH);   // turn the LED_BUILTIN on (HIGH is the voltage level)
    delay(0250);               // wait for a second
    digitalWrite(LED_BUILTIN, LOW);    // turn the LED_BUILTIN off by making the voltage LOW
    delay(0250);               // wait for a second

    digitalWrite(LED_BUILTIN, HIGH);   // turn the LED_BUILTIN on (HIGH is the voltage level)
    delay(0250);               // wait for a second
    digitalWrite(LED_BUILTIN, LOW);    // turn the LED_BUILTIN off by making the voltage LOW
    delay(0600);               // wait for a second

    digitalWrite(LED_BUILTIN, HIGH);   // turn the LED_BUILTIN on (HIGH is the voltage level)
    delay(1000);               // wait for a second
    digitalWrite(LED_BUILTIN, LOW);    // turn the LED_BUILTIN off by making the voltage LOW
    delay(0250);               // wait for a second

    digitalWrite(LED_BUILTIN, HIGH);   // turn the LED_BUILTIN on (HIGH is the voltage level)
    delay(1000);               // wait for a second
    digitalWrite(LED_BUILTIN, LOW);    // turn the LED_BUILTIN off by making the voltage LOW
    delay(0250);               // wait for a second

    digitalWrite(LED_BUILTIN, HIGH);   // turn the LED_BUILTIN on (HIGH is the voltage level)
    delay(1000);               // wait for a second
    digitalWrite(LED_BUILTIN, LOW);    // turn the LED_BUILTIN off by making the voltage LOW
    delay(0250);               // wait for a second

    digitalWrite(LED_BUILTIN, HIGH);   // turn the LED_BUILTIN on (HIGH is the voltage level)
    delay(0250);               // wait for a second
    digitalWrite(LED_BUILTIN, LOW);    // turn the LED_BUILTIN off by making the voltage LOW
    delay(0250);               // wait for a second

    digitalWrite(LED_BUILTIN, HIGH);   // turn the LED_BUILTIN on (HIGH is the voltage level)
    delay(0250);               // wait for a second
    digitalWrite(LED_BUILTIN, LOW);    // turn the LED_BUILTIN off by making the voltage LOW
    delay(0250);               // wait for a second

    digitalWrite(LED_BUILTIN, HIGH);   // turn the LED_BUILTIN on (HIGH is the voltage level)
    delay(0250);               // wait for a second
    digitalWrite(LED_BUILTIN, LOW);    // turn the LED_BUILTIN off by making the voltage LOW
    delay(1000);               // wait for a second
}

void SOS_LP() {
    digitalWrite(LED_BUILTIN, HIGH);   // turn the LED_BUILTIN on (HIGH is the voltage level)
    delay_lp(0250);               // wait for a second
    digitalWrite(LED_BUILTIN, LOW);    // turn the LED_BUILTIN off by making the voltage LOW
    delay_lp(0250);               // wait for a second

    digitalWrite(LED_BUILTIN, HIGH);   // turn the LED_BUILTIN on (HIGH is the voltage level)
    delay_lp(0250);               // wait for a second
    digitalWrite(LED_BUILTIN, LOW);    // turn the LED_BUILTIN off by making the voltage LOW
    delay_lp(0250);               // wait for a second

    digitalWrite(LED_BUILTIN, HIGH);   // turn the LED_BUILTIN on (HIGH is the voltage level)
    delay_lp(0250);               // wait for a second
    digitalWrite(LED_BUILTIN, LOW);    // turn the LED_BUILTIN off by making the voltage LOW
    delay_lp(0600);               // wait for a second

    digitalWrite(LED_BUILTIN, HIGH);   // turn the LED_BUILTIN on (HIGH is the voltage level)
    delay_lp(1000);               // wait for a second
    digitalWrite(LED_BUILTIN, LOW);    // turn the LED_BUILTIN off by making the voltage LOW
    delay_lp(0250);               // wait for a second

    digitalWrite(LED_BUILTIN, HIGH);   // turn the LED_BUILTIN on (HIGH is the voltage level)
    delay_lp(1000);               // wait for a second
    digitalWrite(LED_BUILTIN, LOW);    // turn the LED_BUILTIN off by making the voltage LOW
    delay_lp(0250);               // wait for a second

    digitalWrite(LED_BUILTIN, HIGH);   // turn the LED_BUILTIN on (HIGH is the voltage level)
    delay_lp(1000);               // wait for a second
    digitalWrite(LED_BUILTIN, LOW);    // turn the LED_BUILTIN off by making the voltage LOW
    delay_lp(0250);               // wait for a second

    digitalWrite(LED_BUILTIN, HIGH);   // turn the LED_BUILTIN on (HIGH is the voltage level)
    delay_lp(0250);               // wait for a second
    digitalWrite(LED_BUILTIN, LOW);    // turn the LED_BUILTIN off by making the voltage LOW
    delay_lp(0250);               // wait for a second

    digitalWrite(LED_BUILTIN, HIGH);   // turn the LED_BUILTIN on (HIGH is the voltage level)
    delay_lp(0250);               // wait for a second
    digitalWrite(LED_BUILTIN, LOW);    // turn the LED_BUILTIN off by making the voltage LOW
    delay_lp(0250);               // wait for a second

    digitalWrite(LED_BUILTIN, HIGH);   // turn the LED_BUILTIN on (HIGH is the voltage level)
    delay_lp(0250);               // wait for a second
    digitalWrite(LED_BUILTIN, LOW);    // turn the LED_BUILTIN off by making the voltage LOW
    delay_lp(1000);               // wait for a second
}
