/*
 * neopixel_play.h

   WS2812 RGB LED Test  ws2812-rgb-test.ino
   Uses Adafruit NeoPixel Library
   Functions from Adafruit NeopPixel Strandtest Examples
   DroneBot Workshop 2018, https://dronebotworkshop.com
 */
 #ifndef NEOPIXEL_PLAY_H
 #define NEOPIXEL_PLAY_H

#include <Adafruit_NeoPixel.h>
#include "constants.h"

class NeoLEDPlay : public HalloweenBase {
private:
  Adafruit_NeoPixel& neoleds;

protected:
  static const uint8_t INTERVAL = 30;    // delay for pixels take effect
  static const byte MAXLIGHT = 255;

public:
  NeoLEDPlay(Adafruit_NeoPixel& neo);

  virtual void clean();

  void setMyColor(int cval, uint8_t wait=INTERVAL);
  uint32_t getMyColor(byte intensity=(MAXLIGHT/2));

  void rainbowCycle(uint8_t wait=INTERVAL);
  void colorWipe(uint32_t c, uint8_t wait=INTERVAL);
  void rainbow(uint8_t wait=INTERVAL);
  void theaterChase(uint32_t c, uint8_t wait=INTERVAL);
  void theaterChaseRainbow(uint8_t wait=INTERVAL);

protected:
  uint32_t NeoLEDPlay::check_delay_time(uint32_t delay_time);
  void select_play(int8_t sel=-1, uint32_t wait=INTERVAL);
  void select_play(int8_t sel=-1, uint32_t clr, uint32_t wait=INTERVAL);
  uint32_t wheel(byte WheelPos);

};
////////////////////////
// Function to set all LEDs off
NeoLEDPlay::NeoLEDPlay(Adafruit_NeoPixel& neo)
: neopixels(neo) {
  neopixels.begin();
  clean();
}
void NeoLEDPlay::clean() { // Cycle through all LEDs
  HalloweenBase::clean();
  clear_leds();
}
uint32_t NeoLEDPlay::check_delay_time(uint32_t delay_time) {
  uint32 min_delay = INTERVAL * neoleds.numPixels();
  return (delay <= min_delay) ? min_delay : delay_time;
}
void NeoLEDPlay::clear_leds() { // Cycle through all LEDs
  for (int i = 0; i < neoleds.numPixels(); i++) { // Set color to zero which is off
    neoleds.setPixelColor(i, neoleds.Color(0, 0, 0));
  }
  neoleds.show();
  delay(INTERVAL);
}
void NeoLEDPlay::select_play(int8_t sel, uint32_t wait=INTERVAL) {
  byte clrByPos = random(0, MAXLIGHT);
  select_play(wheel(clrByPos), sel, wait);
}
void NeoLEDPlay::select_play(int8_t sel, uint32_t clr, uint32_t wait=INTERVAL) {
  if sel < 0 {
    sel = random(0, 5);
  }
  switch (sel) {
    case 0:
      theaterChaseRainbow(wait);
      break;
    case 1:
      rainbowCycle(wait);
      break;
    case 2:
      rainbow(wait);
      break;
    case 3:
      theaterChase(clr, wait);
      break;
    case 4:
      colorWipe(clr, wait);
      break;
    default:
      clear_leds();
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t NeoLEDPlay::wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return neoleds.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return neoleds.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return neoleds.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}
///////// the following from onlne neopixel.one
// Fill the dots one after the other with a color
void NeoLEDPlay::colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i = 0; i < neoleds.numPixels(); i++) {
    neoleds.setPixelColor(i, c);
    neoleds.show();
    delay(wait);
  }
}
void NeoLEDPlay::rainbow(uint8_t wait) {
  uint16_t i, j;
  for(j = 0; j < 256; j++) {
    for(i = 0; i < neoleds.numPixels(); i++) {
      neoleds.setPixelColor(i, wheel((i+j) & 255));
    }
    neoleds.show();
    delay(wait);
  }
}
// Slightly different, this makes the rainbow equally distributed throughout
void NeoLEDPlay::rainbowCycle(uint8_t wait) {
  uint16_t i, j;
  for(j = 0; j < 256 * 5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< neoleds.numPixels(); i++) {
      neoleds.setPixelColor(i, wheel(((i * 256 / neoleds.numPixels()) + j) & 255));
    }
    neoleds.show();
    delay(wait);
  }
}
//Theatre-style crawling lights.
void NeoLEDPlay::theaterChase(uint32_t c, uint8_t wait) {
  for (int j = 0; j < 10; j++) {  //do 10 cycles of chasing
    for (int q = 0; q < 3; q++) {
      for (uint16_t i = 0; i < neoleds.numPixels(); i = i + 3) {
        neoleds.setPixelColor(i + q, c);    //turn every third pixel on
      }
      neoleds.show();
      delay(wait);

      for (uint16_t i = 0; i < neoleds.numPixels(); i = i +3) {
        neoleds.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}
//Theatre-style crawling lights with rainbow effect
void NeoLEDPlay::theaterChaseRainbow(uint8_t wait) {
  for (int j = 0; j < 256; j++) {     // cycle all 256 colors in the wheel
    for (int q = 0; q < 3; q++) {
      for (uint16_t i=0; i < neoleds.numPixels(); i = i + 3) {
        neoleds.setPixelColor(i + q, this.wheel((i+j) % 255)); //turn every third pixel on
      }
      neoleds.show();
      delay(wait);

      for (uint16_t i = 0; i < neoleds.numPixels(); i = i + 3) {
        neoleds.setPixelColor(i + q, 0);        //turn every third pixel off
      }
    }
  }
}

///// TODO:not in use  uint32_t cval = getMyColor(color_val)
// void NeoLEDPlay::setMyColor(uint16_t pos, uint32_t cval) {
//   Serial.print("color val=");
//   Serial.println(cval, HEX);
//   neoleds.setPixelColor(pos, cval);
//   neoleds.show();
//   isactive = true;
// }
// ///// TODO:not in use
// uint32_t NeoLEDPlay::getMyColor(byte intensity) {
//   int val = random(0, 7);         // ramdonly in 7 colors
//   if (val == 0) {
//     return neoleds.Color(intensity, 0, 0);
//   } else if (val == 1) {
//     return neoleds.Color(0, intensity, 0);
//   } else if (val == 2) {
//     return neoleds.Color(0, 0, intensity);
//   } else if (val == 3) {
//     return neoleds.Color(intensity, intensity, 0);
//   } else if (val == 4) {
//     return neoleds.Color(0, intensity, intensity);
//   } else if (val == 5) {
//     return neoleds.Color(intensity, 0, intensity);
//   } else  {
//     return neoleds.Color(intensity, intensity, 0);
//   }
// }
//
#endif
