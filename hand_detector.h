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

  virtual int process(int state) = 0;
  virtual int updateTime(int state, uint32 msec) = 0;
  virtual void clean() {countdown = 0; enabled = false};

  int process(int state);
  void updateTime(int state, int msec);
  void clean() {;};

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
  bool bRet = (digitalRead(PIN_PIR) == HIGH));
  bool state_change = (bRet != bFound);
  if (state_change) {
    if (bRet) {
      Serial.print("Somebody ");
      pOutput->print("=>INFO: Somebody ");
    } else {
      Serial.print("Nobody");
      pOutput->print("=>INFO: Nobody ");
    }
    Serial.println("is here");
    pOutput->println("is here");
    bFound = bRet;
  }
  return state_change;
}
int HandDetector::process(int state) {
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
void HandDetector::updateTime(int state, int msec) {
  int ret_state = state;
  if (enabled) {
    bool bchanged = search();
    if (bchanged) {
      if (bFound) {
        countdown = PIR_DELAY_TIME;
        ret_state = DETECT_HAND;
      } else {
        countdown = 0;
        ret_state = (state == DETECT_HAND) ? DETECT_BODY : state;
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
