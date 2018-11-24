/**
 * BT_SR505.h
 * write and read from blue tooth SR505
 */
#ifndef BT_SR505_H
#define BT_SR505_H
#include <SoftwareSerial.h>

class TestBlueTooth : public HalloweenBase {
public:
  const uint32_t DELAY_TIME = 3000;    // 3 sec

private:
  SoftwareSerial& BTSerial;
  char _data;

public:
  TestBlueTooth(SoftwareSerial& btdevice);
  virtual int8_t process(int8_t state);
  virtual int8_t updateTime(int8_t state, uint32_t msec);
};
//////////////
TestBlueTooth::TestBlueTooth(SoftwareSerial& btdevice)
  : BTSerial(btdevice) {
  HalloweenBase::clean();
  enabled = false;
}

int8_t TestBlueTooth::process(int8_t state) {
  enabled = true;
  Serial.print("TestBlueTooth::process: enabled=");
  Serial.println(enabled);
  BTSerial.write("=>INFO: state is ");
  BTSerial.write(state);
  countdown = DELAY_TIME;
  return state;
}
int8_t TestBlueTooth::updateTime(int8_t state, uint32_t msec) {
  if (enabled) {
    Serial.print("TestBlueTooth::updateTime: enabled=");
    Serial.println(enabled);
    countdown -= msec;
    if (countdown <= 0) {   // N sec no input
      Serial.println("TestBlueTooth::updateTime: done test");
      BTSerial.write(" =>ECHO: Done test");
      HalloweenBase::clean();
    } else if (BTSerial.available()){
      _data = BTSerial.read();
      if (isPrintable(_data)) {
        Serial.print("receive data=");
        Serial.println(_data);
        BTSerial.write(" =>ECHO: got data=");
        BTSerial.write(_data);
      } else {
        Serial.println("WARN: receive unprintable");
        BTSerial.write(" =>ECHO WARN: unprintable=");
        BTSerial.write(_data);
      }
      countdown = DELAY_TIME;
    }
  }
  return state;
}
#endif
