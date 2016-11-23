#ifndef SnoozeSkeletonDriverClass_h
#define SnoozeSkeletonDriverClass_h

#include "SnoozeBlock.h"
#include "Arduino.h"

class SnoozeSkeletonDriverClass : public SnoozeBlock {
private:
    virtual void disableDriver( void );
    virtual void enableDriver( void );
    virtual void clearIsrFlags( void );
    volatile uint32_t save_pin_config;
    int digital_pin;
    
public:
    SnoozeSkeletonDriverClass(void) { isDriver = true; }
    void configure( int pin );
};

#endif /* defined(SnoozeSkeletonDriverClass_h) */
