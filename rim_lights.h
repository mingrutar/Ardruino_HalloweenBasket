/*
 *  rim_lights.h

*/
#ifndef RIM_LIGHTS_H
#define RIM_LIGHTS_H

#include "neopixel_play.h"

const uint16 LED_COUNT 40;
static Adafruit_NeoPixel rim_leds = Adafruit_NeoPixel(LED_COUNT, PIN_NEO_RIM, NEO_GRB + NEO_KHZ800);
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
  const uint32 DELAY_TIME = 2000;     // in msec
  const uint32 SHORT_ON = 500;        // in msec
  const uint32 LONG_OFF = 30000;      // in msec,

  bool color_on;
  uint32 on_time, off_time;

public:
  Rim_LED() : NeoLEDPlay(rim_leds) {;};
  virtual int process(int state);
  virtual int updateTime(int state, uint32 msec);
};
//////////////
int Rim_LED::process(int state) {
  enabled = true;
  color_on = true;
  if (state == DETECT_NONE) {
    on_time = SHORT_ON;
    off_time = LONG_OFF;
  } else {
    on_time = DELAY_TIME;
    off_time = 0;
  }
  select_play(-1);                    // any rythm
}
int Rim_LED::updateTime(int state, uint32 msec) {
  if (enabled) {
    countdown -= msec;
    if (countdown <= 0) {
      if ((off_time > 0) && color_on) {  // turn off the leds
        clear_leds();
        countdown = off_time;
      } else {
        select_play(-1);
        countdown = on_time;
      }
      color_on = !color_on;
    }
  }
}
#endif