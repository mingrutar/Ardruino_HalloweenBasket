/*
 *  rim_lights.h

*/
#ifndef RIM_LIGHTS_H
#define RIM_LIGHTS_H

#include "neopixel_play.h"

static const int rim_led_count = 40;
static Adafruit_NeoPixel rim_leds = Adafruit_NeoPixel(rim_led_count, PIN_NEO_RIM, NEO_GRB + NEO_KHZ800);
// Create instance of NeoPixel class
// Parameter 1 = number of pixels in leds
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)
class Rim_LED : public NeoLEDPlay {
private:
  static const int DELAY_TIME = 1000;     // in msec
  static const int LONG_OFF = 10000;      // in msec,

  static const int STATE1_PLAY = 3;       // theaterChase
  static const int STATE3_PLAY = -1;      // any,

  bool color_on;
  void set_play(int8_t state);

public:
  Rim_LED();
  virtual int8_t process(int8_t state);
  virtual int8_t updateTime(int8_t state, uint32_t msec);
};
//////////////
Rim_LED::Rim_LED() : NeoLEDPlay(rim_leds) {
  Serial.println("Rim_LED::Rim_LED()");
};

int8_t Rim_LED::process(int8_t state) {
  enabled = true;
  color_on = false;
  set_play(state);
  return state;
}

void Rim_LED::set_play(int8_t state) {
  countdown = DELAY_TIME;
  if (state == DETECT_NONE) {
    if (color_on) {
      clear_leds();
      countdown = LONG_OFF;
    } else {
      select_play(STATE1_PLAY);            // choose theaterChase
    }
    color_on = !color_on;
  } else {
    select_play(-1);                    // any rythm
  }
}
int8_t Rim_LED::updateTime(int8_t state, uint32_t msec) {
  if (enabled) {
    countdown -= msec;
    if (countdown <= 0) {
      set_play(state);
    }
  }
  return state;
}
#endif
