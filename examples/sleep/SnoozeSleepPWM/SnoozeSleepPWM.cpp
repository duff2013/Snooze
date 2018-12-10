#include "SnoozeSleepPWM.h"
#include "defines.h"
/*
 Set the FTM | TPM back to 2 MHz operation. This happens right before
 we switch to 2 MHz then put the processor to sleep.
 */
void SnoozeSleepPWM::enableDriver( void ) {
#if defined(KINETISL)
    SIM_SOPT2_VALUE = SIM_SOPT2;
    SIM_SOPT2 |= SIM_SOPT2_TPMSRC( 3 );
#endif
    *FTMx_CNT = 0;
    *FTMx_MOD = DEFAULT_FTM_02_MOD;
    while ( *FTMx_MOD != DEFAULT_FTM_02_MOD );
    *FTMx_SC = FTM_SC_CLKS( 1 ) | FTM_SC_PS( DEFAULT_FTM_02_PRESCALE );
#if defined(__MK66FX1M0__) && defined(TPM1_CH0_PIN) || defined(TPM1_CH1_PIN)
    TPMx_CxV_VALUE = *TPMx_CxV;
    *TPMx_CxV = cval;
#else
    FTMx_CxV_VALUE = *FTMx_CxV;
    *FTMx_CxV = cval;
#endif
    
}

/*
 Not used, but must be defined. This would be called after we are
 at F_CPU processor speed by then the glitch to the pwm waveform
 is to critical.
 */
void SnoozeSleepPWM::disableDriver( void ) {

}

/*
 Set the FTM | TPM back to F_CPU operation. This gets called from
 whatever driver interrupt wakes the processor.
 */
void SnoozeSleepPWM::clearIsrFlags( void ) {
#if defined(KINETISL)
    SIM_SOPT2 = SIM_SOPT2_VALUE;
#endif
    *FTMx_CNT = 0;
    *FTMx_MOD = DEFAULT_FTM_MOD;
    while ( *FTMx_MOD != DEFAULT_FTM_MOD );
    *FTMx_SC = FTMx_SC_VALUE;
    *FTMx_CxV = rval;
}

/*
 Saves all the FTM | TPM register values at F_CPU and also saves and reconfigures
 these values for 2 MHz operation while in sleep.
 */
void SnoozeSleepPWM::configure( uint8_t pin, int val, uint8_t res ) {
    isUsed = true;
    pwm_pin = pin;
    pwm_value = val;
    pwm_resloution = res;
    rval = ( ( uint32_t )val * ( uint32_t )( DEFAULT_FTM_MOD + 1 ) ) >> pwm_resloution;
    cval = ( ( uint32_t )val * ( uint32_t )( DEFAULT_FTM_02_MOD + 1 ) ) >> pwm_resloution;
    
    FTMx_MOD_VALUE = DEFAULT_FTM_MOD;
    
    switch ( pwm_pin ) {
#ifdef FTM0_CH0_PIN
        case FTM0_CH0_PIN: // PTC1, FTM0_CH0
            FTMx_CxV      = &FTM0_C0V;
            FTMx_SC_VALUE = FTM0_SC;
            FTMx_MOD      = &FTM0_MOD;
            FTMx_SC       = &FTM0_SC;
            FTMx_CNT      = &FTM0_CNT;
            break;
#endif
#ifdef FTM0_CH1_PIN
        case FTM0_CH1_PIN: // PTC2, FTM0_CH1
            FTMx_CxV      = &FTM0_C1V;
            FTMx_SC_VALUE = FTM0_SC;
            FTMx_MOD      = &FTM0_MOD;
            FTMx_SC       = &FTM0_SC;
            FTMx_CNT      = &FTM0_CNT;
            break;
#endif
#ifdef FTM0_CH2_PIN
        case FTM0_CH2_PIN: // PTC3, FTM0_CH2
            FTMx_CxV      = &FTM0_C2V;
            FTMx_SC_VALUE = FTM0_SC;
            FTMx_MOD      = &FTM0_MOD;
            FTMx_SC       = &FTM0_SC;
            FTMx_CNT      = &FTM0_CNT;
            break;
#endif
#ifdef FTM0_CH3_PIN
        case FTM0_CH3_PIN: // PTC4, FTM0_CH3
            FTMx_CxV      = &FTM0_C3V;
            FTMx_SC_VALUE = FTM0_SC;
            FTMx_MOD      = &FTM0_MOD;
            FTMx_SC       = &FTM0_SC;
            FTMx_CNT      = &FTM0_CNT;
            break;
#endif
#ifdef FTM0_CH4_PIN
        case FTM0_CH4_PIN: // PTD4, FTM0_CH4
            FTMx_CxV      = &FTM0_C4V;
            FTMx_SC_VALUE = FTM0_SC;
            FTMx_MOD      = &FTM0_MOD;
            FTMx_SC       = &FTM0_SC;
            FTMx_CNT      = &FTM0_CNT;
            break;
#endif
#ifdef FTM0_CH5_PIN
        case FTM0_CH5_PIN: // PTD5, FTM0_CH5
            FTMx_CxV      = &FTM0_C5V;
            FTMx_SC_VALUE = FTM0_SC;
            FTMx_MOD      = &FTM0_MOD;
            FTMx_SC       = &FTM0_SC;
            FTMx_CNT      = &FTM0_CNT;
            break;
#endif
#ifdef FTM0_CH6_PIN
        case FTM0_CH6_PIN: // PTD6, FTM0_CH6
            FTMx_CxV      = &FTM0_C6V;
            FTMx_SC_VALUE = FTM0_SC;
            FTMx_MOD      = &FTM0_MOD;
            FTMx_SC       = &FTM0_SC;
            FTMx_CNT      = &FTM0_CNT;
            break;
#endif
#ifdef FTM0_CH7_PIN
        case FTM0_CH7_PIN: // PTD7, FTM0_CH7
            FTMx_CxV      = &FTM0_C7V;
            FTMx_SC_VALUE = FTM0_SC;
            FTMx_MOD      = &FTM0_MOD;
            FTMx_SC       = &FTM0_SC;
            FTMx_CNT      = &FTM0_CNT;
            break;
#endif
#ifdef FTM1_CH0_PIN
        case FTM1_CH0_PIN: // PTA12, FTM1_CH0
            //used
            FTMx_CxV      = &FTM1_C0V;
            FTMx_SC_VALUE = FTM1_SC;
            FTMx_SC       = &FTM1_SC;
            FTMx_MOD      = &FTM1_MOD;
            FTMx_CNT      = &FTM1_CNT;
            break;
#endif
#ifdef FTM1_CH1_PIN
        case FTM1_CH1_PIN: // PTA13, FTM1_CH1
            FTMx_CxV      = &FTM1_C1V;
            FTMx_SC_VALUE = FTM1_SC;
            FTMx_MOD      = &FTM1_MOD;
            FTMx_SC       = &FTM1_SC;
            FTMx_CNT      = &FTM1_CNT;
            break;
#endif
#ifdef FTM2_CH0_PIN
        case FTM2_CH0_PIN: // PTB18, FTM2_CH0
            FTMx_CxV      = &FTM2_C0V;
            FTMx_SC_VALUE = FTM2_SC;
            FTMx_MOD      = &FTM2_MOD;
            FTMx_SC       = &FTM2_SC;
            FTMx_CNT      = &FTM2_CNT;
            break;
#endif
#ifdef FTM2_CH1_PIN
        case FTM2_CH1_PIN: // PTB19, FTM1_CH1
            FTMx_CxV      = &FTM2_C1V;
            FTMx_SC_VALUE = FTM2_SC;
            FTMx_MOD      = &FTM2_MOD;
            FTMx_SC       = &FTM2_SC;
            FTMx_CNT      = &FTM2_CNT;
            break;
#endif
#ifdef FTM3_CH0_PIN
        case FTM3_CH0_PIN:
            FTMx_CxV      = &FTM3_C0V;
            FTMx_SC_VALUE = FTM3_SC;
            FTMx_MOD      = &FTM3_MOD;
            FTMx_SC       = &FTM3_SC;
            FTMx_CNT      = &FTM3_CNT;
            break;
#endif
#ifdef FTM3_CH1_PIN
        case FTM3_CH1_PIN:
            FTMx_CxV      = &FTM3_C1V;
            FTMx_SC_VALUE = FTM3_SC;
            FTMx_MOD      = &FTM3_MOD;
            FTMx_SC       = &FTM3_SC;
            FTMx_CNT      = &FTM3_CNT;
            break;
#endif
#ifdef FTM3_CH2_PIN
        case FTM3_CH2_PIN:
            FTMx_CxV      = &FTM3_C2V;
            FTMx_SC_VALUE = FTM3_SC;
            FTMx_MOD      = &FTM3_MOD;
            FTMx_SC       = &FTM3_SC;
            FTMx_CNT      = &FTM3_CNT;
            break;
#endif
#ifdef FTM3_CH3_PIN
        case FTM3_CH3_PIN:
            FTMx_CxV      = &FTM3_C3V;
            FTMx_SC_VALUE = FTM3_SC;
            FTMx_MOD      = &FTM3_MOD;
            FTMx_SC       = &FTM3_SC;
            FTMx_CNT      = &FTM3_CNT;
            break;
#endif
#ifdef FTM3_CH4_PIN
        case FTM3_CH4_PIN:
            FTMx_CxV      = &FTM3_C4V;
            FTMx_SC_VALUE = FTM3_SC;
            FTMx_MOD      = &FTM3_MOD;
            FTMx_SC       = &FTM3_SC;
            FTMx_CNT      = &FTM3_CNT;
            break;
#endif
#ifdef FTM3_CH5_PIN
        case FTM3_CH5_PIN:
            FTMx_CxV      = &FTM3_C5V;
            FTMx_SC_VALUE = FTM3_SC;
            FTMx_MOD      = &FTM3_MOD;
            FTMx_SC       = &FTM3_SC;
            FTMx_CNT      = &FTM3_CNT;
            break;
#endif
#ifdef FTM3_CH6_PIN
        case FTM3_CH6_PIN:
            FTMx_CxV      = &FTM3_C6V;
            FTMx_SC_VALUE = FTM3_SC;
            FTMx_MOD      = &FTM3_MOD;
            FTMx_SC       = &FTM3_SC;
            FTMx_CNT      = &FTM3_CNT;
            break;
#endif
#ifdef FTM3_CH7_PIN
        case FTM3_CH7_PIN:
            FTMx_CxV      = &FTM3_C7V;
            FTMx_SC_VALUE = FTM3_SC;
            FTMx_MOD      = &FTM3_MOD;
            FTMx_SC       = &FTM3_SC;
            FTMx_CNT      = &FTM3_CNT;
            break;
#endif
#ifdef TPM1_CH0_PIN
        case TPM1_CH0_PIN:
            
            // not implemented
            TPMx_CxV      = &TPM1_C0V;
            FTMx_SC_VALUE = TPM1_SC;
            FTMx_MOD      = &TPM1_MOD;
            FTMx_SC       = &TPM1_SC;
            FTMx_CNT      = &FTM1_CNT;
            break;
#endif
#ifdef TPM1_CH1_PIN
        case TPM1_CH1_PIN:
            // not implemented
            TPMx_CxV      = &TPM1_C1V;
            FTMx_SC_VALUE = TPM1_SC;
            FTMx_MOD      = &TPM1_MOD;
            FTMx_SC       = &TPM1_SC;
            FTMx_CNT      = &TTM1_CNT;
            break;
#endif
        default:
            break;
    }
}

