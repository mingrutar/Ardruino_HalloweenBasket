/*
 * facial_lights.h
 *
 *
 */
#ifndef FACIAL_LIGHTS_H
#define FACIAL_LIGHTS_H

#include "neopixel_play.h"

static const int face_led_count = 10;
static Adafruit_NeoPixel face_leds = Adafruit_NeoPixel(face_led_count, PIN_NEO_Face, NEO_GRB + NEO_KHZ800);

class FacialLights : public NeoLEDPlay {
private:
  static const uint32_t DELAY_TIME = 1000;     // in msec
  static const uint32_t LONG_OFF = 60000;      // in msec,
  bool color_on;
  uint32_t on_time, off_time;

public:
  FacialLights();
  virtual int8_t process(int8_t state);
  virtual int8_t updateTime(int8_t state, uint32_t msec);
 };
///////////////
FacialLights::FacialLights() : NeoLEDPlay(face_leds) {
  Serial.println("FacialLights::FacialLights()");
}
int8_t FacialLights::process(int8_t state) {
  enabled = true;
  if (state != DETECT_NONE) {
    enabled = true;
    select_play(-1);                    // any rythm
    on_time = DELAY_TIME;
    off_time = (state == DETECT_HAND) ? DELAY_TIME : LONG_OFF;
    countdown = on_time;
    color_on = true;
  } else {
    clean();
  }
  return state;
}
int8_t FacialLights::updateTime(int8_t state, uint32_t msec) {
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
