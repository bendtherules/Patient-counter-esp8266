#include <Arduino.h>
#include "buzzer.h"
#include "myOTA.h"
#include "myDisplay.h"
#include <assert.h>
#include <IRrecv.h>
#include <IRremoteESP8266.h>
#include <IRac.h>
#include <IRtext.h>
#include <IRutils.h>

#define LED_BUILTIN D4

// BUZZER Setup
#define BUZZER_PIN D7

// IR Remote
const uint8_t kRecvPin = D6;
const uint16_t kCaptureBufferSize = 1024;
#if DECODE_AC
// Some A/C units have gaps in their protocols of ~40ms. e.g. Kelvinator
// A value this large may swallow repeats of some protocols
const uint8_t kTimeout = 50;
#else   // DECODE_AC
// Suits most messages, while not swallowing many repeats.
const uint8_t kTimeout = 15;
#endif  // DECODE_AC

const uint16_t kMinUnknownSize = 12;
const uint8_t kTolerancePercentage = 40; 
#define LEGACY_TIMING_INFO false

IRrecv irrecv(kRecvPin, kCaptureBufferSize, kTimeout, true);

decode_results results;

unsigned long KEYPAD_NUMBER_MAX_DELAY = 2000;

#define NUMBER_DEFAULT 1
uint8_t currentNumber = NUMBER_DEFAULT;

// Color remote codes
// const String KEY_1 = "0xF710EF";
// const String KEY_2 = "0xF7906F";
// const String KEY_3 = "0xF750AF";
// const String KEY_4 = "0xF730CF";
// const String KEY_5 = "0xF7B04F";
// const String KEY_6 = "0xF7708F";
// const String KEY_7 = "0xF708F7";
// const String KEY_8 = "0xF78877";
// const String KEY_9 = "0xF748B7";
// const String KEY_0 = "0xF7A857";
// const String KEY_A = "0xF7D02F";
// const String KEY_B = "0xF7F00F";
// const String KEY_C = "0xF7C837";
// const String KEY_D = "0xF7E817";
// const String KEY_ON = "0xF7C03F";
// const String KEY_OFF = "0xF740BF";

// Numerical remote code
const String KEY_1 = "0x1FE50AF";
const String KEY_2 = "0x1FED827";
const String KEY_3 = "0x1FEF807";
const String KEY_4 = "0x1FE30CF";
const String KEY_5 = "0x1FEB04F";
const String KEY_6 = "0x1FE708F";
const String KEY_7 = "0x1FE00FF";
const String KEY_8 = "0x1FEF00F";
const String KEY_9 = "0x1FE9867";
const String KEY_0 = "0x1FEE01F";
const String KEY_A = "0x1FEC03F";
const String KEY_B = "0x1FE40BF";
const String KEY_C = "0x1FE807F";
const String KEY_D = "0x1FE20DF";
const String KEY_ON = "0x1FE48B7";
const String KEY_OFF = "0x1FE48B7";
const String KEY_MODE = "0x1FE58A7";


char lastPressedKey = '\0';
long lastMillis = millis();
bool shouldIgnoreLast = false;
void handleKeyPress(char pressedKey) {
  long currentMillis = millis();
  bool isSecondKeyPress = (currentMillis - lastMillis) < KEYPAD_NUMBER_MAX_DELAY;
  if (pressedKey) {
    Serial.printf("Pressed: %c\n", pressedKey);
    if (pressedKey == 'A') {
      currentNumber = (currentNumber < 99) ? (currentNumber + 1): 99;
      Serial.printf("currentNumber: %d\n", currentNumber);
      playBuzzer(BUZZER_PIN, TONE_BEEP);
      showNumber(currentNumber);
    }
    if (pressedKey == 'B') {
      currentNumber = (currentNumber > 0) ? (currentNumber - 1): 0;
      Serial.printf("currentNumber: %d\n", currentNumber);
      playBuzzer(BUZZER_PIN, TONE_BEEP);
      showNumber(currentNumber);
    }
    if (pressedKey == 'C') {
      playBuzzer(BUZZER_PIN, TONE_SARE_JAHA_SE_ACCHA);
    }
    if (pressedKey == 'D') {
      currentNumber = NUMBER_DEFAULT;
      Serial.printf("currentNumber: %d\n", currentNumber);
      playBuzzer(BUZZER_PIN, TONE_BEEP);
      showNumber(currentNumber);
    }
    if (pressedKey == 'R') {
      Serial.println("Pressed restart. Rstarting..");
      Serial.flush();
      playBuzzer(BUZZER_PIN, TONE_BEEP_LONG);
      ESP.restart();
    }
    if (pressedKey == 'U') {
      Serial.println("Pressed update. Updating via remote server..");
      playBuzzer(BUZZER_PIN, TONE_BEEP_TWICE);
      handleOTARemote();
    }
    // If pressed number
    uint8_t currentNumberEntered = pressedKey - '0';
    uint8_t lastNumberEntered = lastPressedKey - '0';
    if (currentNumberEntered <= 9) {
      if (isSecondKeyPress && (lastNumberEntered <= 9) && !shouldIgnoreLast) {
        currentNumberEntered = lastNumberEntered*10 + currentNumberEntered;
        Serial.printf("Second number, total: %d, lastPressedKey:%c\n", currentNumberEntered, lastPressedKey);
        playBuzzer(BUZZER_PIN, TONE_BEEP_TWICE);
        // Stop 3rd number from adding to 2nd number
        shouldIgnoreLast = true;
      } else {
        Serial.printf("First number, total: %d\n", currentNumberEntered);
        playBuzzer(BUZZER_PIN, TONE_BEEP);
        shouldIgnoreLast = false;
      }
      currentNumber = currentNumberEntered;
      showNumber(currentNumber);
    }
  }
}

void setup() {
  Serial.begin(115200);
  setupDisplay();

  showNumber(currentNumber);

  // playBuzzer(BUZZER_PIN);

  assert(irutils::lowLevelSanityCheck() == 0);
  Serial.printf("\n" D_STR_IRRECVDUMP_STARTUP "\n", kRecvPin);
  #if DECODE_HASH
  // Ignore messages with less than minimum on or off pulses.
  irrecv.setUnknownThreshold(kMinUnknownSize);
  #endif  // DECODE_HASH
  irrecv.setTolerance(kTolerancePercentage);  // Override the default tolerance.
  irrecv.enableIRIn();  // Start the receiver
  
  setupOTALocal();
}

void loop() {
  // Check if the IR code has been received.
  if (!shouldIgnoreLast && (millis() - lastMillis > KEYPAD_NUMBER_MAX_DELAY)) {
    playBuzzer(BUZZER_PIN, TONE_BEEP_TWICE);
    shouldIgnoreLast = true;
  }
  if (irrecv.decode(&results)) {
    // Display the basic output of what we found.
    Serial.print(resultToHumanReadableBasic(&results));
    Serial.println();    // Blank line between entries

    char pressedKey = '\0'; // default ignore
    if (results.decode_type == NEC) {
      String hexKey = resultToHexidecimal(&results);
      Serial.printf("Received hex: %S\n", hexKey);
      if (hexKey == KEY_1) {
        pressedKey = '1';
      } else if (hexKey == KEY_2) {
        pressedKey = '2';
      } else if (hexKey == KEY_3) {
        pressedKey = '3';
      } else if (hexKey == KEY_4) {
        pressedKey = '4';
      } else if (hexKey == KEY_5) {
        pressedKey = '5';
      } else if (hexKey == KEY_6) {
        pressedKey = '6';
      } else if (hexKey == KEY_7) {
        pressedKey = '7';
      } else if (hexKey == KEY_8) {
        pressedKey = '8';
      } else if (hexKey == KEY_9) {
        pressedKey = '9';
      } else if (hexKey == KEY_0) {
        pressedKey = '0';
      } else if (hexKey == KEY_A) {
        pressedKey = 'A';
      } else if (hexKey == KEY_B) {
        pressedKey = 'B';
      } else if (hexKey == KEY_C) {
        pressedKey = 'C';
      } else if (hexKey == KEY_D) {
        pressedKey = 'D';
      } else if (hexKey == KEY_ON) {
        pressedKey = 'R';
      } else if (hexKey == KEY_OFF) {
        pressedKey = 'R';
      } else if (hexKey == KEY_MODE) {
        pressedKey = 'U';
      }
      if (pressedKey != '\0') {
        handleKeyPress(pressedKey);
      }
      lastPressedKey = pressedKey;
      lastMillis = millis();
    }
    yield();             // Feed the WDT (again)
  }

  handleOTALocal();
}
