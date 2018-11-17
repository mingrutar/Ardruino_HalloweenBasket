/*
 * stepper_motor.h https://mertarduinotutorial.blogspot.com/2016/12/arduino-tutorial-06-stepper-motor.html
 * https://www.instructables.com/id/Arduino-How-to-Control-a-Stepper-Motor-With-L293D-/(schema)
 * wiring: L293D with Stepper motor (SM), pin are 0-base
 * 0=>+5, 1=>pin8; 2=>SM-orange (A); 3&4 => GND; 5 =>SM-blue;6=>pin9;7=>5V
 * 8=>+5, 9=>pin10; 10=> SM-yellow; 11&12 => GND; 13 =>SM-pink;14=>pin11;15=>5V
 */
 #ifndef STEPPER_MOTOR_H
 #define STEPPER_MOTOR_H

#include "constant.h"

class StepprMotor : public HalloweenBase {
private:
  const uint8_t NUM_PHASES = 8;
  const uint8_t NUM_PHASE_VALS = 4;
  const uint8_t phase_values[NUM_PHASE_VALS][NUM_PHASES] = {
    {LOW, LOW, LOW, HIGH},
    {LOW, LOW, HIGH, HIGH},
    {LOW, LOW, HIGH, LOW},
    {LOW, HIGH, HIGH, LOW},
    {LOW, HIGH, LOW, LOW},
    {HIGH, HIGH, LOW, LOW},
    {HIGH, LOW, LOW, LOW},
    {HIGH, LOW, LOW, HIGH} };
  const int SlowDelayTime = 20;     //msec
  const int FastDelayTime = 10;     //msec

  uint8_t cur_phase;

public:
  StepprMotor();
  virtual int process(int state);
  virtual int updateTime(int state, uint32 msec);

private:
  void set_phase();
};
//////////////
StepprMotor::StepprMotor{
  pinMode(PIN_L293D_Motor_1, OUTPUT);
  pinMode(PIN_L293D_Motor_2, OUTPUT);
  pinMode(PIN_L293D_Motor_3, OUTPUT);
  pinMode(PIN_L293D_Motor_4, OUTPUT);
  cur_phase = 0;

  lmsec_val = millis();
  delayTime = SlowDelayTime;
  Serial.print("delayTime=");
  Serial.println(delayTime);
}
int StepprMotor::process(int state) {
  enabled = true;
  set_phase();
  cur_phase = cur_phase % NUM_PHASES;
  countdown = SlowDelayTime;
  return state;
}
int StepprMotor::updateTime(int state, uint32 msec) {
  if (enabled) {
    countdown -= msec;
    if (countdown <= 0) {
      set_phase();
      countdown = SlowDelayTime;
    }
  }
  return state;
}
void StepprMotor::set_phase() {
  uint8_t* pval = phase_values[cur_phase];
  digitalWrite(PIN_L293D_Motor_1, *pval);
  digitalWrite(PIN_L293D_Motor_2, *(pval+1));
  digitalWrite(PIN_L293D_Motor_3, *(pval+2));
  digitalWrite(PIN_L293D_Motor_4, *(pval+3));
  cur_phase = (++cur_phase) % NUM_PHASES;
}
#endif
