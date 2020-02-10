#if defined(__MK64FX512__)

#ifndef common_h
#define common_h

#define COMPARE_WAKE    34
#define ALARM_WAKE      35
#define TIMER_WAKE      36
#define TOUCH_WAKE      37

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif


typedef enum {
    TSI          = 3,
    CMP          = 4,
} PIN_TYPE;

/* powerdown Modes */
typedef enum {
    RUN,
    RUN_LP,
    WAIT,
    VLPW,
    STOP,
    VLPS,
    LLS,
    VLLS3,
    VLLS2,
    VLLS1,
    VLLS0
} POWERDOWN_MODES;

#endif /* common_h */
#endif /* __MK64FX512__ */
