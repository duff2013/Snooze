><b>Updated (1/12/16 v5.0.1)</b><br>
* Fix for new Teensy Reg Defs.<br>

><b>Updated (10/27/15 v5.0.0)</b><br>
* Now CPU Scaling is supported by the REDUCED_CPU_BLOCK macro.<br>
* Reset systick when cpu goes to 2 MHz.<br>

><b>Updated (9/30/15 v5.4.2)</b><br>
* Fixed Compiling TeensyLC PDB register in peripheral.h.<br>
* Fixed TeensyLC warnings.<br>

><b>Updated (9/7/15 v5.4.1)</b><br>
* Removed debug stuff.<br>
* Fixed Properites Catagory warning.<br>

><b>Updated (6/4/15 v5.4)</b><br>
* Made setTimer "time" param uint16_t.<br>
* Fixed sleep function tsi wakeup for Teensy LC, Teensy 3.x not working yet maybe never.<br>
* Removed unused code.<br>

><b>Updated (5/31/15 v5.3)</b><br>
* Fixed sleep function low power timer.<br>
* Fixed digital pin wake on LOW or HIGH interrupt type, now if pin interrupt type is configured LOW and the pin is LOW when going into sleep it will wake immediately, the same goes for if it is HIGH interrupt type.<br>

><b>Updated (5/28/15 v5.2.1)</b><br>
* Remove debug stuff.<br>

><b>Updated (5/19/15 v5.2)</b><br>
* Better documentation now but still needs work.<br>
* Fixed TSI wakeup number for sleep interrupts.<br>
* Digital Pins configurations only now change interrupt vector for the selected pin for the sleep function. This was do to Adafruit_nRF8001 library needing to keep its interrupt config intact. Possible problems still exists if a SnoozeBlock pin shares the same vector as the library or user code, not really sure how to handle that.
* Fixed LPTMR not resseting its compare time if other wakeup interrupt came before it. This would make the LPTMR not wait the full param time to fire its interrupt in next sleep mode.
* All library interrupts priorities raised to (current - 16) and LLWU priority raised to 32 because calling a function from an isr will block a lower priority interrupt.

><b>Updated (4/24/15 v5.1)</b><br>
* Fixed bug in Hibernate not giving the right wake up source and stopped setting unused registers in LLWU irq, Thanks to https://forum.pjrc.com/members/39437-rhubley.
* Functions sleep, deepSleep and hibernate now return wake source.
* Code cleanup and various code refactoring.
* Added library properties.
* Now compatible with Teensyduino 1.22.

><b>Updated (3/12/15)</b><br>
* Fixed again setting digital pin for Teensy-3.x/LC.
* Teensy LC pin 17 is set automatically to OUTPUT->LOW to reduce power.
* Now TSI work with Teensy-LC for deepSleep and hibernate.
* Compare some what works for Teensy-LC but will further modified in next release.
* Examples now are now Teensy-LC compatible.

><b>Updated (3/9/15)</b><br>
* Fixed setting digital pin for Teensy LC.
* Also need to set Teensy LC pin 17 to OUTPUT->HIGH.

><b>Updated</b><br>
* Initial TeensyLC support, not all wakeup sources are available, RTC, TSI, CMP and some digital pins are not waking the LC up either.

<h3>Initial Release:</h3>
1.  Trimmed down and simplified, tried to mimic standard Arduino conventions for setting wakeup pins etc..<br>
2.  Since Teensyduino now can be programed at many different CPU speeds, dynamically changing cpu speeds is no longer supported, maybe in future I can bring it back if wanted.<br>
3.  Now the teensy can sleep at ~10uA with hibernate function. Special care is need when using this function. Must uncomment the define statement to use.<br>
4.  All sleep mode work the same with regards to setting up wakeup events. A separate class (SnoozeBlock) holds all configurations similar to LowPower_Teensy3's sleepBlock but I hope better and easier to implement.<br>