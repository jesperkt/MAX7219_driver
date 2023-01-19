#ifndef seven_segment_MAX7219_h
#define seven_segment_MAX7219_h

#include <Arduino.h>

const static byte asciiTableRef[] PROGMEM = {
  B00000001, B10000000, B00000000, B01111110, B00110000, B01101101, B01111001, B00110011, B01011011, B01011111, B01110000,
  B01111111, B01111011, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000,
  B00000000, B01110111, B00011111, B00001101, B00111101, B01001111, B01000111, B01111011,
  B00110111, B00110000, B00111000, B00000000, B00001110, B00000000, B00010101, B01111110,
  B01100111, B00000000, B00000101, B01011011, B00001111, B00111110, B00011100, B00000000,
  B00000000, B00111011, B00000000
};

class SegmentDriver {
private:
  int sendSPI(byte value, byte instruction);

  int MOSI;

  int CLK;

  int CS;

public:
  SegmentDriver(int MOSI, int CLK, int CS);
  int turnOn();
  int turnOff();
  int setBrightness(int value);
  int setScanLimit(int limit);
  int setChar(int place, char chr);
  int setString(String string);
  void clear();
};
#endif  //seven_segment_MAX7219_h