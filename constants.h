/**
  constants.h
*/
#ifndef CONSTANTS_H
#define CONSTANTS_H
#include <SoftwareSerial.h>

const int PIN_SERVO_SG90_PULSE = 39;   // was 9
const int PIN_Buzz = 9;
const int PIN_BlueTooth_TX = 10;      // HC-05 Rx
const int PIN_BlueTooth_RX = 11;      // HC-05 Tx

const int PIN_L293D_Motor_1 = 4;      // L293DPin 1
const int PIN_L293D_Motor_2 = 5;      // L293DPin 6
const int PIN_L293D_Motor_3 = 6;      // L293DPin 8  2nd at other side
const int PIN_L293D_Motor_4 = 7;      // L293DPin 14

const int PIN_NEO_RIM = 50;
const int PIN_NEO_Face = 51;          // was 8
const int PIN_PIR = 52;               // was 6, HC-SR505


const int PIN_SR04_ECHO = 12;         // ultrasound sensor SR04, L10
const int PIN_SR04_TRIG = 8;

const int PIN_LED_1 = 23;
const int PIN_LED_2 = 26;

// state
const int8_t DETECT_NONE = 0;
const int8_t DETECT_BODY = 1;
const int8_t DETECT_HAND = 2;

// shared time delay
const uint32_t PIR_DELAY_TIME = 8000;     // in msec, 8 sec fr RIP state change

extern SoftwareSerial BTSerial;

// HalloweenBase isthe  base class for Halloween project

class HalloweenBase {
public:
  HalloweenBase() : enabled(false) {;};

  virtual int8_t process(int8_t state) = 0;
  virtual int8_t updateTime(int8_t state, uint32_t msec) = 0;
  virtual void clean();

  inline void enableit(bool val) {enabled = val;}
  
protected:
  // virtual void at_start() {
  // TODO: add action
  // }
  bool enabled;
  int32_t countdown;
};
#endif
