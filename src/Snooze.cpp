#include "Snooze.h"

#ifdef __cplusplus
extern "C" {
#endif
    extern int  source        ( void );
    extern void hal_idle      ( void );
    extern int  hal_source    ( void );
    extern int  hal_sleep     ( void );
    extern int  hal_deepSleep ( void );
    extern int  hal_hibernate ( void );
#ifdef __cplusplus
}
#endif

/*******************************************************************************
 *  idle - Puts the processor into wait mode until next interrupt
 *******************************************************************************/
void SnoozeClass::idle( SNOOZE_BLOCK ) {
    hal_idle( );
}
/*******************************************************************************
 *  sleep - Most versatile sleep mode. Least power savings
 *
 *  @param configuration SnoozeBlock class config.
 *
 *  @return wakeup source
 *******************************************************************************/
int SnoozeClass::sleep( SNOOZE_BLOCK ) {
    SnoozeBlock *p = &configuration;
    p->enableDriver( 1 );
    int source = hal_sleep( );
    p->disableDriver( 1 );
    return source;
}
/*******************************************************************************
 *  deepSleep - Moderate power savings
 *
 *  @param configuration SnoozeBlock class config.
 *
 *  @return wakeup source
 *******************************************************************************/
//int SnoozeClass::deepSleep( SNOOZE_BLOCK, SLEEP_MODE mode ) {
int SnoozeClass::deepSleep( SNOOZE_BLOCK ) {
    SnoozeBlock *p = &configuration;
    p->enableDriver( 2 );
    int source = hal_deepSleep( );
    p->disableDriver( 2 );
    return source;
}
/*******************************************************************************
 *  hibernate - Most power savings.
 *
 *  @param configuration SnoozeBlock class config.
 *
 *  @return wakeup source
 *******************************************************************************/
//int SnoozeClass::hibernate( SNOOZE_BLOCK, SLEEP_MODE mode ) {
int SnoozeClass::hibernate( SNOOZE_BLOCK ) {
    SnoozeBlock *p = &configuration;
    p->enableDriver( 3 );
    int source = hal_hibernate( );
    p->disableDriver( 3 );
    return source;
}
