#include "SnoozeSkeletonDriverClass.h"
/*

*/
void SnoozeSkeletonDriverClass::enableDriver( uint8_t mode ) {
  // This function gets called before any sleep or
  // low power operation from the Snooze Class.

  // save current pin state before we change it to an OUTPUT
  volatile uint32_t *config;
  config = portConfigRegister( digital_pin );
  save_pin_config = *config;
  // turn pin to OUTPUT
  pinMode( digital_pin, OUTPUT );
  config = portConfigRegister( digital_pin );
  // blink led a few times
  for ( int i = 0; i < 3; i++ ) {
    digitalWriteFast( digital_pin, HIGH );
    delay( 50 );
    digitalWriteFast( digital_pin, LOW );
    delay( 50 );
  }
}

/*

*/
void SnoozeSkeletonDriverClass::disableDriver( uint8_t mode ) {
  // This function gets called after any sleep or
  // low power operation from the Snooze Class.

  // since it is still configured OUTPUT toggle pin
  for ( int i = 0; i < 1; i++ ) {
    digitalWriteFast( digital_pin, HIGH );
    delay( 50 );
    digitalWriteFast( digital_pin, LOW );
    delay( 50 );
  }

  // restore pin to previous state
#ifdef KINETISK
  *portModeRegister( digital_pin ) = 0;
#else
  *portModeRegister( digital_pin ) &= ~digitalPinToBitMask( digital_pin );
#endif
  volatile uint32_t *config;
  config = portConfigRegister( digital_pin );
  *config = save_pin_config;
}
/*

*/
void SnoozeSkeletonDriverClass::clearIsrFlags( uint8_t mode ) {
  // This function gets called from wakeup Isr, you clear
  // any module flags that need to cleared here.

  // Nothing to do in this driver...
}
/*

*/
void SnoozeSkeletonDriverClass::configure( int pin ) {
  // Configure which pin to use

  // save the pin
  digital_pin = pin;

  // let SnoozeBlock that this driver is configured and active
  // all classes must set the 'isUsed' variable = true.
  isUsed = true;
}

