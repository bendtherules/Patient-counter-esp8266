#include <Arduino.h>

enum DISPLAY_POSITION {
  POSITION_LEFT,
  POSITION_RIGHT,
};

#define LCD_COLUMNS 16
#define LCD_ROWS 2

void setupDisplay();
void showNumber(uint8_t number);
void showInsideInfo(const char* text);