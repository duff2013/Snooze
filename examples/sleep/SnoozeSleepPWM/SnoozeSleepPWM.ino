/***************************************
  This example shows how you can use the pwm
  while in 'sleep' mode, pwm will not work
  with deepSleep or hibernate because the
  pwm clocks are not active.

  Because 'sleep' mode puts the cpu speed
  at 2 MHz the FTM registers have to be
  updated for the 2 MHz cpu which this
  driver does.

  Supported Micros: T-LC/3.x

  Teensy 4.0 will not work with this sketch
 ****************************************/
#include <Snooze.h>
#include "SnoozeSleepPWM.h"

SnoozeTimer     timer;
SnoozeSleepPWM  pwm;// 'sleep' mode pwm driver

// Connect the timer and pwm drivers
SnoozeBlock config(timer, pwm);

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  timer.setTimer(30);
  delay(300);

}

void loop() {
  analogWrite(3, 32);
  // pin, value, resolution
  pwm.configure(3, 32, 8);
  Snooze.sleep(config);
  blink();

  analogWrite(3, 64);
  // pin, value, resolution
  pwm.configure(3, 64, 8);
  Snooze.sleep(config);
  blink();

  analogWrite(3, 128);
  // pin, value, resolution
  pwm.configure(3, 128, 8);
  Snooze.sleep(config);
  blink();

  analogWrite(3, 256);
  // pin, value, resolution
  pwm.configure(3, 256, 8);
  Snooze.sleep(config);
  blink();
}

void blink() {
  for (int i = 0; i < 2; i++) {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(50);
    digitalWrite(LED_BUILTIN, LOW);
    delay(50);
  }
}
