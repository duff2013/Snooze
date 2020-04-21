/*************************************************
  This example uses the Bounce library to make sure
  the button is still pressed for 3 seconds after
  waking up from low power mode. If it released
  before the 3 seconds are up, go back to sleep.

  Supported Micros: T-LC/3.x

  Teensy 4.0 hibernate wakes up with a reset so this sketch
  will not work.
 *************************************************/

#include <Snooze.h>
#include <Bounce.h>

// Load drivers
// Always use SnoozeUSBSerial with T4.0.
// SnoozeUSBSerial driver does what Serial does like "print", -
// "println", "prinf", etc and also handles the effects of using -
// the Arduino Serial monitor and sleeping. Use it when you -
// want to print to serial monitor for sleeping applications.
SnoozeUSBSerial usb;
// pin wakeup driver
SnoozeDigital digital;// this is the pin wakeup driver
#if defined(__MKL26Z64__)
// configures the lc's 5v data buffer (OUTPUT, LOW) for low power
Snoozelc5vBuffer lc5vBuffer;
#endif

const uint8_t BUTTON = 7;

// debounce of 5ms
Bounce button = Bounce(BUTTON, 5);

// install driver into SnoozeBlock
#if defined(__MK66FX1M0__) || defined(__MK64FX512__) || defined(__MK20DX256__)
SnoozeBlock config_teensy(usb, digital);
#elif defined(__MKL26Z64__)
SnoozeBlock config_teensyLC(usb, digital, lc5vBuffer);
#endif
// ------------------------------------------------------------------
void setup() {
  // Configure pin 6 for bounce library
  pinMode(BUTTON, INPUT_PULLUP);
  // debug led
  pinMode(LED_BUILTIN, OUTPUT);
  while (!usb);
  delay(100);
  usb.println("start...");
  delay(20);
  //pin, mode, type
  digital.pinMode(BUTTON, INPUT_PULLUP, FALLING);
}
// ------------------------------------------------------------------
void loop() {
SLEEP:
  digitalWrite(LED_BUILTIN, LOW);

  // you need to update before sleeping.
  button.update();

  usb.println("Going to deepSleep now.\n");
  delay(5);

  // returns module that woke processor after waking from low power mode.
#if defined(__IMXRT1062__) || defined(__MK66FX1M0__) || defined(__MK64FX512__) || defined(__MK20DX256__)
  Snooze.hibernate( config_teensy );
#elif defined(__MKL26Z64__)
  Snooze.hibernate( config_teensyLC );
#endif

  // indicate the button woke it up, hold led high for as long as the button
  // is held down.
  digitalWrite(LED_BUILTIN, HIGH);

  elapsedMillis timeout = 0;
  // bounce needs to call update longer than the debounce time = 5ms,
  // which is set in constructor.
  while (timeout < 6) button.update();

  // now check for 3 second button hold
  bool awake = threeSecondHold();

  // if not held for 3 seconds go back to sleep
  if (!awake) goto SLEEP;

  usb.println("Button Wakeup Activated.");

  // the button was held for at least 3 seconds if
  // you get here do some stuff for 7 seconds then
  // go to sleep.
  elapsedMillis time = 0;

  while (1) {
    unsigned int t = time;
    if (t > 7000) goto SLEEP;
    usb.printf("doin stuff for: %i milliseconds.\n", t);
    // back to sleep after 7 seconds
    digitalWrite(LED_BUILTIN, HIGH);
    delay(25);
    digitalWrite(LED_BUILTIN, LOW);
    delay(25);
  }
}
// ------------------------------------------------------------------
bool threeSecondHold() {
  // this is the 3 sec button press check
  while (button.duration() < 3000) {

    // get the current pin state, must have this!
    button.update();

    // Check button state, if the button
    // is not pressed for 3 seconds go
    // back to sleep.
    if (button.read() != 0) return false;
  }
  // button was held for 3 seconds so now we are awake
  return true;
}
