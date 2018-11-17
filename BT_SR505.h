/**
 * BT_SR505.h
 * write and read from blue tooth SR505
 */
 #ifndef BT_SR505_H
 #define BT_SR505_H

class TestBlueTooth : public HalloweenBase{
public:
  const uin DELAY_TIME = 3000;    // 3 sec

private:
  SoftwareSerial& _BTSerial;
  char _data;

public:
  TestBlueTooth(SoftwareSerial& btdevice);
  virtual int process(int state);
  virtual int updateTime(int state, uint32_t msec);
};
//////////////
TestBlueTooth::TestBlueTooth(SoftwareSerial& btdevice)
  : BTSerial(btdevice) {
  HalloweenBase::clean();
}
int BT_output::process(int state) {
  enabled = true;
  BTSerial.write("=>INFO: state is ");
  BTSerial.write(state);
  countdown = DELAY_TIME;
  return state;
}
int BT_output::updateTime(int state, uint32_t msec) {
  if (enabled) {
    countdown -= msec;
    if (countdown <= 0) {   // N sec no input
      Serial.println("Done test");
      BTSerial.write(" =>ECHO: Done test");
      HalloweenBase::clean();
    } else if (BTSerial.available())){
      data = BTSerial.read();
      if (isPrintable(data)) {
        Serial.print("receive data=");
        Serial.println(data);
        BTSerial.write(" =>ECHO: got data=");
        BTSerial.write(data);
      } else {
        Serial.println("WARN: receive unprintable");
        BTSerial.write(" =>ECHO WARN: unprintable=");
        BTSerial.write(data);
      }
      countdown = DELAY_TIME;
    }
  }
  return state;
}
#endif
