/***************************************
  This shows all the wakeups for sleep.

  Sleep is the most flexable and any
  interrupt can wake the processor.

  Supported Micros: T-LC/3.x/4.0
****************************************/
#include <Snooze.h>
// Load drivers
SnoozeDigital digital;
SnoozeCompare compare;
SnoozeTimer timer;
SnoozeAlarm  alarm;
SnoozeUSBSerial usb;
#if defined(__MKL26Z64__)
// configures the lc's 5v data buffer (OUTPUT, LOW) for low power
Snoozelc5vBuffer  lc5vBuffer;
#endif
/***********************************************************
  Teensy 4.0 does not have a Touch Interface.

  Teensy 3.6/LC can't use Timer Driver with either Touch or
  Compare Drivers and Touch can't be used with Compare.

  Teensy 3.x/LC touch interface does not work with sleep.

  Teensy LC does not have a rtc so Alarm driver can't be
  used as of yet.

  Teensy 3.2 can use any Core Drivers together.
***********************************************************/
#if defined(__IMXRT1062__)
SnoozeBlock config_teensy40(usb, digital, compare, alarm);
#elif defined(__MK66FX1M0__)
SnoozeBlock config_teensy36(digital, compare, alarm);
#elif defined(__MK64FX512__)
SnoozeBlock config_teensy35(digital, compare, timer);
#elif defined(__MK20DX256__)
SnoozeBlock config_teensy32(digital, compare, timer);
#elif defined(__MK20DX128__)
SnoozeBlock config_teensy30(digital, compare, timer);
#elif defined(__MKL26Z64__)
SnoozeBlock config_teensyLC(lc5vBuffer, digital, timer);
#endif

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  /********************************************************
    Define digital pins for waking the teensy up. This
    combines pinMode and attachInterrupt in one function.

    Teensy 4.0
    Digtal pins: all pins

    Teensy 3.x
    Digtal pins: all pins

    Teensy LC
    Digtal pins: all interrupt able pins
  ********************************************************/
  digital.pinMode(21, INPUT_PULLUP, FALLING);//pin, mode, type
  digital.pinMode(22, INPUT_PULLUP, FALLING);//pin, mode, type

  /********************************************************
    Teensy LC Does not have a RTC.

    Set RTC alarm wake up in (hours, minutes, seconds).
  ********************************************************/
#if !defined(__MKL26Z64__)
  alarm.setRtcTimer(0, 0, 10);// hour, min, sec
#endif

/********************************************************
  Teensy 4.0 Set Low Power Timer wake up in Seconds.
  MAX: 131071s

  Teensy 3.x/LC Set Low Power Timer wake up in Milliseconds.
  MAX: 65535ms
********************************************************/
#if defined(__IMXRT1062__)
  timer.setTimer(5);// seconds
#else
  timer.setTimer(5000);// milliseconds
#endif

  /********************************************************
    In sleep the Compare module works by setting the
    internal 6 bit DAC to a volatge threshold for voltage
    crossing measurements. The internal DAC uses a 64 tap
    resistor ladder network supplied by VOUT33 at 0.0515625v
    per tap (VOUT33/64). Thus the possible threshold voltages
    are 0.0515625*(0-64). Only one compare pin can be used at
    a time.

    parameter "type": LOW & FALLING are the same and have no effect.
    parameter "type": HIGH & RISING are the same and have no effect.

    Teensy 4.0
    Compare pins: 0, 1, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23

    Teensy 3.x
    Compare pins: 11,9,4

    Teensy LC
    Compare pins: 11
  ********************************************************/
  // trigger at threshold values greater than 1.65v
  //(pin, type, threshold(v))
  //compare.pinMode(11, HIGH, 1.65);

  // trigger at threshold values less than 1.65v
  //(pin, type, threshold(v))
  compare.pinMode(11, LOW, 1.65);
}

void loop() {
  int who;
  /********************************************************
    feed the sleep function its wakeup parameters. Then go
    to sleep.
  ********************************************************/
#if defined(__IMXRT1062__)
  who = Snooze.sleep( config_teensy40 );// return module that woke processor
#elif defined(__MK66FX1M0__)
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

  if (who == 35) { // alarm wakeup value
    for (int i = 0; i < 4; i++) {
      digitalWrite(LED_BUILTIN, HIGH);
      delay(200);
      digitalWrite(LED_BUILTIN, LOW);
      delay(200);
    }
  }

  if (who == 36) { // timer wakeup value
    for (int i = 0; i < 5; i++) {
      digitalWrite(LED_BUILTIN, HIGH);
      delay(200);
      digitalWrite(LED_BUILTIN, LOW);
      delay(200);
    }
  }
}
