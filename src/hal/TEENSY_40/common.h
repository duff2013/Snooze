#if defined(__IMXRT1062__)

#ifndef common_h
#define common_h

#define COMPARE_WAKE    34
#define ALARM_WAKE      35
#define TIMER_WAKE      36
#define TOUCH_WAKE      37

#ifdef __cplusplus
extern "C" {
#endif
    extern volatile int wake_source;
#ifdef __cplusplus
}
#endif


#endif

#endif
