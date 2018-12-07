#include <SoftwareSerial.h>

#include "constants.h"
//#include "BT_SR505.h"
#include "facial_lights.h"
#include "rim_lights.h"
#include "pub_sub.h"
#include "sing_song.h"
#include "hand_detector.h"
#include "stepper_motor.h"
#include "servo_ultrasound.h"

static const long BAUDRATE = 115200;    // 9600 19200 38400 57600, 115200
static const int DURATION = 20;         // msec

static long lmsec_val=0, msec_val;
static int8_t cur_state = DETECT_NONE;

int8_t cur_cmd_idx = -1; 

//
static const int NUM_DEVICE = 6;
static const int NUM_CMD = 9;
static const int STATE_1_CMD = 4;

static HalloweenBase* all_dev[NUM_DEVICE+1];
static HalloweenBase* pcommands[NUM_CMD+1];

void check_array(HalloweenBase** parray, int asize) {
  int jj = 0;
  for (HalloweenBase** p = parray; (*p); p++) {
    jj++;
  }
  if (jj != asize) {
    HalloweenBase* pi;
    for (int i = 0; i < asize; i++) {
      if (*(parray+i)== NULL) {
        Serial.print(" !!!!check_array: NULL item for i=");
        Serial.println(i);
      }
    }
  } else {
    Serial.print(" check_array: OK for array size=");
    Serial.println(jj);
  }
}
static void init_dev() {
  Serial.println("init_dev: all_dev...");
  int i = 0;
  all_dev[i++] = new BodyDetector();
  all_dev[i++] = new Rim_LED();       // 1 for rim led
  all_dev[i++] = new HandDetector();  // 2
  all_dev[i++] = new SingSong();      // 3
  all_dev[i++] = new FacialLights();  // 4 for face led
//  all_dev[i++] = new StepprMotor();   // 5
  all_dev[i++] = NULL;
    
  HalloweenBase* body_watcher_pub[] = {all_dev[0], NULL};
  HalloweenBase* body_watcher_sub[] = {all_dev[1], NULL};
  PubSub* body_watcher = new PubSub(body_watcher_pub, body_watcher_sub, DETECT_BODY);
  
  HalloweenBase* candy_watcher_pub[] = {all_dev[2], NULL};
  HalloweenBase* candy_watcher_sub1[] = {all_dev[3], all_dev[4], NULL};
  PubSub* candy_watcher1 = new PubSub(candy_watcher_pub, candy_watcher_sub1, DETECT_HAND);
  HalloweenBase* candy_watcher_sub2[] = {all_dev[3], NULL};
  PubSub* candy_watcher2 = new PubSub(candy_watcher_pub, candy_watcher_sub2, DETECT_HAND);
  
  HalloweenBase* all_pub[] = { all_dev[1], all_dev[0], NULL};
  HalloweenBase* all_sub[] = { candy_watcher1, NULL};
  PubSub* all_action = new PubSub(all_pub, all_sub, DETECT_BODY);
   
  i = 0;
  // state = 0
  pcommands[i++] = all_dev[0];             //0, servo/us
  pcommands[i++] = all_dev[1];             //1, rim_led
  pcommands[i++] = body_watcher;           //2, 
  pcommands[i++] = all_action;             //3
  // state = 1
  pcommands[i++] = all_dev[2];             //4, rip
  pcommands[i++] = all_dev[3];             //5, sing song 
  pcommands[i++] = all_dev[4];             //6, fled
  pcommands[i++] = candy_watcher1;         //7 with Face LED
  pcommands[i++] = candy_watcher2;         //8 without Face LED
  
  pcommands[i++] = NULL;
//  pcommands[i++] = all_dev[5];             //5, sepped motor

  check_array(all_dev, NUM_DEVICE);
  check_array(pcommands, NUM_CMD);
}
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  randomSeed(analogRead(0));
  Serial.println("start..");
  Serial.println("Connect to HC-05 from any other bluetooth device with 1234 as pairing key!.");
  Serial.print("Serial1 started at ");
  Serial.println(BAUDRATE);

  Serial1.begin(BAUDRATE);    // connected to BT
  Serial1.println("OK");
  msec_val = millis();
  lmsec_val = msec_val;

  init_dev();
  
  Serial.println(" ++ start clean..");
  int jj = 0;
  for (HalloweenBase** pcmd = all_dev; (*pcmd); pcmd++) {
    jj++;
//    Serial.println(" !!! pcmd(clean)=");
    (*pcmd)->clean();
  }
  Serial.print(" -- cleaned all_dev #=");
  Serial.print(jj);
  Serial.print(", cur_cmd_idx=");
  Serial.println(cur_cmd_idx); 
}

// change state. called by pub_sub module
bool update_state(int8_t newState) {
  bool ret = cur_state != newState;
  if (ret) {
    Serial.print("main::update_state: cur_state=");
    Serial.print(cur_state);
    Serial.print(", newState=");
    Serial.println(newState);
    cur_state = newState;
//    report_state('U');
  }
  return ret;
}
static int8_t new_state;
char data;
//
// report state via blue tooth 
// tt indicate the origin. L -> in loop()
//
void report_state(char tt) {
  Serial1.write(",$");
  Serial1.write(tt);
  Serial1.write(":(S)=");
  char tmp = cur_state + '0';
  Serial1.write(tmp);
  Serial1.write("(C)=");
  tmp = (cur_cmd_idx == -1) ? '-' : cur_cmd_idx + '0';
  Serial1.write(tmp);
  Serial1.write("\n");
}
void loop() {
  data = 0;
  if (Serial1.available()){
    data = Serial1.read();
  } else if (Serial.available()){ 
    data = Serial.read();
  }
  if (data != 0) {
    report_state('L');
    if (isPrintable(data)) {
      int cmd = data - '0';
      if ((cmd >= 0) && (cmd < NUM_CMD)) {
        if (cur_cmd_idx>= 0) {
          pcommands[cur_cmd_idx]->clean();
        }
        cur_state = cmd < STATE_1_CMD ? DETECT_NONE : DETECT_BODY;
        new_state = pcommands[cmd]->process(cur_state);
        cur_cmd_idx = cmd;
        update_state(new_state);
      } else {
        if (cur_cmd_idx != -1)
          pcommands[cur_cmd_idx]->clean();
        cur_cmd_idx = -1;
      }
    } else {
      Serial.print("WARN: ignore unknown input ");
      Serial.println(data, HEX);
    }
  }
  // update time
  msec_val = millis();
  if (((msec_val- lmsec_val) >= DURATION) && (cur_cmd_idx >=0))  {
    new_state = pcommands[cur_cmd_idx]->updateTime(cur_state, (msec_val- lmsec_val));
    update_state(new_state);
    lmsec_val = msec_val;
  } else {
    delay(DURATION);
  }
}
void HalloweenBase::clean() {
  countdown = 0; 
  enabled = false;
}

