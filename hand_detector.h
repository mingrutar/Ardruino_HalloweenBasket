/*
 * hand_detector.h
 * HC-SR505 mini PIR Motion Sensor
 * spec @ https://www.rapidonline.com/pdf/78-4110_v1.pdf
 */
 #ifndef HAND_DETECTOR_H
 #define HAND_DETECTOR_H
 #include "constants.h"

class HandDetector : public HalloweenBase {
private:
  bool bFound;
public:
  HandDetector();

  virtual int8_t process(int8_t state);
  virtual int8_t updateTime(int8_t state, uint32_t msec);
  virtual void clean();

private:
  bool search();
};
////////////
HandDetector::HandDetector() {
  pinMode(PIN_PIR, INPUT);
  digitalWrite(PIN_PIR, LOW);
  bFound = false;
}
bool HandDetector::search() {
  bool bRet = (digitalRead(PIN_PIR) == HIGH);
//  Serial.print("HandDetector::process: HIGH=");
//  Serial.print(bRet ? "true" : "false");
  if (bRet != bFound) {
    if (bRet) {
      Serial.print(", Somebody ");
      Serial1.print("=>INFO: Somebody ");
    } else {
      Serial.print(", Nobody ");
      Serial1.print("=>INFO: Nobody ");
    }
    Serial.println("is here");
    Serial1.println("is here");
    bFound = bRet;
    return true;
  } else {
    return false;
  }
}
void HandDetector::clean() {
  
}
int8_t HandDetector::process(int8_t state) {
  enabled = true;
  bool bchanged = search();
  if (bchanged) {
    if (bFound) {
      countdown = PIR_DELAY_TIME;
      return DETECT_HAND;
    } else {
      countdown = 0;
      return (state == DETECT_HAND) ? DETECT_BODY : state;
    }
  } else {
    return state;
  }
}
int8_t HandDetector::updateTime(int8_t state, uint32_t msec) {
  int ret_state = state;
  if (enabled) {
    if (state != DETECT_HAND) {
      bool bchanged = search();
      if (bchanged) {
//        Serial.print("HandDetector::updateTime: state=");
//        Serial.print(state);
//        Serial.print(", bchanged=");
//        Serial.println(bchanged ? "true" : "false");
        if (bFound) {
          countdown = PIR_DELAY_TIME;
          ret_state = DETECT_HAND;
        } else {
          countdown = 0;
          ret_state = (state == DETECT_HAND) ? DETECT_BODY : state;
        }
      }
    } else if (countdown > 0) {
      countdown -= msec;
      if (countdown <= 0) {
        countdown = 0;
        ret_state = (state == DETECT_HAND) ? DETECT_BODY : state;
      }
    }
  }
  return ret_state;
}
#endif
