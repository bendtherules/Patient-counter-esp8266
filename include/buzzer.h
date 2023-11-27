#include <Arduino.h>

enum TONE_TYPE {
  TONE_BEEP,
  TONE_BEEP_TWICE,
  TONE_BEEP_ONE_EIGHT,
  TONE_BEEP_LONG,
  TONE_SARE_JAHA_SE_ACCHA,
};

void playBuzzer(uint8_t pin, TONE_TYPE toneType);
