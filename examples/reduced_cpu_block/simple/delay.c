#include "Arduino.h"
#define TWO_MHZ 2000000
// delay and micros for 2 MHz cpu speed
/***************************************************************************************************/
uint32_t micros_lp(void) {
  uint32_t count, current, istatus;

  __disable_irq();
  current = SYST_CVR;
  count = systick_millis_count;
  istatus = SCB_ICSR; // bit 26 indicates if systick exception pending
  __enable_irq();
  //systick_current = current;
  //systick_count = count;
  //systick_istatus = istatus & SCB_ICSR_PENDSTSET ? 1 : 0;
  if ((istatus & SCB_ICSR_PENDSTSET) && current > 50) count++;
  current = ((2000000 / 1000) - 1) - current;
  return count * 1000 + current / (TWO_MHZ / 1000000);
}
/***************************************************************************************************/
void delay_lp(uint32_t ms) {
  uint32_t start = micros();

  if (ms > 0) {
    while (1) {
      while ((micros() - start) >= 1000) {
        ms--;
        if (ms == 0) return;
        start += 1000;
      }
      yield();
    }
  }
}
