#include <Arduino.h>

const float songSpeed = 1.0;
#define NOTE_D3 147
#define NOTE_C4 262
#define NOTE_D4 294
#define NOTE_E4 330
#define NOTE_F4 349
#define NOTE_G4 392
#define NOTE_G_black_4 415
#define NOTE_A4 440
#define NOTE_A_black_4 466
#define NOTE_B4 494
#define NOTE_C5 523
#define NOTE_C_black_5 554
#define NOTE_D5 587
#define NOTE_D_black_5 622
#define NOTE_E5 659
#define NOTE_F5 698
#define NOTE_F_black_5 740
#define NOTE_G5 784
#define NOTE_G_black_5 830
#define NOTE_A5 880
#define NOTE_A_black_5 932
#define NOTE_B5 988
#define NOTE_D6 1175

#define rest 10

enum TONE_TYPE {
  TONE_BEEP,
  TONE_BEEP_TWICE,
  TONE_BEEP_ONE_EIGHT,
  TONE_BEEP_LONG,
  TONE_SARE_JAHA_SE_ACCHA,
};

void playBuzzer(uint8_t pin, TONE_TYPE toneType) {
  // Music notes of the song, 0 is a rest/pulse
  int notes[] = {
    // NOTE_E5,0,NOTE_E5,0,
    // NOTE_D_black_5,0,NOTE_C_black_5,0,NOTE_D_black_5,
    // 0,NOTE_C_black_5,0,NOTE_C_black_5,0,

    // NOTE_G_black_4,0,NOTE_A_black_4,0,NOTE_C_black_5,0,NOTE_D_black_5,
    // 0,NOTE_F5,0,NOTE_F_black_5,0,NOTE_F5,0,NOTE_F5,0,

    // NOTE_F5,0,NOTE_F_black_5,0,NOTE_G_black_5,0,NOTE_F_black_5,0,
    // NOTE_F5,0,NOTE_F_black_5,0,NOTE_A_black_5,0,NOTE_G_black_5,0,

    // NOTE_F5,0,NOTE_F_black_5,0,NOTE_G_black_5,0,
    // NOTE_F_black_5,0,NOTE_E5,0,NOTE_D_black_5,0,
    // NOTE_C_black_5,0,NOTE_C_black_5,0,NOTE_C5,0,
    // NOTE_A_black_4,0,NOTE_G_black_4,0,


    // NOTE_E5,0,NOTE_E5,0,
    // NOTE_D_black_5,0,NOTE_C_black_5,0,NOTE_D_black_5,
    // 0,NOTE_C_black_5,0,NOTE_C_black_5,0,

    // NOTE_G_black_4,0,NOTE_A_black_4,0,NOTE_C_black_5,0,NOTE_D_black_5,
    // 0,NOTE_F5,0,NOTE_F_black_5,0,NOTE_F5,0,NOTE_F5,0,

    // NOTE_F5,0,NOTE_F_black_5,0,NOTE_G_black_5,0,NOTE_F_black_5,0,
    // NOTE_F5,0,NOTE_F_black_5,0,NOTE_A_black_5,0,NOTE_G_black_5,0,

    NOTE_F5,0,NOTE_F_black_5,0,NOTE_G_black_5,0,
    NOTE_F_black_5,0,NOTE_E5,0,NOTE_D_black_5,0,
    NOTE_C_black_5,0,NOTE_C_black_5,0,NOTE_C5,0,
    NOTE_A_black_4,0,NOTE_G_black_4,0,  
  };
  // Durations (in ms) of each music note of the song
  // Quarter Note is 250 ms when songSpeed = 1.0
  int durations[] = {
    // 300,200,200,300,
    // 200,100,200,300,100,
    // 100,250,200,250,400,

    // 200,100,200,100,200,300,100,
    // 10,300,200,300,300,250,300,200,400,

    // 200,100,200,100,250,300,250,200,
    // 200,100,200,100,250,100,300,200,

    // 200,100,200,100,250,300,
    // 200,100,300,300,200,100,
    // 300,300,300,300,200,100,
    // 200,100,300,400,

    // 300,200,200,300,
    // 200,100,200,300,100,
    // 100,250,200,250,400,

    // 200,100,200,100,200,300,100,
    // 10,300,200,300,300,250,300,200,400,

    // 200,100,200,100,250,300,250,200,
    // 200,100,200,100,250,100,300,200,

    200,100,200,100,250,300,
    200,100,300,300,200,100,
    300,300,300,300,200,100,
    200,100,300,400,
      
  };

  pinMode(pin, OUTPUT);
  if (toneType == TONE_SARE_JAHA_SE_ACCHA) {
    const int totalNotes = sizeof(notes) / sizeof(int);
    // Loop through each note
    for (int i = 0; i < totalNotes; i++)
    {
      const int currentNote = notes[i];
      float wait = durations[i] / songSpeed;
      // Play tone if currentNote is not 0 frequency, otherwise pause (noTone)
      if (currentNote != 0)
      {
        tone(pin, notes[i], wait); // tone(pin, frequency, duration)
      }
      else
      {
        noTone(pin);
      }
      // delay is used to wait for tone to finish playing before moving to next loop
      delay(wait);
    }
    noTone(pin);
  } else if (toneType == TONE_BEEP) {
    digitalWrite(pin, HIGH);
    delay(100);
    digitalWrite(pin, LOW);
  } else if (toneType == TONE_BEEP_TWICE) {
    digitalWrite(pin, HIGH);
    delay(80);
    digitalWrite(pin, LOW);
    delay(50);
    digitalWrite(pin, HIGH);
    delay(80);
    digitalWrite(pin, LOW);
  } else if (toneType == TONE_BEEP_ONE_EIGHT) {
    {
      digitalWrite(pin, HIGH);
      delay(400);
      digitalWrite(pin, LOW);
      delay(200);
    }
    for (size_t tmpIndex = 0; tmpIndex < 8; tmpIndex++)
    {
      digitalWrite(pin, HIGH);
      delay(80);
      digitalWrite(pin, LOW);
      delay(40);
    }
  } else if (toneType == TONE_BEEP_LONG) {
    digitalWrite(pin, HIGH);
    delay(500);
    digitalWrite(pin, LOW);
  }
}
