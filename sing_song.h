/*
 * sing_song.h
 * notes are from https://codebender.cc/sketch:238461#Songs%20on%20Piezo.ino,
 * thanks!!!
 */
#ifndef SING_SONG_H
#define SING_SONG_H

#include "constants.h"

const int notes[] = { 0,
  262, 277, 294, 311, 330, 349, 370, 392, 415, 440, 466, 494,
  523, 554, 587, 622, 659, 698, 740, 784, 831, 880, 932, 988,
  1047, 1109, 1175, 1245, 1319, 1397, 1480, 1568, 1661, 1760, 1865, 1976,
  2093, 2217, 2349, 2489, 2637, 2794, 2960, 3136, 3322, 3520, 3729, 3951
};
size_t note_size = sizeof(notes)/sizeof(notes[0]);

class SingSong : public HalloweenBase {
private:
  static const int LOW_DURATION = 80;    // millisec
  static const int HOW_DURATION = 800;
  static const int MAX_NUM_NOTE = 105;

  int play_note[MAX_NUM_NOTE];
  int play_duration;
  int nnote;
  int cur_note;

public:
  SingSong() {;}
  virtual int8_t process(int8_t state);
  virtual int8_t updateTime(int8_t state, uint32_t msec);

  int play();
  void log_info();
};
///////////////

int8_t SingSong::process(int8_t state) {
  if (state == DETECT_NONE) {
    clean();
  } else {
    enabled = true;
    play_duration = random(LOW_DURATION, HOW_DURATION);     // in msec
    nnote = min(MAX_NUM_NOTE, PIR_DELAY_TIME / play_duration); // 8000 msec
    for (int i = 0; i < nnote; i++) {       // build play list
       play_note[i] = notes[random(1, note_size-1)];
    }
    cur_note = 0;
    tone(PIN_Buzz, play_note[cur_note++], play_duration);
    countdown = play_duration;
  }
  return state;
}
int8_t SingSong::updateTime(int8_t state, uint32_t msec) {
  if (enabled) {
    countdown -= msec;
    if ((countdown <= 0) && (cur_note < nnote)) {
      tone(PIN_Buzz, play_note[cur_note++], play_duration);
      countdown = play_duration;
    }
  }
  return state;
}
void SingSong::log_info() {
  Serial.print("play_duration=");
  Serial.print(play_duration);
  Serial.print(", nnote=");
  Serial.println(nnote);

  Serial1.print("play_duration=");
  Serial1.print(play_duration);
  Serial1.print(", nnote=");
  Serial1.println(nnote);
}
#endif
