/**********************************************************
  This sketch shows the use of SPI Driver used in Snooze
  along with the standard SD and Bounce libraries. In this
  example the Teensy SD Data-Logger will deepSleep until you
  push the button (WAKEUP_BUTTON) which will wake the Teensy
  and start logging data until the button is released in
  which the Teensy will go back into deepSleep.

  The SPI Driver configures the SDIO or SPI pins to minimize
  leakage current during deepSleep and reconfigure the pins
  back after wake up.

  Supported Micros: T-LC/3.x

  Teensy 4.0 hibernate wakes up with a reset so this sketch
  will not work.
 ***********************************************************/

/***********************************************************
  SNOOZE
 ***********************************************************/
#include <Snooze.h>
// Load drivers
// SnoozeUSBSerial driver does what Serial does like "print", -
// "println", "prinf", etc and also handles the effects of using -
// the Arduino Serial monitor and sleeping. Use it when you -
// want to print to serial monitor for sleeping applications.
SnoozeUSBSerial   usb;
SnoozeDigital     digital;
SnoozeSPI         sdCard;

SnoozeBlock config(usb, digital, sdCard);
/***********************************************************
  SD
 ***********************************************************/
#include <SD.h>
#include <SPI.h>

// set up variables using the SD utility library functions:
Sd2Card  card;
SdVolume volume;
SdFile   root;

const int chipSelect = BUILTIN_SDCARD;
/***********************************************************
  BOUNCE
 ***********************************************************/
#include <Bounce.h>
const uint8_t WAKEUP_BUTTON = 7;
// use bounce for pin 7, debounce of 5ms
Bounce button = Bounce(WAKEUP_BUTTON, 5);
/***********************************************************
  TIME
 ***********************************************************/
#include <TimeLib.h>
elapsedMillis rtcMilliSeconds;
/***********************************************************/

uint32_t dataCount;
// ----------------------------------------------------------------------------------------------
void setup() {
  //
  setSyncProvider(getTeensy3Time);
  //
  dataCount = 0;
  // Configure LED Pin
  pinMode(LED_BUILTIN, OUTPUT);
  // Configure pin 21 for bounce library
  pinMode(WAKEUP_BUTTON, INPUT_PULLUP);

  /******************Snooze Driver Config******************/
  // Configure pin WAKEUP_BUTTON for Snooze wakeup, also -
  // used by bounce but no need to reconfigure the pin -
  // after waking Snooze does it for you.
  digital.pinMode(WAKEUP_BUTTON, INPUT_PULLUP, FALLING);//pin, mode, type
  // Configure SD Card for Low Power operation
  sdCard.setClockPin( BUILTIN_SDCARD );
  /****************End Snooze Driver Config****************/

  // UNCOMMENT THESE TWO LINES FOR TEENSY AUDIO BOARD:
  //SPI.setMOSI(7);  // Audio shield has MOSI on pin 7
  //SPI.setSCK(14);  // Audio shield has SCK on pin 14

  // Wait for Serial connection to open.
  while (!usb);
  delay(100);

  usb.println("Snooze SD Card Push Button Hold Data-Logger Example");
  usb.println("---------------------------------------------------");
  // see if the card is present and can be initialized:
  SD.begin(chipSelect);
  if (!card.init(SPI_HALF_SPEED, chipSelect)) {
    // don't do anything more:
    while (1) {
      usb.println("Card failed, or not present!");
      blink();
      blink();
    }
  }
  elapsedMillis time = 0;
  while (time < 1000) {
    usb.print(".");
    delay(200);
  }
  usb.println();
  usb.print("-> ");
  usb.println("SD Card Initialized.");
  sdCardInfo();
  delay(1000);
}
// ------------------------------------------------------------------------
void loop() {
  sleepDataLogger();
  handleButtonHold();
}
// ------------------------------------------------------------------------
inline void sleepDataLogger() {
SLEEP:
  digitalWriteFast(LED_BUILTIN, LOW);
  usb.println("Logger Will Sleep Now...");
  // delay to finish usb print before sleeping
  delay(5);
  // you need to update bounce before sleeping.
  button.update();
  // put Teensy into low power sleep mode
  Snooze.hibernate( config );
  digitalWriteFast(LED_BUILTIN, HIGH);

  // check for 300ms that the button is held
  bool hold = threeHundredMilliSecondHoldCheck();

  // if not held for 300ms go back to sleep
  if (hold == false) goto SLEEP;

  usb.println("Data Logger now awake, ready to log data.");
}
// ------------------------------------------------------------------
// Make sure the button is help low for aleast 300ms before data logging.
bool threeHundredMilliSecondHoldCheck() {
  button.update();
  // Pin is configured as PULLUP so 0 means the button has been
  // pushed.
  while (button.read() == 0) {
    // Monitor the pin hold duration.
    if (button.duration() > 300) return true;
    // Update bounce library.
    button.update();
  }
  return false;
}
// ------------------------------------------------------------------------
void handleButtonHold() {
  button.update();
  if (button.read() == HIGH) {
    usb.println("No Logging of Data!!! Button released before logging could commence.");
    return;
  }
  usb.println("Logging data");
  button.update();
  elapsedMillis log_time = 0;
  while (button.read() == LOW) {
    if (log_time >= 99) {
      log_time = 0;
      logData(dataCount++);
    }
    button.update();
  }
  digitalWrite(LED_BUILTIN, LOW);
}
// ------------------------------------------------------------------------
void logData(uint32_t count) {
  // make a string for assembling the data to log:
  String dataString;//"Time\tAna1\tAna2\tAna3\r\n";
  // read three sensors and append to the string:
  dataString = digitalClockDisplay(rtcMilliSeconds - 1);
  dataString += "\t";
  dataString += String(count);
  dataString += String("\t");
  for (int analogPin = 0; analogPin < 3; analogPin++) {
    dataString += analogRead(analogPin);
    if ( analogPin < 2) dataString += "\t";
  }
  dataString += "\r\n";
  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  File dataFile = SD.open("datalog.txt", FILE_WRITE);

  // if the file is available, write to it:
  if (dataFile) {
    dataFile.print(dataString);
    dataFile.close();
    // print to the serial port too:
    int len = dataString.length();
    usb.print(dataString);
  }
  // if the file isn't open, pop up an error:
  else {
    usb.println("error opening datalog.txt.");
  }
}
// ------------------------------------------------------------------------
void sdCardInfo() {
  // print the type of card
  usb.print("-> Card type: ");

  switch (card.type()) {
    case SD_CARD_TYPE_SD1: {
        usb.println("SD1");
        break;
      }
    case SD_CARD_TYPE_SD2: {
        usb.println("SD2");
        break;
      }
    case SD_CARD_TYPE_SDHC: {
        usb.println("SDHC");
        break;
      }
    default: {
        usb.println("Unknown");
        break;
      }
  }

  // Now we will try to open the 'volume'/'partition' - it should be FAT16 or FAT32
  if (!volume.init(card)) {
    usb.println("Could not find FAT16/FAT32 partition.\nMake sure you've formatted the card");
    return;
  }

  // print the type and size of the first FAT-type volume
  uint32_t volumesize;
  usb.print("\nVolume type is FAT");
  usb.println(volume.fatType(), DEC);
  usb.println();
  // clusters are collections of blocks
  volumesize = volume.blocksPerCluster();
  // we'll have a lot of clusters
  volumesize *= volume.clusterCount();
  if (volumesize < 8388608ul) {
    usb.print("Volume size (bytes): ");
    // SD card blocks are always 512 bytes
    usb.println(volumesize * 512);
  }
  usb.print("Volume size (Kbytes): ");
  volumesize /= 2;
  usb.println(volumesize);
  usb.print("Volume size (Mbytes): ");
  volumesize /= 1024;
  usb.println(volumesize);

  usb.println("\nFiles found on the card (name, date and size in bytes): ");
  root.openRoot(volume);

  // list all files in the card with date and size
  root.ls(LS_R | LS_DATE | LS_SIZE);
  delay(100);
}
// ------------------------------------------------------------------------
void rtc_seconds_isr() {
  // zero out millisecond counter
  rtcMilliSeconds = 0;
}

time_t getTeensy3Time() {
  // set rtc seconds interrupt
  //RTC_IER |= 0x10;
  rtcMilliSeconds = 0;
  // enable interrupt
  //NVIC_ENABLE_IRQ(IRQ_RTC_SECOND);
  return Teensy3Clock.get();
}
// ------------------------------------------------------------------------
String digitalClockDisplay(elapsedMillis millis_t) {

  String time;

  if (hour() < 10) time += 0;
  //< 10 ? '0' + hour() : hour();
  time += hour();
  time += ":";

  if (minute() < 10) time += 0;
  //< 10 ? '0' + minute() : minute();
  time += minute();
  time += ":";

  if (second() < 10) time += 0;
  time += second();
  time += ".";

  if (millis_t < 100) time += 0;
  if (millis_t < 10) time += 0;
  time += (int)millis_t;

  return time;
}
// ------------------------------------------------------------------------
void blink() {
  digitalWriteFast(LED_BUILTIN, HIGH);
  delay(15);
  digitalWriteFast(LED_BUILTIN, LOW);
  delay(15);
}
