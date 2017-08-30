/***************************************
 This shows the use of the Snooze USB
 Serial driver when using deepSleep.
 The USB Serial driver does not wake the
 processor it only aids in using the USB
 Serial in sleeping appilcations.
 ****************************************/
#include <Snooze.h>
// Load drivers
SnoozeDigital     digital;
SnoozeTimer       timer;
SnoozeUSBSerial   usb;
// configures the lc's 5v data buffer (OUTPUT, LOW) for low power
Snoozelc5vBuffer  lc5vBuffer;
/***********************************************************
 Install drivers, timer to wake and USB Serial Driver to
 fix printing to serial monitor after sleeping.
 ***********************************************************/
#if defined(__MK66FX1M0__)
SnoozeBlock config_teensy36(usb, timer, digital);
#elif defined(__MK64FX512__)
SnoozeBlock config_teensy35(usb, timer, digital);
#elif defined(__MK20DX256__)
SnoozeBlock config_teensy32(usb, timer, digital);
#elif defined(__MKL26Z64__)
SnoozeBlock config_teensyLC(usb, timer, digital, lc5vBuffer);
#endif

int idx;

void setup() {
    pinMode(LED_BUILTIN, OUTPUT);
    while (!Serial);
    delay(100);
    Serial.println("Starting...");
    delay(100);
    /********************************************************
     Set Low Power Timer wake up in milliseconds.
     ********************************************************/
    timer.setTimer(5000);// milliseconds
    /********************************************************
     Define digital pins for waking the teensy up. This
     combines pinMode and attachInterrupt in one function.
     ********************************************************/
    digital.pinMode(21, INPUT_PULLUP, RISING);//pin, mode, type
    digital.pinMode(22, INPUT_PULLUP, RISING);//pin, mode, type
    // sleep index
    idx = 0;
}

void loop() {
    int who = 0; // what woke us up
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
#elif defined(__MKL26Z64__)
    who = Snooze.deepSleep( config_teensyLC );// return module that woke processor
#endif
    // wait for serial monitor
    elapsedMillis time = 0;
    while (!Serial && time < 1000) {
        Serial.write(0x00);// print out a bunch of NULLS to serial monitor
        digitalWriteFast(LED_BUILTIN, HIGH);
        delay(30);
        digitalWriteFast(LED_BUILTIN, LOW);
        delay(30);
    }
    // normal delay for Arduino Serial Monitor
    delay(200);
    // print who woke the teensy up, i.e. timer || digital
    Serial.printf("Timer Driver number indicator: %i | index: %i\n", who, idx);
    delay(1000);
    idx++;
}
