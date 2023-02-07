// Use this with serial plotter to get a nice plot of touch positions

#include "SoftPotMagic.h"


// Change this to set the interval of sampling data (5ms by default)
const int INTERVAL = 5;

// ADC pins used (A0 for left side and A1 for right side)
const int ADC_LEFT = A0;
const int ADC_RIGHT = A1;

bool noTouch = true;
int firstTouch = 255;
int deltaTouch = 0;
int changedTouch = 0;

void setup() {
  // Set the analog pins that you used to connect to the SoftPot (in this case,
  // A0 and A1)
  SoftPotMagic.begin(ADC_LEFT, ADC_RIGHT);

  Serial.begin(115200);

  // Do a manual calibration with 10kOhm test resistors and 10kOhm SoftPot (the
  // typical setup)
  SoftPotMagic.setCalib(10000.0, 10000.0, 10000.0, 1023, 0);

  // Find touch release threshold (might be necessary on some boards)
  SoftPotMagic.autoCalibZero();

  // Minimum gap ratio, any gaps detected that is bigger than specified will be
  // treated as multitouch
  SoftPotMagic.setMinGapRatio(0.50f);
}

void loop() {
  // Make sure to run this function to refresh the value
  SoftPotMagic.update();

  const int touch1 = SoftPotMagic.pos1();
  const int touch2 = SoftPotMagic.pos2();

  if (touch1 != 255) {    // Check if touched (at least with one finger)
    noTouch = false;
    if (touch2 != 255) {  // If touched with two fingers, register as a button.
        Serial.println("=Button=");
    } else {  // otherwise register as slider

        if (touch2 != 255) {   // check that second finger doesnt comes in later.
            Serial.println("=Button= +");
        } else {  // register the slider delta and save the last delta
          if (firstTouch == 255) {
            firstTouch = touch1;
            Serial.print("First Touch: ");
            Serial.println(firstTouch);
          } else {
            deltaTouch = firstTouch - touch1;
            Serial.print("Delta: ");
            Serial.println(deltaTouch);
          }
        }
      // }
    }
  } else {
    firstTouch = 255;
    deltaTouch = 0;
    Serial.println("no touch");
    noTouch = true;
  }
  // Sleep for INTERVAL ms
  delay(INTERVAL);
}
