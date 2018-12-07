/*
 * pub_sub.h
 */
#ifndef PUB_SUB_H
#define PUB_SUB_H

#include "constants.h"

extern bool update_state(int8_t newState);

class PubSub : public HalloweenBase {
public:
  PubSub(HalloweenBase** trigger, HalloweenBase** targets, int to_state);

  virtual int8_t process(int8_t state);
  virtual int8_t updateTime(int8_t state, uint32_t msec);
  virtual void clean();

private:
  HalloweenBase** pPub;
  HalloweenBase** pSub;
  int trigged_state;
};

////////
PubSub::PubSub(HalloweenBase** trigger, HalloweenBase** targets, int to_state)
  : pPub(trigger), pSub(targets) {
  for (HalloweenBase** ptgr = pPub; *ptgr; ptgr++) {
    (*ptgr)->enableit(true);
  }
  trigged_state = to_state;
  for (HalloweenBase** pcmd = pSub; *pcmd; pcmd++) {
    (*pcmd)->clean();
  }
}
int8_t PubSub::process(int8_t state) {
  Serial.print("PubSub::process, state=");
  Serial.print(state);
  
  int ret_state = -1;
  for (HalloweenBase** ptgr = pPub; *ptgr; ptgr++) {
    ret_state = (*ptgr)->process(state);
  }
  Serial.print(", trigged_state=");
  Serial.print(trigged_state);
  Serial.print(", ret_state=");
  Serial.println(ret_state);
  if (update_state(ret_state) && (trigged_state == ret_state)) {
    for (HalloweenBase** pcmd = pSub; *pcmd; pcmd++) {
      ret_state = (*pcmd)->process(ret_state);
    }
  }
  return ret_state;
}
int8_t PubSub::updateTime(int8_t state, uint32_t msec) {
  int8_t ret_state = -1;
  for (HalloweenBase** ptgr = pPub; *ptgr; ptgr++) {
    ret_state = (*ptgr)->updateTime(state, msec);
  }
  if (update_state(ret_state)) {
    Serial.print("PubSub::updateTime state=");
    Serial.print(state);
    Serial.print(", trigged_state=");
    Serial.print(trigged_state);
    Serial.print(", ret_state=");
    Serial.println(ret_state);
    if (trigged_state == ret_state) {
      int8_t sub_ret_state;
      for (HalloweenBase** pcmd = pSub; *pcmd; pcmd++) {
        sub_ret_state = (*pcmd)->process(ret_state);
      } 
//    } else if (ret_state == DETECT_NONE) {
//      for (HalloweenBase** pcmd = pSub; *pcmd; pcmd++) {
//        (*pcmd)->clean();
//      }
//      pPub->clean();
    }
  } else {
//    Serial.print("PubSub::updateTime, no change  ret_state=");
//    Serial.println(ret_state);
    for (HalloweenBase** pcmd = pSub; *pcmd; pcmd++) {
       ret_state = (*pcmd)->updateTime(ret_state, msec);
    }
  }
  return ret_state;
}
void PubSub::clean() {
  Serial.print("PubSub::clean()");
  for (HalloweenBase** ptgr = pPub; *ptgr; ptgr++) {
    (*ptgr)->clean();
  }
  for (HalloweenBase** pcmd = pSub; *pcmd; pcmd++) {
    (*pcmd)->clean();
  }
  Serial.println(" done");
}
#endif
