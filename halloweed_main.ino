
#include <SoftwareSerial.h>
#include "constants.h"
#include "BT_SR505.h"
#include "servo_ultrasound.h"
#include "facial_lights.h"
#include "rim_lights.h"
#include "pub_sub.h"
#include "sing_song.h"
#include "hand_detector.h"

// Commands
// const int CMD_Detect_Sing = 0;   // detect and sing
// const int CMD_Stepper = 1;       // stepper motor
// const int CMD_RIM_LED = 2;       // rim neopixel LED
// const int CMD_Bluetooth = 3;     // bluetooth
// const int CMD_Face_LED = 4;      // face neopixel LED
// const int CMD_Eye_LED = 5;       // eye LED

// const int CMD_S1 = 5;      // servo-ultrasound, s1
// const int CMD_S2 = 6;      // state 2, stepper motor + rim neopixel LED
// const int CMD_S3 = 7;      // state 3, Detect_Sing+Face_LED+Eye_LED
// const int CMD_13 = 8;      // all

static const long BAUDRATE = 115200;    // 9600 19200 38400 57600, 115200
static const int DURATION = 10;         // msec

static long lmsec_val=0, msec_val;
static int cur_state = DETECT_NONE;

static SoftwareSerial BTSerial(PIN_BlueTooth_RX, PIN_BlueTooth_TX); // RX | TX
static const int NUM_DEVICE = 6;
static HalloweenBase* all_dev[] = { new Watcher(), new StepprMotor(),
  new Rim_LED(), new FacialLights(), new SingSong(), new hand_detector(), NULL};

PubSub* body_watcher = new PubSub(all_dev[0], {all_dev[1], all_dev[2], NULL});
PubSub* candy_watcher = new PubSub(all_dev[4], {all_dev[3], NULL});

static HalloweenBase* pcommands[] = {
  new TestBlueTooth(BTSerial),        // cmd 0
  all_dev[0],             //1
  all_dev[1],             //2
  all_dev[2],             //3
  all_dev[3],             //4
  all_dev[4],             //5
  all_dev[5],             //6
  body_watcher,           //7
  candy_watcher,          //8
  new PubSub(all_dev[0],{all_dev[1], all_dev[2], candy_watcher, NULL})              //9
};
int num_cmds = sizeof(pcommands) / sizeof(pcommands[0]);   //
int cur_cmd_idx = num_cmds - 1;

// called by a HalloweenBase module TODO: => event?
bool update_state(int newState) {
  bool ret = cur_state != newState;
  if (ret) {
    Serial.print("changing state from");
    Serial.print(cur_state);
    Serial.print(" to ");
    Serial.println(newState);
    BTSerial.print("=>INFO: changing state from");
    BTSerial.print(cur_state);
    BTSerial.print(" to ");
    BTSerial.println(newState);
    cur_state = newState;
  }
  return ret;
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  randomSeed(analogRead(0));
  BTSerial.begin(BAUDRATE);
  Serial.println("Connect to HC-05 from any other bluetooth device with 1234 as pairing key!.");
  Serial.print("BTserial started at ");
  Serial.println(BAUDRATE);
  HalloweenBase::pOutput = &BTSerial;

  msec_val = millis();
  lmsec_val = msec_val;

  for (HalloweenBase* pcmd = all_dev; pcmd; pcmd++) {
    pcmd++->clean();
  }
}
static int new_state;
static bool bvalid;
void loop() {
  if (BTSerial.available()){
    bvalid = false;
    data = BTSerial.read();
    if (isPrintable(data)) {
      Serial.print("receive data=");
      Serial.println(data);
      int cmd = data - '0';
      if ((cmd >= 0) && (cmd < num_cmds)) {
        pcommands[cur_cmd_idx]->clean();
        new_state = pcommands[cmd]>process(cur_state);
        update_state(new_state);
        bvalid = true;
        cur_cmd_idx = cmd;
      }
    }
    if (!bvalid) {
      Serial.print("WARN: unknown input");
      Serial.println(data);
      BTSerial.print("=>WARN: unknown input");
      BTSerial.println(data);
    }
  }
  // update time
  msec_val = millis();
  if ((msec_val- lmsec_val) >= DURATION) {
    new_state = pcommands[cur_cmd_idx]->updateTime(cur_state, (msec_val- lmsec_val));
    update_state(new_state);
    lmsec_val = msec_val;
  }
}
