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
static const int DURATION = 10;         // msec

static long lmsec_val=0, msec_val;
static int8_t cur_state = DETECT_NONE;

int8_t cur_cmd_idx = -1; 

SoftwareSerial BTSerial(PIN_BlueTooth_RX, PIN_BlueTooth_TX); // RX | TX
static const int NUM_DEVICE = 6;
static const int NUM_CMD = 10;

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
  all_dev[i++] = new StepprMotor();   // 1
  all_dev[i++] = new Rim_LED();                // 2 for rim led
  all_dev[i++] = new HandDetector();  // 3
  all_dev[i++] = new SingSong();      // 4
  all_dev[i++] = new FacialLights();  // 5 for face led
  all_dev[i++] = NULL;
    
  HalloweenBase* pdetector1[] = {all_dev[1], all_dev[2], NULL};
  PubSub* body_watcher = new PubSub(all_dev[0], pdetector1);
  HalloweenBase* pdetector2[] = {all_dev[4], all_dev[5], NULL};
  PubSub* candy_watcher = new PubSub(all_dev[3], pdetector2);
  HalloweenBase* pall[] = {all_dev[1], all_dev[2], candy_watcher, NULL};
  i = 0;
//  pcommands[i++] = new TestBlueTooth(BTSerial);  //0
  pcommands[i++] = all_dev[0];             //0, servo/us
  pcommands[i++] = all_dev[1];             //1, sm
  pcommands[i++] = all_dev[2];             //2, rim_led
  pcommands[i++] = all_dev[3],             //3, rip
  pcommands[i++] = all_dev[4],             //4, sing song 
  pcommands[i++] = all_dev[5],             //5, fled
  pcommands[i++] = body_watcher,           //6, 
  pcommands[i++] = candy_watcher,          //7
  pcommands[i++] = new PubSub(all_dev[0], pall);    //8
  pcommands[i++] = NULL;

  check_array(all_dev, NUM_DEVICE);
  check_array(pcommands, NUM_CMD);
}
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  randomSeed(analogRead(0));
  Serial.println("start..");
//  Serial.println("Connect to HC-05 from any other bluetooth device with 1234 as pairing key!.");
  Serial.print("BTserial started at ");
  Serial.println(BAUDRATE);

  BTSerial.begin(BAUDRATE);
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

// called by a HalloweenBase module TODO: => event?
bool update_state(int8_t newState) {
  bool ret = cur_state != newState;
  if (ret) {
    Serial.print("changing state from");
    Serial.print(cur_state);
    Serial.print(" to ");
    Serial.println(newState);
//    BTSerial.print("=>INFO: changing state from");
//    BTSerial.print(cur_state);
//    BTSerial.print(" to ");
//    BTSerial.println(newState);
    cur_state = newState;
  }
  return ret;
}
static int8_t new_state;
static bool bvalid;
char data;
void loop() {
  if (BTSerial.available()){
    bvalid = false;
    data = BTSerial.read();
    BTSerial.write(",R=");
    BTSerial.write(data);
//    Serial.print("r ");
//    Serial.println(data, HEX);
    if (isPrintable(data)) {
      BTSerial.write("P;");
      BTSerial.write("\n");
//      Serial.print("y");
      int cmd = data - '0';
      if ((cmd >= 0) && (cmd < NUM_CMD)) {
        bvalid = true;
        if (cur_cmd_idx>= 0) {
          pcommands[cur_cmd_idx]->clean();
        }
        new_state = pcommands[cmd]->process(cur_state);
        cur_cmd_idx = cmd;
        update_state(new_state);
      }
    }
    if (!bvalid) {
//      Serial.print("WARN: unknown input");
//      Serial.println(data);
      BTSerial.write("=>WARN: unknown input");
      BTSerial.write(data);
      BTSerial.write("\n");
    }
  }
  // update time
  msec_val = millis();
  if (((msec_val- lmsec_val) >= DURATION) && (cur_cmd_idx >=0))  {
    new_state = pcommands[cur_cmd_idx]->updateTime(cur_state, (msec_val- lmsec_val));
    update_state(new_state);
    lmsec_val = msec_val;
//    Serial.println(" A loop ...");
  } else {
    delay(DURATION);
  }
}
void HalloweenBase::clean() {
  countdown = 0; 
  enabled = false;
}

