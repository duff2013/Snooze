/*************************************************
 This example is used when you want to simulate a
 button being held for certian amount of time
 before waking your Teensy from hibernate sleep.
 
 This example uses the Bounce library to make sure
 the button is still pressed for 3 seconds after
 waking up from low power mode. If it released
 before the 3 seconds are up, go back to sleep.
 *************************************************/

#include <Snooze.h>
#include <Bounce.h>

// Load drivers
SnoozeDigital digital;// this is the pin wakeup driver
// configures the lc's 5v data buffer (OUTPUT, LOW) for low power
Snoozelc5vBuffer lc5vBuffer;

// use bounce for pin 21, debounce of 5ms
Bounce button = Bounce(21, 5);

// install driver into SnoozeBlock
#if defined(__MK66FX1M0__) || defined(__MK64FX512__) || defined(__MK20DX256__)
SnoozeBlock config_teensy3x(digital);
#elif defined(__MKL26Z64__)
SnoozeBlock config_teensyLC(digital, lc5vBuffer);
#endif

void setup() {
    // Configure pin 2 for bounce library
    pinMode(21, INPUT_PULLUP);
    // debug led
    pinMode(LED_BUILTIN, OUTPUT);
    while (!Serial);
    delay(100);
    Serial.println("start...");
    delay(20);
    //pin, mode, type
    digital.pinMode(21, INPUT_PULLUP, FALLING);
}

void loop() {
    // if not held for 3 sec go back here to sleep.
SLEEP:
    // you need to update before sleeping.
    button.update();
    
    // returns module that woke processor after waking from low power mode.
#if defined(__MK66FX1M0__) || defined(__MK64FX512__) || defined(__MK20DX256__)
    Snooze.hibernate( config_teensy3x );
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
    
    // the button was held for at least 3 seconds if
    // you get here do some stuff for 7 seconds then
    // go to sleep.
    elapsedMillis time = 0;
    
    while (1) {
        unsigned int t = time;
        Serial.printf("doin stuff for: %i milliseconds\n", t);
        
        // back to sleep after 7 seconds
        if (time > 7000) {
            Serial.println("sleeping now :)");
            
            // little delay so serial can finish sending
            delay(5);
            
            goto SLEEP;
        }
        digitalWrite(LED_BUILTIN, HIGH);
        delay(50);
        digitalWrite(LED_BUILTIN, LOW);
        delay(50);
    }
}

bool threeSecondHold() {
    // this is the 3 sec button press check
    while (button.duration() < 3000) {
        
        // get the current pin state, must have this!
        button.update();
        
        // check the pin 2 state, if button not
        // pressed before 3 seconds go back to
        // sleep. We read 0 since pin 2 is
        // configured as INPUT_PULLUP.
        if (button.read() != 0) {
            digitalWrite(LED_BUILTIN, LOW);
            // let go of button before 3 sec up
            return false;
        }
    }
    digitalWrite(LED_BUILTIN, LOW);
    
    // button was held for 3 seconds so now we are awake
    return true;
}
