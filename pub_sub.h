/*
 * pub_sub.h
 */
 #ifndef PUB_SUB_H
 #define PUB_SUB_H

 #include "constants.h"

extern bool update_state(int8_t newState);

class PubSub : public HalloweenBase {
public:
  PubSub(HalloweenBase* trigger, HalloweenBase** targets, int to_state);

  virtual int8_t process(int8_t state);
  virtual int8_t updateTime(int8_t state, uint32_t msec);
  virtual void clean();

private:
  HalloweenBase* pPub;
  HalloweenBase** pSub;
  int trigged_state;
};

////////
PubSub::PubSub(HalloweenBase* trigger, HalloweenBase** targets, int to_state)
  : pPub(trigger), pSub(targets) {
  trigger->enableit(true);
  trigged_state = to_state;
  for (HalloweenBase** pcmd = pSub; *pcmd; pcmd++) {
    (*pcmd)->clean();
  }
}
int8_t PubSub::process(int8_t state) {
  Serial.print("PubSub::process, state=");
  Serial.println(state);
  int ret_state = pPub->process(state);
  if (update_state(ret_state) && (trigged_state == ret_state)) {
    Serial.print("PubSub::process trigged state=");
    Serial.println(ret_state);
    for (HalloweenBase** pcmd = pSub; *pcmd; pcmd++) {
      ret_state = (*pcmd)->process(ret_state);
    }
  }
  return ret_state;
}
int8_t PubSub::updateTime(int8_t state, uint32_t msec) {
  int ret_state = pPub->updateTime(state, msec);
  if (update_state(ret_state)) {
    if (trigged_state == ret_state) {
      Serial.print("PubSub::updateTime state trigged new_state=");
      Serial.println(ret_state);
      for (HalloweenBase** pcmd = pSub; *pcmd; pcmd++) {
        ret_state = (*pcmd)->process(ret_state);
      } 
    } else if (ret_state == DETECT_NONE) {
      for (HalloweenBase** pcmd = pSub; *pcmd; pcmd++) {
        (*pcmd)->clean();
      }
      pPub->clean();
    }
  } else {
    for (HalloweenBase** pcmd = pSub; *pcmd; pcmd++) {
       ret_state = (*pcmd)->updateTime(ret_state, msec);
    }
  }
  return ret_state;
}
void PubSub::clean() {
  Serial.print("PubSub::clean()");
  pPub->clean();
  for (HalloweenBase** pcmd = pSub; *pcmd; pcmd++) {
    (*pcmd)->clean();
  }
  Serial.println(" done");
}
#endif
