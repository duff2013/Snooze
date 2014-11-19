#Snooze

>This library is the latest revision based on my earlier work on the LowPowwer_Teensy3. I changed the name to not conflict with the Arduino LowPower library plus I think it sounds cooler.

<h4>Compatible with Teensyduino 1.20</h4> 

<h3>Initial Release:</h3>
1.  Trimmed down and simplified, tried to mimic standard Arduino conventions for setting wakeup pins etc..<br>
2.  Since Teensyduino now can be programed at many different CPU speeds, dynamically changing cpu speeds is no longer supported, maybe in future I can bring it back if wanted.<br>
3.  Now the teensy can sleep at ~10uA with hibernate function. Special care is need when using this function. Must uncomment the define statement to use.<br>
4.  All sleep mode work the same with regards to setting up wakeup events. A separate class (SnoozeBlock) holds all configurations similar to LowPower_Teensy3's sleepBlock but I hope better and easier to implement.<br>


