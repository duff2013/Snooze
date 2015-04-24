#Snooze v5.1
>Updated (4/24/15) Fixed bug in Hibernate not giving the right wake up source and stopped setting unused registers in LLWU irq, Thanks to https://forum.pjrc.com/members/39437-rhubley. Functions sleep, deepSleep and hibernate now return wake source. Code cleanup and various code refacrtoring. Added library properties. Now compatible with Teensyduino 1.22.

>Updated (3/12/15) Fixed again setting digital pin for Teensy-3.x/LC. Teensy LC pin 17 is set automatically to OUTPUT->LOW to reduce power. Now TSI work with Teensy-LC for deepSleep and hibernate. Compare some what works for Teensy-LC but will further modified in next release. Examples now are now Teensy-LC compatible.

>Updated (3/9/15) Fixed setting digital pin for Teensy LC. Also need to set Teensy LC pin 17 to OUTPUT->HIGH.

>Updated for initial TeensyLC support, not all wakeup sources are available, RTC, TSI, CMP and some digital pins are not waking the LC up either.

>This library is the latest revision based on my earlier work on the LowPower_Teensy3. I changed the name to not conflict with the Arduino LowPower library plus I think it sounds cooler.

<h4>Compatible with Teensyduino 1.21, older versions might not work.</h4> 

<h3>Initial Release:</h3>
1.  Trimmed down and simplified, tried to mimic standard Arduino conventions for setting wakeup pins etc..<br>
2.  Since Teensyduino now can be programed at many different CPU speeds, dynamically changing cpu speeds is no longer supported, maybe in future I can bring it back if wanted.<br>
3.  Now the teensy can sleep at ~10uA with hibernate function. Special care is need when using this function. Must uncomment the define statement to use.<br>
4.  All sleep mode work the same with regards to setting up wakeup events. A separate class (SnoozeBlock) holds all configurations similar to LowPower_Teensy3's sleepBlock but I hope better and easier to implement.<br>