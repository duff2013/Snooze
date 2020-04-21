/**********************************************************
  Simple Data Logger Example using Deep Sleep mode.

  Hold the button pin(3) to log data.

  Supported Micros: T-LC/3.x/4.0
 **********************************************************/

#include <Snooze.h>
#include <Bounce.h>

// Load drivers
// Always use SnoozeUSBSerial with T4.0.
// SnoozeUSBSerial driver does what Serial does like "print",
// "println", "prinf", etc and also handles the effects of using
// the Arduino Serial monitor and sleeping. Use it when you
// want to print to serial monitor for sleeping applications.
SnoozeUSBSerial usb;
// button wakeup driver
SnoozeDigital digital;
#if defined(__MKL26Z64__)
// configures the lc's 5v data buffer (OUTPUT, LOW) for low power
Snoozelc5vBuffer lc5vBuffer;
#endif

const uint8_t BUTTON = 7;

// use bounce for pin 4, debounce of 5ms
Bounce button = Bounce(BUTTON, 5);

// buffer to hold data for printing, this is because the -
// Serial Monitor takes a few seconds to come back online -
// after waking up from sleep mode
static char print_buffer[2000] = {0};

// install driver into SnoozeBlock
#if defined(__IMXRT1062__) | defined(__MK66FX1M0__) || defined(__MK64FX512__) || defined(__MK20DX256__)
SnoozeBlock config_teensy(usb, digital);
#elif defined(__MKL26Z64__)
SnoozeBlock config_teensy(usb, digital, lc5vBuffer);
#endif
// ------------------------------------------------------------------
void setup() {
  // Configure pin 3 for bounce library
  pinMode(BUTTON, INPUT_PULLUP);
  // Configure Snooze pinMode(pin, mode, irq type)
  digital.pinMode(BUTTON, INPUT_PULLUP, FALLING);
  // debug led
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  while (!usb);
  delay(100);
  digitalWrite(LED_BUILTIN, LOW);

  usb.println("Starting Data Logger /w Deep Sleeping Example...");
  usb.println("-------------------------------------------");
  delay(20);
}
// ------------------------------------------------------------------
void loop() {
SLEEP:
  digitalWrite(LED_BUILTIN, LOW);

  // you need to update before sleeping.
  button.update();

  usb.println("Going to deepSleep now.\n");
  delay(5);
  Snooze.deepSleep( config_teensy );
  digitalWrite(LED_BUILTIN, HIGH);

  // check for 300ms that the button is held
  bool hold = threeHundredMilliSecondHoldCheck();

  usb.println("Data Logging.");

  // if not held for 300ms go back to sleep
  if (hold == false) goto SLEEP;

  // Data logging routine
  datalog();
}
// ------------------------------------------------------------------
// Make sure the button is help low for aleast 300ms before data logging.
bool threeHundredMilliSecondHoldCheck() {
  // Pin is configured as PULLUP so 0 means the button has been
  // pushed.
  button.update();
  while (button.read() == 0) {
    // Monitor the pin hold duration.
    if (button.duration() > 300) return true;
    // Update bounce library.
    button.update();
  }
  return false;
}
// ------------------------------------------------------------------
void datalog() {
  elapsedMillis samplerate = 0;
  unsigned int idx = 0;
  button.update();
  while (button.read() == 0) {
    if (samplerate >= 20) {
      unsigned int x = analogRead(A0);
      unsigned int y = analogRead(A1);
      unsigned int z = analogRead(A2);

      // print data to serial monitor
      usb.printf("index: %3u | x: %3u | y: %3u | z: %3u\n", idx++, x, y, z);

      digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
      samplerate = 0;
    }
    // Update bounce library.
    button.update();
  }
  digitalWrite(LED_BUILTIN, LOW);
}
