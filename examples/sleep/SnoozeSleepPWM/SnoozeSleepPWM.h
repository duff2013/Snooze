#ifndef SnoozeSleepPWM_h
#define SnoozeSleepPWM_h

#include "SnoozeBlock.h"
#include "Arduino.h"

class SnoozeSleepPWM : public SnoozeBlock {
private:
    virtual void disableDriver( void );
    virtual void enableDriver( void );
    virtual void clearIsrFlags( void );
    uint32_t cval;
    uint32_t rval;
    uint32_t max;
    uint32_t mod;
    uint8_t pwm_pin;
    uint8_t pwm_value;
    uint8_t pwm_resloution;
    volatile uint32_t *FTMx_CxV;
    volatile uint32_t *TPMx_CxV;
    volatile uint32_t *FTMx_MOD;
    volatile uint32_t *TPMx_MOD;
    volatile uint32_t *FTMx_SC;
    volatile uint32_t *TPMx_SC;
    volatile uint32_t *FTMx_CNT;
    uint32_t SIM_SOPT2_VALUE;
    uint32_t FTMx_CxV_VALUE;
    uint32_t TPMx_CxV_VALUE;
    uint32_t FTMx_MOD_VALUE;
    uint32_t TPMx_MOD_VALUE;
    uint32_t FTMx_SC_VALUE;
    uint32_t TPMx_SC_VALUE;
public:
    SnoozeSleepPWM(void) { isDriver = true; }
    void configure( uint8_t pin, int value, uint8_t res  );
};

#endif /* defined(SnoozeSleepPWM_h) */
