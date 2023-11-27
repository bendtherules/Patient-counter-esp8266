#include <Arduino.h>

enum DISPLAY_POSITION {
  POSITION_LEFT,
  POSITION_RIGHT,
};

void setupDisplay();
void showNumber(uint8_t number);