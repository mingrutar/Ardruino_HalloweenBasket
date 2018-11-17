/*
 * pub_sub.h
 */
 #ifndef PUB_SUB_H
 #define PUB_SUB_H

 #include "constants.h"

class PubSub : public HalloweenBase {
public:
  PubSub(HalloweenBase* trigger, HalloweenBase** targets);

  virtual int process(int state);
  virtual int updateTime(int state, uint32 msec);
  virtual void clean();

private:
  HalloweenBase* pPub;
  HalloweenBase** pSub;
};

////////
PubSub::PubSub(HalloweenBase* trigger, HalloweenBase** targets)
  : pPub(trigger), pSub(targets)) {
  trigger.enabled = true;
  for (HalloweenBase** pcmd = pSub; *pcmd; pcmd++) {
    (*pcmd)->clean();
  }
}
int PubSub::process(int state) {
  int ret_state = pPub->process(state);
  if (update_state(ret_state)) {
    for (HalloweenBase** pcmd = pSub; *pcmd; pcmd++) {
      ret_state = (*pcmd)->process(ret_state);
    }
  }
  return ret_state;
}
int PubSub::updateTime(int state, uint32 msec) {
  int ret_state = pPub->updateTime(state, msec);
  if (update_state(ret_state)) {
    for (HalloweenBase** pcmd = pSub; *pcmd; pcmd++) {
      ret_state = (*pcmd)->updateTime(ret_state, msec);
    }
  }
  return ret_state;
}
void PubSub::clean() {
  pPub->clean();
  HalloweenBase** pcmd = targets;
  for (HalloweenBase** pcmd = pSub; *pcmd; pcmd++) {
    (*pcmd)->clean();
  }
}
#endif
