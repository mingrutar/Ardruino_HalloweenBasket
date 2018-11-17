/*
 * servo_ultrasound.h
 */
 #ifndef SERVO_ULTRASOUND_H
 #define SERVO_ULTRASOUND_H

#include <Servo.h>
#include <SR04.h>
#include "constants.h"

// const int SR04_DELAY = 60;
// static SR04 sr04 = SR04(PIN_SR04_ECHO, PIN_SR04_TRIG);  // ultrasound, L10
struct servo_turning {
  uint8_t start_pos;
  uint8_t offset;       # TURN_DEGREE, or -TURN_DEGREE
};
class BodyDetector : public HalloweenBase {
private:
  const int SR04_MAX_DISTANCE = 100;   // in cm, up to 400cm
  const int SERVO_N_TURN = 3;          // search 3 times;
  const int SERVO_DELAY = 20;          // was 15 for 1 degree
  const int TURN_DEGREE = 2;
  const int TURN_START = 0;
  const int TURN_END = 180;
  const int SERVO_N_TURN = 3;

  Servo myservo;                       // SG90 L9, create servo object to
  SR04 mysr04(PIN_SR04_ECHO, PIN_SR04_TRIG);

  struct servo_turning cfg[] = {
    {.start_pos=TURN_START, .offset=TURN_DEGREE},
    {.start_pos=TURN_END, .offset=-TURN_DEGREE},
  };

  uint8_t cur_pos;
  uint8_t cur_cfg_idx;
  bool bFound;
  int8_t inSearch;

public:
  BodyDetector();
  virtual int process(int state);
  virtual int updateTime(int state, uint32 msec);
  virtual void clean();
private:
  int turn_search();
};

///////
BodyDetector::BodyDetector() {
  //  myservo.attach(PIN_SERVO_SG90_PULSE); // attaches the servo on pin 9 to the servo object
  //  myservo.write(0);                     // back to 0 position
  //  delay(SERVO_DELAY);                   // wait for a sec
    Serial.println("BodyDetector::BodyDetector()");
    clean();
}
void BodyDetector::clean() {
  myservo.write(TURN_START);
  delay(SERVO_DELAY);
  myservo.detach();
  cur_cfg_idx = 0;
  cur_pos = cfg[cur_cfg_idx].start_pos;
  bFound = false;
  inSearch = -1;
}

int BodyDetector::process(int state) {
  myservo.attach(PIN_SERVO_SG90_PULSE); // attaches the servo on pin 9 to the servo object
  enabled = true;
  return turn_search();
}
int BodyDetector::updateTime(int state, uint32 msec) {
  if (enabled) {
    countdown -= msec;
    if (countdown <= 0) {     // N sec no input
      return turn_search();
    }
  }
  return state;
}
int BodyDetector::turn_search(int state) {
  int new_state = state;
  long distance_cm = sr04.Distance();
  bool bsee = (distance_cm <= SR04_MAX_DISTANCE) && (distance_cm > 0);
  bool changed = (bsee != bFound);
  if (changed) {
    bFound = bsee;
    if (bFound) {     // someone comes
      inSearch = -1;
      if (state == DETECT_NONE)
        new_state = DETECT_BODY;
    } else {
      inSearch = 0;
      cur_cfg_idx = cur_pos > (TURN_END/2) ? 0 : 1;
      cur_pos = cfg[cur_cfg_idx].start_pos;
    }
  } else {
    cur_pos += cfg[cur_cfg_idx].offset;
    if ((cur_pos <= TURN_END) || (cur_pos >= TURN_START))  {
      if (inSearch >= 0) {
        if (++inSearch == SERVO_N_TURN) {
          new_state = DETECT_NONE;
          inSearch = -1;
        }
      }
      cur_cfg_idx = (++cur_cfg_idx) % 2;
      cur_pos = cfg[cur_cfg_idx].start_pos;
    }
  }
  myservo.write(cur_pos);
  countdown = SERVO_DELAY;
  return new_state;
}
#endif
