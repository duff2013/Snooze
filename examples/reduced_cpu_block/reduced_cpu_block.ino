/***************************************
 * REDUCED_CPU_BLOCK allows you to run
 * code at 2 MHz (VLPR). See micro_lp and
 * delay_lp to see how to convert core
 * functionality at F_CPU to work at 2 MHz.
 ****************************************/
#include <Snooze.h>

#ifdef __cplusplus
extern "C" {
#endif
    void delay_lp(uint32_t ms);
#ifdef __cplusplus
}
#endif

void setup() {
    pinMode(LED_BUILTIN, OUTPUT);
}


void loop() {
    SOS(); // F_CPU
    /*
     * code inside this macro
     * will run at 2 MHz.
     * code outside will run
     * at whatever F_CPU you
     * compiled at.
     */
    REDUCED_CPU_BLOCK() {
        SOS_LP();// 2 MHz
    }
}

// SOS from http://www.thejamesjones.com/blog/make-your-arduino-blink-sos
void SOS() {
    digitalWrite(LED_BUILTIN, HIGH);   // turn the LED_BUILTIN on (HIGH is the voltage level)
    delay(0250);               // wait for a second
    digitalWrite(LED_BUILTIN, LOW);    // turn the LED_BUILTIN off by making the voltage LOW
    delay(0250);               // wait for a second
    
    digitalWrite(LED_BUILTIN, HIGH);   // turn the LED_BUILTIN on (HIGH is the voltage level)
    delay(0250);               // wait for a second
    digitalWrite(LED_BUILTIN, LOW);    // turn the LED_BUILTIN off by making the voltage LOW
    delay(0250);               // wait for a second
    
    digitalWrite(LED_BUILTIN, HIGH);   // turn the LED_BUILTIN on (HIGH is the voltage level)
    delay(0250);               // wait for a second
    digitalWrite(LED_BUILTIN, LOW);    // turn the LED_BUILTIN off by making the voltage LOW
    delay(0600);               // wait for a second
    
    digitalWrite(LED_BUILTIN, HIGH);   // turn the LED_BUILTIN on (HIGH is the voltage level)
    delay(1000);               // wait for a second
    digitalWrite(LED_BUILTIN, LOW);    // turn the LED_BUILTIN off by making the voltage LOW
    delay(0250);               // wait for a second
    
    digitalWrite(LED_BUILTIN, HIGH);   // turn the LED_BUILTIN on (HIGH is the voltage level)
    delay(1000);               // wait for a second
    digitalWrite(LED_BUILTIN, LOW);    // turn the LED_BUILTIN off by making the voltage LOW
    delay(0250);               // wait for a second
    
    digitalWrite(LED_BUILTIN, HIGH);   // turn the LED_BUILTIN on (HIGH is the voltage level)
    delay(1000);               // wait for a second
    digitalWrite(LED_BUILTIN, LOW);    // turn the LED_BUILTIN off by making the voltage LOW
    delay(0250);               // wait for a second
    
    digitalWrite(LED_BUILTIN, HIGH);   // turn the LED_BUILTIN on (HIGH is the voltage level)
    delay(0250);               // wait for a second
    digitalWrite(LED_BUILTIN, LOW);    // turn the LED_BUILTIN off by making the voltage LOW
    delay(0250);               // wait for a second
    
    digitalWrite(LED_BUILTIN, HIGH);   // turn the LED_BUILTIN on (HIGH is the voltage level)
    delay(0250);               // wait for a second
    digitalWrite(LED_BUILTIN, LOW);    // turn the LED_BUILTIN off by making the voltage LOW
    delay(0250);               // wait for a second
    
    digitalWrite(LED_BUILTIN, HIGH);   // turn the LED_BUILTIN on (HIGH is the voltage level)
    delay(0250);               // wait for a second
    digitalWrite(LED_BUILTIN, LOW);    // turn the LED_BUILTIN off by making the voltage LOW
    delay(1000);               // wait for a second
}

void SOS_LP() {
    digitalWrite(LED_BUILTIN, HIGH);   // turn the LED_BUILTIN on (HIGH is the voltage level)
    delay_lp(0250);               // wait for a second
    digitalWrite(LED_BUILTIN, LOW);    // turn the LED_BUILTIN off by making the voltage LOW
    delay_lp(0250);               // wait for a second
    
    digitalWrite(LED_BUILTIN, HIGH);   // turn the LED_BUILTIN on (HIGH is the voltage level)
    delay_lp(0250);               // wait for a second
    digitalWrite(LED_BUILTIN, LOW);    // turn the LED_BUILTIN off by making the voltage LOW
    delay_lp(0250);               // wait for a second
    
    digitalWrite(LED_BUILTIN, HIGH);   // turn the LED_BUILTIN on (HIGH is the voltage level)
    delay_lp(0250);               // wait for a second
    digitalWrite(LED_BUILTIN, LOW);    // turn the LED_BUILTIN off by making the voltage LOW
    delay_lp(0600);               // wait for a second
    
    digitalWrite(LED_BUILTIN, HIGH);   // turn the LED_BUILTIN on (HIGH is the voltage level)
    delay_lp(1000);               // wait for a second
    digitalWrite(LED_BUILTIN, LOW);    // turn the LED_BUILTIN off by making the voltage LOW
    delay_lp(0250);               // wait for a second
    
    digitalWrite(LED_BUILTIN, HIGH);   // turn the LED_BUILTIN on (HIGH is the voltage level)
    delay_lp(1000);               // wait for a second
    digitalWrite(LED_BUILTIN, LOW);    // turn the LED_BUILTIN off by making the voltage LOW
    delay_lp(0250);               // wait for a second
    
    digitalWrite(LED_BUILTIN, HIGH);   // turn the LED_BUILTIN on (HIGH is the voltage level)
    delay_lp(1000);               // wait for a second
    digitalWrite(LED_BUILTIN, LOW);    // turn the LED_BUILTIN off by making the voltage LOW
    delay_lp(0250);               // wait for a second
    
    digitalWrite(LED_BUILTIN, HIGH);   // turn the LED_BUILTIN on (HIGH is the voltage level)
    delay_lp(0250);               // wait for a second
    digitalWrite(LED_BUILTIN, LOW);    // turn the LED_BUILTIN off by making the voltage LOW
    delay_lp(0250);               // wait for a second
    
    digitalWrite(LED_BUILTIN, HIGH);   // turn the LED_BUILTIN on (HIGH is the voltage level)
    delay_lp(0250);               // wait for a second
    digitalWrite(LED_BUILTIN, LOW);    // turn the LED_BUILTIN off by making the voltage LOW
    delay_lp(0250);               // wait for a second
    
    digitalWrite(LED_BUILTIN, HIGH);   // turn the LED_BUILTIN on (HIGH is the voltage level)
    delay_lp(0250);               // wait for a second
    digitalWrite(LED_BUILTIN, LOW);    // turn the LED_BUILTIN off by making the voltage LOW
    delay_lp(1000);               // wait for a second
}

//*************************************Dynamic Core C-Linkage**************************************
#ifdef __cplusplus
extern "C" {
#endif
    //*******************low power micros @ 2 MHZ*************************
    uint32_t micros_lp(void) {
        uint32_t count, current, istatus;
        
        __disable_irq();
        current = SYST_CVR;
        count = systick_millis_count;
        istatus = SCB_ICSR; // bit 26 indicates if systick exception pending
        __enable_irq();
        
        if ((istatus & SCB_ICSR_PENDSTSET) && current > 50) count++;
        current = ((TWO_MHZ / 1000) - 1) - current;
        return count * 1000 + current / (TWO_MHZ / 1000000);
    }
    
    //*******************low power delay_lp @ 2 MHZ************************
    void delay_lp(uint32_t ms) {
        uint32_t start = micros_lp();
        
        if (ms > 0) {
            while (1) {
                if ((micros_lp() - start) >= 1000) {
                    ms--;
                    if (ms == 0) return;
                    start += 1000;
                }
                yield();
            }
        }
    }
#ifdef __cplusplus
}
#endif