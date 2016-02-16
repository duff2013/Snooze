#Snooze v5.5.2

<h1>General Usage</h1>
<h4>Teensyduino 1.27 and above only!!!</h4>
<h3>SnoozeBlock -</h3>
```SnoozeBlock class is used to configure what wakeups you want. You can define many SnoozeBlocks classes for different configurations.```

Function                     | about                        |
-----------------------------| ---------------------------- |
pinMode                      | Setting Pin, Comapre, TSI wakeups. Pins include type/mode and interrupt kind: RISING,FALLING,CHANGE,LOW,HIGH. Comapre is voltage crossing wakeup. TSI is capacentance threshold. |
setTimer                     | Low Power Timer: 1ms to 65535ms in 1ms granulity. |
setAlarm                     | RTC, can set seconds, minutes and hours til wake. | 
setLowVoltage                | Wakeup teensy if low voltage detected, not implemented yet. |
setPeripheral                | Turn on/off modules, adc/serial/usb etc... |
___
<h3>Snooze -</h3>
```Snooze class is the different sleep modes offered```

Function          | about |
----------------- | ---------------------------- |
debug             | Library source code serial debug, not implemented yet.  |
idle              | Spin loop power reduction, puts the cpu in wait mode until the next systick isr. Use in waiting loop. |
sleep             | Most versiatle "sleep mode", use a SnoozeBlock or any interrupt to wake processor. This puts the cpu into VLPW mode. |
deepSleep         | Limited pin wakeups defined by LLWU. |
hibernate         | Same as deepSleep but the USB regualtor is put into low power state. Only limited current can be source from 3.3v output and the voltage will drop to ~2.7v. | 
___
<h5>pinMode: Digital, Touch Sense pins</h5>
```c
/**
*  pinMode - Configures either a digital pin or tsi pin as a wakeup source.
*  
*  Teensy 3.x (digital pin)
*  @param pin  - sleep pins(1-33), deepSleep, hibernate pins (2,4,6,7,9,10,11,13,16,21,22,26,30,33)
*  @param mode - |INPUT|INPUT_PULLUP|OUTPUT|
*  @param val  - |HIGH|LOW|RISING|FALLING|CHANGE|
*
*  Teensy 3.x (tsi pin)
*  @param pin  - sleep pins(does not work), deepSleep, hibernate pins (0,1,15,16,17,18,19,22,23,25,32,33)
*  @param mode - |TSI|
*  @param val  - threshold capacitance value, greater than this value wakes.
*
*  Teensy LC (digital pin)
*  @param pin  - sleep pins(1-23), deepSleep, hibernate pins (2,4,6,7,9,10,11,13,16,21,22)
*  @param mode - |INPUT|INPUT_PULLUP|OUTPUT|
*  @param val  - |HIGH|LOW|RISING|FALLING|CHANGE|
*
*  Teensy LC (tsi pin)
*  @param pin  - sleep pins(does not work), deepSleep, hibernate pins (0,1,3,4,15,16,17,18,19,22,23)
*  @param mode - |TSI|
*  @param val  - threshold capacitance value, greater than this value wakes.
*/
void SnoozeBlock::pinMode( int pin, int mode, int val );
```
___
<h5>pinMode: Compare pins</h5>
```c
/**
*  pinMode - Configures compare wakeup source.
*
*  @param pin  either pin 11 or 12 can be used
*  @param mode |CMP|
*  @param type |RISING|FALLING|
*  @param val  threshold crossing value in volts.
*/
void SnoozeBlock::pinMode( int pin, int mode, int type, double val );
```
___
<h5>setTimer: Low Power Timer</h5>
```c
/**
*  setTimer - wake up period in milli seconds in 1ms granularity.
*
*  @param period 1ms to 65535ms
*/
void SnoozeBlock::setTimer( uint16_t period );
```
___
<h5>setAlarm: RTC</h5>
```c
/**
*  setAlarm - Configures RTC wakeup source. hr, min and sec will be added for the total time.
*
*  @param hours   number of hours in the future.
*  @param minutes number of minutes in the future.
*  @param seconds number of seconds in the future.
*/
void SnoozeBlock::setAlarm( uint8_t hours, uint8_t minutes, uint8_t seconds );
```
___
<h5>source: </h5>
```c
/**
*  source - returns the wakeup source.
*
*  @return Digital pins return pin number, CMP - 34, RTC - 35, LPTMR - 36, TSI - 37.
*/
int SnoozeClass::source( void );
```
___
<h5>idle: </h5>
```c
/**
*  idle - puts processor into wait mode until next interrupt typically systick.
*/
void SnoozeClass::idle( void );
```
___
<h5>sleep: </h5>
```c
/**
*  sleep - most versatile sleep mode. SnoozeBlock configuration or any interrupt can wake the processor.
*
*  @param configuration SnoozeBlock class config.
*
*  @return wakeup source
*/
int SnoozeClass::sleep( SnoozeBlock &configuration );
```
___
<h5>deepSleep: </h5>
```c
/**
*  deepSleep - LLWU is used to handle interrupts that wake. USB regulator is enabled and 3.3V output pin can supply full current (100mA MAX).
*
*  @param configuration SnoozeBlock class config.
*  @param mode          |LLS|VLLS3|VLLS2|VLLS1|
*
*  @return wakeup source
*/
int SnoozeClass::deepSleep( SnoozeBlock &configuration, SLEEP_MODE mode );
```
___
<h5>hibernate: </h5>
```c
/**
*  hibernate - LLWU is used to handle interrupts that wake. USB regulator is disabled and 3.3V output pin can only supply limited current and voltage drops to ~2.7V.
*
*  @param configuration SnoozeBlock class config.
*  @param mode          |LLS|VLLS3|VLLS2|VLLS1|
*
*  @return wakeup source
*/
int SnoozeClass::hibernate( SnoozeBlock &configuration, SLEEP_MODE mode );
```
___
<h5>setPeripheral: </h5>
```c
/**
*  Allows you to turn off/on clocks for different hardware on teensy.
*/
periph_t setPeripheral;
```