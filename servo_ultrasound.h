/*
 * servo_ultrasound.h
 */
 #ifndef SERVO_ULTRASOUND_H
 #define SERVO_ULTRASOUND_H

#include <Servo.h>
#include <SR04.h>
#include "constants.h"

static const int TURN_DEGREE = 2;
static const int TURN_START = 10;
static const int TURN_END = 180;

static int cfg[][2] = {
  {TURN_START, TURN_DEGREE},
  {TURN_END, -TURN_DEGREE}
};
static SR04 mysr04 = SR04(PIN_SR04_ECHO, PIN_SR04_TRIG);
static Servo myservo;                       // SG90 L9, create servo object to

class BodyDetector : public HalloweenBase {
private:
  const int SR04_MAX_DISTANCE = 100;    // in cm, up to 400cm
  const int SERVO_N_TURN = 4;           // search 4 times;
  const int SERVO_DELAY = 30;           // was 15 for 1 degree
  const int LONG_SERVO_DELAY = 500;     // wait 0.5 sec

  int cur_pos;
  int cur_cfg_idx;
  bool bFound;
  int8_t inSearch;

  bool is_longwait;
  bool is_attached;
  
public:
  BodyDetector();
  virtual int8_t process(int8_t state);
  virtual int8_t updateTime(int8_t state, uint32_t msec);
  virtual void clean();
private:
  int8_t turn_search(int8_t state);
  int8_t test(int8_t state);                  // to learn the servo
};
///////
BodyDetector::BodyDetector() {
    myservo.attach(PIN_SERVO_SG90_PULSE); // attaches the servo on pin 9 to the servo object
    myservo.write(0);                   // back to 0 position
    delay(SERVO_DELAY);                   // wait for a sec
    is_attached = true;
    enabled = true;
    is_longwait = false;
}
void BodyDetector::clean() {
  Serial.print("BodyDetector::clean()");
  myservo.detach();
  is_attached = false;
  enabled = false;
  cur_cfg_idx = 0;
  cur_pos = cfg[cur_cfg_idx][0];
  bFound = false;
  inSearch = -1;
  Serial.println(" done");
}

int8_t BodyDetector::process(int8_t state) {
  enabled = true;
//  myservo.attach(PIN_SERVO_SG90_PULSE); // attaches the servo on pin 9 to the servo object
  
  inSearch = SERVO_N_TURN;
//  return test(state);     // TODO: remove
  return turn_search(state);
}
int8_t BodyDetector::updateTime(int8_t state, uint32_t msec) {
  if (enabled) {
    countdown -= msec;
    if (countdown <= 0) {     // N sec no input
//      return test(state);
      return turn_search(state);
    }
  }
  return state;
}
/*
void BodyDetector::test_0(int state) {
  Serial.println("BodyDetector::test_0(): test ");
  myservo.write(0);
  delay(LONG_SERVO_DELAY);
  Serial.println("BodyDetector::BodyDetector(): back to pos 0");
  
  for (int pos = TURN_START; pos < TURN_END; pos += TURN_DEGREE) {
    myservo.write(pos);                   // back to 0 position
    delay(SERVO_DELAY);                   // wait for a sec
  }
  Serial.print("BodyDetector::test(): turned ");
  Serial.println(TURN_END);
  myservo.detach();
  delay(SERVO_DELAY);                   // wait for a sec
  enabled = false;
  return state;
} */
int8_t BodyDetector::test(int8_t state) {
  cur_pos += cfg[cur_cfg_idx][1];           //cfg[cur_cfg_idx].offset;
  if ((cur_pos <= TURN_START) || (cur_pos >= TURN_END))  { //reach the end
    cur_cfg_idx = (++cur_cfg_idx) % 2;    // change direction
    cur_pos = cfg[cur_cfg_idx][0];
    if (--inSearch == 0) {
      enabled = false;
      Serial.print("BodyDetector::test detached, inSearch=");
      Serial.println(inSearch);
    }
    is_longwait = true;
  } else {
    is_longwait = false;
  }
  if (cur_pos % 10 == 0) {
    Serial.print("BodyDetector::test(): cur_pos=");
    Serial.println(cur_pos);
  }
  myservo.write(cur_pos);
  countdown = is_longwait ? LONG_SERVO_DELAY : SERVO_DELAY;
  if (is_longwait) {
    myservo.detach();
  }
  return state;
}
int8_t BodyDetector::turn_search(int8_t state) {
  int8_t new_state = state;
  long distance_cm = mysr04.Distance();
  bool bsee = (distance_cm <= SR04_MAX_DISTANCE) && (distance_cm > 0);
  bool changed = (bsee != bFound) && (distance_cm != 0);
  if (changed) {
    bFound = bsee;
    Serial.print(" BodyDetector::turn_search changed: bFound=");
    Serial.print(bFound);
    Serial.print(", distance_cm=");
    Serial.print(distance_cm);
    Serial.print(", cur_pos=");
    Serial.print(cur_pos);

    if (bFound) {     // someone comes
      inSearch = -1;
      if (state == DETECT_NONE) {   //ignore if someoneis here
        new_state = DETECT_BODY;
//        Serial.print(" BodyDetector::turn_search CHANGE: bFound=true distance_cm=");
//        Serial.println(distance_cm);
      }
    } else {
      inSearch = SERVO_N_TURN;
    }
    Serial.print(", state=");
    Serial.print(state);
    Serial.print(", new_state=");
    Serial.println(new_state);
  }   
  if (!bFound) {          // no body
    cur_pos += cfg[cur_cfg_idx][1];           //cfg[cur_cfg_idx].offset;
    if ((cur_pos <= TURN_START) || (cur_pos >= TURN_END))  { //reach the end
      cur_cfg_idx = (++cur_cfg_idx) % 2;    // change direction
      cur_pos = cfg[cur_cfg_idx][0];
      if (--inSearch == 0) {
        inSearch = -1;
        Serial.println(" At end, set state => DETECT_NONE");
        new_state = DETECT_NONE;
      }
      is_longwait = true;
      Serial.print(" BodyDetector::turn_search CHANGE: bFound=true inSearch=");
      Serial.println(inSearch);
    } else {
      is_longwait = false;
    }
    if (cur_pos % 10 == 0) {
      Serial.print("BodyDetector::turn_search(): cur_pos=");
      Serial.print(cur_pos);
      Serial.print(", bFound=");
      Serial.print(bFound ? "true" : "false");
      Serial.print(", distance_cm=");
      Serial.println(distance_cm);
    }
    if (!is_attached) {
//      Serial.println("+ BodyDetector::turn_search attach");
      myservo.attach(PIN_SERVO_SG90_PULSE); // attaches the servo
      is_attached = true;
    }
    myservo.write(cur_pos);
  } else {
    is_longwait = true;
  }
  countdown = is_longwait ? LONG_SERVO_DELAY : SERVO_DELAY;
  if (is_longwait && is_attached) {
//    Serial.println("- BodyDetector::turn_search detach");
    myservo.detach();
    is_attached = false;
  }
  return new_state;
}
#endif
