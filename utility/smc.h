/*
 *  smc.h
 *  Teensy3
 *
 */

#ifndef __SMC_H__
#define __SMC_H__
/********************************************************************/
#include "kinetis.h"
#include "kinetis_ext.h"
/********************************************************************/
#ifdef __cplusplus
extern "C" {
#endif
    // function prototypes
    void enter_wait(void);
    void enter_stop(void);
    void enter_vlpr(char lpwui_value);
    void exit_vlpr(void);
    void enter_vlps(char lpwui_value);
    void enter_lls(void);
    void enter_vlls3(void);
    void enter_vlls2(void);
    void enter_vlls1(void);
    void enter_vlls0(void);
    void enter_vlls0_nopor(void);
    void disable_ports(void);
    void disable_ports_partial(void);
    void enable_lpwui(void);
    void disable_lpwui(void);
    void stop(void);
    void wait(void);
#ifdef __cplusplus
}
#endif
/********************************************************************/
#endif /* __SMC_H__ */
