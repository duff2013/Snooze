/***************************************
 This shows the use of the Snooze USB
 Serial driver when using deepSleep.
 The USB Serial driver does not wake the
 processor it only aids in using the USB
 Serial in sleeping appilcations.

 Supported Micros: T-LC/3.x/4.0
 ****************************************/
#include <Snooze.h>
// Load drivers
// Always use SnoozeUSBSerial with T4.0.
// SnoozeUSBSerial driver does what Serial does like "print", -
// "println", "prinf", etc and also handles the effects of using -
// the Arduino Serial monitor and sleeping. Use it when you -
// want to print to serial monitor for sleeping applications.
SnoozeUSBSerial usb;
SnoozeTimer     timer;
#if defined(__MKL26Z64__)
// configures the lc's 5v data buffer (OUTPUT, LOW) for low power
Snoozelc5vBuffer lc5vBuffer;
#endif
/***********************************************************
  Install drivers, timer to wake and USB Serial to fix
  printing to serial monitor after sleeping.
 ***********************************************************/
#if defined(__IMXRT1062__)
SnoozeBlock config_teensy40(usb, timer);
#elif defined(__MK66FX1M0__)
SnoozeBlock config_teensy36(usb, timer);
#elif defined(__MK64FX512__)
SnoozeBlock config_teensy35(usb, timer);
#elif defined(__MK20DX256__)
SnoozeBlock config_teensy32(usb, timer);
#elif defined(__MKL26Z64__)
SnoozeBlock config_teensyLC(usb, lc5vBuffer, timer);
#endif

int idx;

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  // USBSerial does what Serial and handles usb -
  // when sleeping.
  while (!usb);
  delay(100);
  usb.println("Starting...");
  delay(100);
  /********************************************************
    Set Low Power Timer wake up in Seconds(T4) or milliseconds
    all other Teensy's.
   ********************************************************/
#if defined(__IMXRT1062__)
  timer.setTimer(5);// seconds
#else
  timer.setTimer(5000);// milliseconds
#endif
  idx = 0;
}

void loop() {
  int who = 0;
  /********************************************************
    feed the sleep function its wakeup parameters. Then go
    to sleep.
   ********************************************************/
#if defined(__IMXRT1062__)
  who = Snooze.deepSleep( config_teensy40 );// return module that woke processor
#elif defined(__MK66FX1M0__)
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
  while (!usb && time < 1000) {
#if !defined(__IMXRT1062__)
    // print out a bunch of NULLS to serial monitor
    // T3.x/LC only
    usb.write(0x00);
#endif
    digitalWriteFast(LED_BUILTIN, HIGH);
    delay(20);
    digitalWriteFast(LED_BUILTIN, LOW);
    delay(20);
  }
  delay(200); // normal delay for Arduino Serial Monitor
  usb.printf("Timer Driver number indicator: %i | index: %i\n", who, idx); // print who woke up, i.e. timer
  delay(1000);
  idx++;
}
