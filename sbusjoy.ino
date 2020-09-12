// Program used to map an SBUS transmitter as
// wireless controller
//
// Based on Robin Tripp's original version, but changed for multicopters
// 2018-02-27 - Original Version
// 2020-09-09 - Changes
//------------------------------------------------------------

#include "Joystick.h"
#include <SBUS.h>

#define SBUS_MIN 180
#define SBUS_MAX 1800

#define JS_MIN -127
#define JS_MAX 128

Joystick_ Joystick;
SBUS sbus(Serial1);

bool failsafeActive = false;


void setup() {
  sbus.begin(false);
  Joystick.setXAxisRange(JS_MIN, JS_MAX);
  Joystick.setYAxisRange(JS_MIN, JS_MAX);
  Joystick.setZAxisRange(JS_MIN, JS_MAX);
  Joystick.setThrottleRange(JS_MIN, JS_MAX);
  Joystick.setRudderRange(JS_MIN, JS_MAX);
  Joystick.begin(false);
}

void loop() {
  sbus.process();
  
  int channels[17];
  
  for (int i=1; i<=16; i++) {
//    Serial.print(" CH" + String(i) + ": " + sbus.getChannel(i));
    channels[i]=map(sbus.getChannel(i), SBUS_MIN, SBUS_MAX, JS_MIN, JS_MAX);
  }
//  Serial.println();

  // TAER control
  Joystick.setXAxis(channels[2]); // CH2 Ail (roll) -> X axis
  Joystick.setYAxis(channels[3]); // CH3 Ele (pich) -> Y axis
  //Joystick.setZAxis(channels[4]); // CH4 Rud (also rudder) -> Z axis

  Joystick.setThrottle(channels[1]); // CH1 Throttle
  Joystick.setRudder(channels[4]);   // CH4 Rudder

  // Remaining buttons
  for (int i=5; i<=16; i++) {
    if (channels[i] > 0) {
      Joystick.pressButton(i-5);
    } else {
      Joystick.releaseButton(i-5);
    }
  }

  // Also could have these:
  // sbus.getFrameLoss() - frame loss percent
  // sbus.getGoodFrames() - good frame num
  // sbus.getLostFrames() - lost frames
  // sbus.getDecoderErrorFrames() - frame decode fail
  // (millis() - sbus.getLastTime()) - time diff between received and now
  
  Joystick.sendState();
  if (sbus.getFailsafeStatus() == SBUS_FAILSAFE_ACTIVE && !failsafeActive) {
    Serial.println("Failsafe active!");
    failsafeActive = true;
  }
  if (sbus.getFailsafeStatus() == SBUS_FAILSAFE_INACTIVE && failsafeActive){
    Serial.println("Failsafe recovered");
    failsafeActive = false;
  }
}
