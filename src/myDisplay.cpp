#include <Arduino.h>
#include <FastLED.h>
#include <LiquidCrystal_I2C.h>

// Skip indicator led
#define SKIP_LEDS 1

#define ACTUAL_LEDS 64

// How many leds in your strip?
#define NUM_LEDS ACTUAL_LEDS + SKIP_LEDS

// For led chips like WS2812, which have a data line, ground, and power, you just
// need to define DATA_PIN.
#define DATA_PIN D8

// Effects
#define FONT_WIDTH 4
#define FONT_HEIGHT 8
#define LED_WIDTH 8
#define LED_HEIGHT 8

CRGB myRed = CRGB(255, 10, 10);
CRGB myPink = CRGB(255, 30, 10);
CRGB myBlue = CRGB(10, 10, 250);

// Define the array of leds
CRGB leds[NUM_LEDS];

// LCD setup
#define LCD_COLUMNS 16
#define LCD_ROWS 2
#define PAGE   ((LCD_COLUMNS) * (LCD_ROWS))
LiquidCrystal_I2C lcd(PCF8574_ADDR_A21_A11_A01, 4, 5, 6, 16, 11, 12, 13, 14, POSITIVE);

bool FONT_NUMBERS[10][FONT_HEIGHT][FONT_WIDTH] = {
  { // 0
    {1, 1, 1, 1},
    {1, 0, 0, 1},
    {1, 0, 0, 1},
    {1, 0, 0, 1},
    {1, 0, 0, 1},
    {1, 0, 0, 1},
    {1, 0, 0, 1},
    {1, 1, 1, 1},
  },
  { // 1
    {0, 0, 1, 0},
    {0, 1, 1, 0},
    {1, 0, 1, 0},
    {0, 0, 1, 0},
    {0, 0, 1, 0},
    {0, 0, 1, 0},
    {0, 0, 1, 0},
    {1, 1, 1, 1},
  },
  { // 2
    {1, 1, 1, 1},
    {1, 0, 0, 1},
    {0, 0, 0, 1},
    {0, 0, 0, 1},
    {1, 1, 1, 1},
    {1, 0, 0, 0},
    {1, 0, 0, 0},
    {1, 1, 1, 1},
  },
  { // 3
    {0, 1, 1, 1},
    {0, 0, 0, 1},
    {0, 0, 0, 1},
    {0, 1, 1, 1},
    {0, 1, 1, 1},
    {0, 0, 0, 1},
    {0, 0, 0, 1},
    {0, 1, 1, 1},
  },
  { // 4
    {1, 0, 0, 0},
    {1, 0, 0, 0},
    {1, 0, 1, 0},
    {1, 0, 1, 0},
    {1, 1, 1, 1},
    {0, 0, 1, 0},
    {0, 0, 1, 0},
    {0, 0, 1, 0},
  },
  { // 5
    {1, 1, 1, 1},
    {1, 0, 0, 0},
    {1, 0, 0, 0},
    {1, 1, 1, 1},
    {0, 0, 0, 1},
    {0, 0, 0, 1},
    {1, 0, 0, 1},
    {1, 1, 1, 1},
  },
  { // 6
    {0, 1, 1, 0},
    {1, 0, 0, 1},
    {1, 0, 0, 0},
    {1, 1, 1, 0},
    {1, 0, 0, 1},
    {1, 0, 0, 1},
    {1, 0, 0, 1},
    {0, 1, 1, 0},
  },
  { // 7
    {1, 1, 1, 1},
    {1, 0, 0, 1},
    {0, 0, 0, 1},
    {0, 0, 1, 0},
    {0, 1, 0, 0},
    {0, 1, 0, 0},
    {0, 1, 0, 0},
    {0, 1, 0, 0},
  },
  { // 8
    {0, 1, 1, 0},
    {1, 0, 0, 1},
    {1, 0, 0, 1},
    {0, 1, 1, 0},
    {0, 1, 1, 0},
    {1, 0, 0, 1},
    {1, 0, 0, 1},
    {0, 1, 1, 0},
  },
  { // 9
    {0, 1, 1, 0},
    {1, 0, 0, 1},
    {1, 0, 0, 1},
    {1, 0, 0, 1},
    {0, 1, 1, 1},
    {0, 0, 0, 1},
    {1, 0, 0, 1},
    {0, 1, 1, 0},
  },
};

enum DISPLAY_POSITION {
  POSITION_LEFT,
  POSITION_RIGHT,
};

void showOutsideNumberSingle(uint8_t number, DISPLAY_POSITION pos, bool show = false) {
  for (size_t tmp_y = 0; tmp_y < FONT_HEIGHT; tmp_y++)
  {
    for (size_t tmp_x = 0; tmp_x < FONT_WIDTH; tmp_x++)
    {
      uint8_t ledIndex = tmp_y * LED_WIDTH + tmp_x + (pos == POSITION_LEFT ? 0 : FONT_WIDTH);
      leds[ledIndex + SKIP_LEDS] = FONT_NUMBERS[number][tmp_y][tmp_x] ? (pos == POSITION_LEFT ? myBlue : myRed) : CRGB::Black;
    }
  }
  if (show) {
    FastLED.show();
  }
}

void cleanDisplay() {
  fill_solid(leds, NUM_LEDS, CRGB::Black);
  fill_solid(leds, SKIP_LEDS, myRed);
}

void showOutsideNumber(uint8_t number) {
  cleanDisplay();
  FastLED.show();

  uint8_t numberRight = number % 10;
  uint8_t numberLeft = min(number / 10, 9);
  Serial.printf("Printing numberRight: %d, numberLeft: %d\n", numberRight, numberLeft);
  showOutsideNumberSingle(numberRight, POSITION_RIGHT);
  if (numberLeft > 0) {
    showOutsideNumberSingle(numberLeft, POSITION_LEFT);
  }
  
  FastLED.show();
}

void clearLCDLine(int line) {               
  lcd.setCursor(0, line);
  for(int n = 0; n < LCD_COLUMNS; n++){
    lcd.print(" ");
  }
  lcd.setCursor(0, line);
}

void showInsideNumber(int number) {
  clearLCDLine(0);
  lcd.print(number);
}

void showNumber(uint8_t number) {
  showOutsideNumber(number);
  showInsideNumber(number);
}

void showInsideInfo(const char* text) {
  clearLCDLine(1);
  lcd.print(text);
}

void setupDisplay() {
  // LED matrix setup
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);  // GRB ordering is assumed
  FastLED.setMaxPowerInVoltsAndMilliamps(5, 400);
  FastLED.setBrightness(255);
  showOutsideNumberSingle(3, POSITION_RIGHT, true);
  delay(10);
  cleanDisplay();
  FastLED.show();

  // LCD setup
  while (lcd.begin(LCD_COLUMNS, LCD_ROWS, LCD_5x8DOTS) != 1) //colums, rows, characters size
  {
    Serial.println(F("PCF8574 is not connected or lcd pins declaration is wrong. Only pins numbers: 4,5,6,16,11,12,13,14 are legal."));
    delay(5000);
  }
  lcd.clear();
}
