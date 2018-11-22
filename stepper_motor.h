/*
 * stepper_motor.h
 * 
 * https://mertarduinotutorial.blogspot.com/2016/12/arduino-tutorial-06-stepper-motor.html
 * https://www.instructables.com/id/Arduino-How-to-Control-a-Stepper-Motor-With-L293D-/(schema)
 * wiring: L293D with Stepper motor (SM), pin are 0-base
 * 0=>+5, 1=>pin8; 2=>SM-orange (A); 3&4 => GND; 5 =>SM-blue;6=>pin9;7=>5V
 * 8=>+5, 9=>pin10; 10=> SM-yellow; 11&12 => GND; 13 =>SM-pink;14=>pin11;15=>5V
 */
#ifndef STEPPER_MOTOR_H
#define STEPPER_MOTOR_H

#include "constants.h"

class StepprMotor : public HalloweenBase {
private:
  static const uint8_t NUM_PHASES = 8;
  static const uint8_t NUM_PHASE_VALS = 4;
  int phase_values[NUM_PHASES][NUM_PHASE_VALS] = {
    {LOW, LOW, LOW, HIGH},
    {LOW, LOW, HIGH, HIGH},
    {LOW, LOW, HIGH, LOW},
    {LOW, HIGH, HIGH, LOW},
    {LOW, HIGH, LOW, LOW},
    {HIGH, HIGH, LOW, LOW},
    {HIGH, LOW, LOW, LOW},
    {HIGH, LOW, LOW, HIGH} };
  static const int SlowDelayTime = 20;     //msec
  static const int FastDelayTime = 10;     //msec

  uint8_t cur_phase;

public:
  StepprMotor();
  virtual int8_t process(int8_t state);
  virtual int8_t updateTime(int8_t state, uint32_t msec);

private:
  void set_phase();
};
//////////////
StepprMotor::StepprMotor() {
  pinMode(PIN_L293D_Motor_1, OUTPUT);
  pinMode(PIN_L293D_Motor_2, OUTPUT);
  pinMode(PIN_L293D_Motor_3, OUTPUT);
  pinMode(PIN_L293D_Motor_4, OUTPUT);
  cur_phase = 0;
}
int8_t StepprMotor::process(int8_t state) {
  enabled = true;
  set_phase();
  cur_phase = cur_phase % NUM_PHASES;
  countdown = SlowDelayTime;
  return state;
}
int8_t StepprMotor::updateTime(int8_t state, uint32_t msec) {
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
  int* pval = phase_values[cur_phase];
  digitalWrite(PIN_L293D_Motor_1, *pval);
  digitalWrite(PIN_L293D_Motor_2, *(pval+1));
  digitalWrite(PIN_L293D_Motor_3, *(pval+2));
  digitalWrite(PIN_L293D_Motor_4, *(pval+3));
  cur_phase = (++cur_phase) % NUM_PHASES;
}
#endif
