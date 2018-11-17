/*
 * facial_lights.h
 *
 *
 */
#ifndef FACIAL_LIGHTS_H
#define FACIAL_LIGHTS_H

#include "neopixel_play.h"

const uint16 LED_COUNT 16;
static Adafruit_NeoPixel face_leds = Adafruit_NeoPixel(LED_COUNT, PIN_NEO_Face, NEO_GRB + NEO_KHZ800);

class FacialLights : public NeoLEDPlay {
private:
  const uint32 DELAY_TIME = 1000;     // in msec
  const uint32 LONG_OFF = 60000;      // in msec,
  bool color_on;
  uint32 on_time, off_time;

public:
  FacialLights() : NeoLEDPlay(face_leds) {;};
  virtual int process(int state);
  virtual int updateTime(int state, uint32 msec);
 };
///////////////
int FacialLights::process(int state) {
  enabled = true;
  if (state != DETECT_NONE) {
    enabled = true;
    select_play(-1);                    // any rythm
    on_time = DELAY_TIME;
    off_time = (state == DETECT_HAND) ? DELAY_TIME : LONG_OFF
    countdown = on_time;
    color_on = true;
  } else {
    clean();
  }
  return state;
}
void FacialLights::updateTime(int state, uint32 msec) {
  if (enabled) {
    countdown -= msec;
    if (countdown <= 0) {
      if (color_on) {
        clear_leds();
        countdown = off_time;
      }
      else {
        select_play(-1);
        countdown = on_time;
      }
      color_on = !color_on;
    }
  }
  return state;
}
#endif
