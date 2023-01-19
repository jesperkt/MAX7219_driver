/*
  Library for 7-Segment Display driver using MAX7219 IC. 
  
  Created Jan. 11. 2023
  By Jesper Thøgersen
*/

#include "seven_segment_MAX7219.h"

/*  Control register (Refer to Table 2 of MAX7219 documentation) 
    Some elements have been left out.
*/
#define CTRL_DECODEMODE 0x09
#define CTRL_INTENSITY 0x0A
#define CTRL_SCANLIMIT 0x0B
#define CTRL_SHUTDOWN 0x0C
#define CTRL_DISPLAYTEST 0x0F
/* End of control register*/

#define ASCII_OFFSET 45   // Offset used to map char to index in asciiTableRef (see header file)
#define DISPLAY_LENGTH 8  // One module has 8 digits/segments
/* 
Class initializer 
Takes 4 inputs:
pinDIN : Corresponds to DIN on the 7-segment board. Must be a vlid SPI MOSI pin. 
pinCLK : Corresponds to CLK on the 7-segment board. Must be a valid SPI CLK pin.
pinCS  : Corresponds to CS on the 7-segment board. Can be any digital pin.
*/
SegmentDriver::SegmentDriver(int pinDIN, int pinCLK, int pinCS) {
  MOSI = pinDIN;
  CLK = pinCLK;
  CS = pinCS;

  pinMode(MOSI, OUTPUT);
  pinMode(CLK, OUTPUT);
  pinMode(CS, OUTPUT);

  digitalWrite(CS, HIGH); // Pull CS high to lock (latch) display

  // Disable display test
  if (sendSPI(0, CTRL_DISPLAYTEST) != 0) {
    Serial.println("Test display failed.");
  }

  // Set scan limit to max use (7)
  if (setScanLimit(7) != 0) {
    Serial.println("Setting scan limit failed.");
  }

  // Set brightness low value
  if (setBrightness(15) != 0) {
    Serial.println("Setting brightness failed.");
  }

  // Power on the display
  turnOn();

  // Clear the display
  clear();

  delay(1000);
}

// Turn on the display
int SegmentDriver::turnOn() {
  return sendSPI(1, CTRL_SHUTDOWN);
}

// Turn of the display
int SegmentDriver::turnOff() {
  return sendSPI(0, CTRL_SHUTDOWN);
}

// Clear the display, by settings all eight characters to space (" ")
void SegmentDriver::clear() {
  for (int i = 1; i < DISPLAY_LENGTH + 1; i++) {
    setString("        ");
  }
}

/*
Set the brightness of the display
Input: value (integer between 0-15)
*/
int SegmentDriver::setBrightness(int value) {
  if (value < 0 || value > 15) {
    return -1;
  }

  return sendSPI(value, CTRL_INTENSITY);
}

/*
Set the scan limit of the display
Input: value (integer between 0-7)
*/
int SegmentDriver::setScanLimit(int limit) {
  if (limit < 0 || limit > 7) {
    return -1;
  }

  return sendSPI(limit, CTRL_SCANLIMIT);
}

/*
Set a single character of the display
Inputs:
place: integer between 0-7
chr: any character. If the character cannot be printed (e.g. X, blank is printed)

returns 0 if OK. Anything other than 0 is assumed an error. 
*/
int SegmentDriver::setChar(int place, char chr) {

  byte val = (byte)chr;

  // Convert to upper case, if the char is lower case.
  // This corresponds to subtracting 32 from the decimal value of the given char.
  if (val >= 97 && val <= 122) {
    val -= 32;
  }

  // We further subtract the ASCII_OFFSET to achieve the right index into the asciiTableRef
  val -= ASCII_OFFSET;

  // If the final value is not an index in the asciiTableRef, we just use val = 2, 
  // which is equivivalent to a space (" ")
  if (val < 0 || val > sizeof(asciiTableRef)) {
    val = 2;
  }

  // Finally send the SPI command with the proper byte and and the right place
  return sendSPI(pgm_read_byte_near(asciiTableRef + val), place + 1);
}


/*
Display a string on the 7-segment display
Inputs:
string: A string.

If the string is longer than 8 characters, only the first 8 characters are shown.

returns 0 if OK. Anything other than 0 is assumed an error. 
*/
int SegmentDriver::setString(String string) {
  int len = (sizeof(string) > 9) ? 8 : sizeof(string) - 1;

  for (int i = 0; i < len; i++) {
    setChar((DISPLAY_LENGTH - 1) - i, string[i]);
  }
  return 0;
}

/*
SPI command for interfacing with the MAX7219 IC. 
Inputs:
data: a single byte of data (usually this is one charactar)
instruction: a single byte representing the control mode to be used. 
  (Refer to the Control Register mapping in the top of the file)

returns 0 if OK. Anything other than 0 is assumed an error. 
*/
int SegmentDriver::sendSPI(volatile byte data, volatile byte instruction) {
  // Pull CS pin low to initiate communication
  digitalWrite(CS, LOW);

  // Send the instruction byte to the MAX7219
  shiftOut(MOSI, CLK, MSBFIRST, instruction);

  // Send the data byte to the MAX7219
  shiftOut(MOSI, CLK, MSBFIRST, data);

  // Pull CS high to latch the display
  digitalWrite(CS, HIGH);

  return 0;
}