/********************************************************************
 - Snooze REDUCED_CPU_BLOCK Hardware Serial Example -

 HardwareSerial1_LP Driver Class reconfigures Serial1 to work at
 a CPU speed of 2 MHz while in the REDUCED_CPU_BLOCK.

 Use second Teensy to read from this serial port.

 Supported Micros: T-LC/3.x
 ********************************************************************/
#include <Snooze.h>
#include "HardwareSerial1_LP.h"
#include "delay.h"

// User Driver Class to make Serial1 work at 2 MHz in the REDUCED_CPU_BLOCK
HardwareSerial1_LP serial1(115200);

// install low power hardware serial driver to a SnoozeBlock
SnoozeBlock serialConfig(serial1);

elapsedMillis time;

void setup() {
    pinMode(LED_BUILTIN, OUTPUT);
    // start Serial1 normally at F_CPU
    Serial1.begin(115200);
    time = 0;
}


void loop() {
    // print serial1 running at F_CPU
    Serial1.printf("F_CPU:\t%i\n", F_CPU);
    Serial1.flush();

    digitalWriteFast(LED_BUILTIN, LOW);

    // REDUCED_CPU_BLOCK will run the HardwareSerial1_LP Driver before
    // and after the code inside the brackets
    REDUCED_CPU_BLOCK(serialConfig) {
        // print serial1 with cpu running at 2 MHz
        Serial1.print("2_MHZ:\t2000000\n");
        Serial1.flush();
        time = 0;
        // see delay.c
        delay_lp(1000);
        uint32_t t = time;
        Serial1.printf("DELAY:\t%i\n", t);
        Serial1.println("----------------------------------------");
        Serial1.flush();
    }

    digitalWriteFast(LED_BUILTIN, HIGH);
    delay(500);
}
