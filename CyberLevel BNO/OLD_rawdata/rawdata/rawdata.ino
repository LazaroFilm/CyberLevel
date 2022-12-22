#include <Adafruit_BNO055.h>
#include <Adafruit_NeoPixel.h>
#include <Adafruit_Sensor.h>
#include <EEPROM.h>
#include <EasyButton.h>
#include <Wire.h>
#include <utility/imumaths.h>

/* This driver reads raw data from the BNO055

  Connections
  ===========
  Connect SCL to analog 5
  Connect SDA to analog 4
  Connect VDD to 3.3V DC
  Connect GROUND to common ground

  History
  =======*
  2015/MAR/03  - First release (KTOWN)
*/

/* Set the delay between fresh samples */
#define BNO055_SAMPLERATE_DELAY_MS (0)

// Check I2C device address and correct line below (by default address is 0x29
// or 0x28)
//                                   id, address
Adafruit_BNO055 bno = Adafruit_BNO055(-1, 0x28);

// Assigning pins
#define LED_PIN 6    // NeoPixel strip
#define BUTTON_PIN 5 // Button switch
#define POT_PIN 18   // Potentiometer

// How many NeoPixels are attached to the Arduino?
#define NUM_PIXELS 20

// Colors on the hue wheel
#define U_RED (65536)          // 65536
#define PURPLE (65536 * 5 / 6) // 54613.3333333
#define BLUE (65536 * 2 / 3)   // 43690.6666667
#define CYAN (65536 / 2)       // 32768
#define GREEN (65536 / 3)      // 21845.3333333
#define YELLOW (65536 / 6)     // 10922.6666667
#define RED (0)                // 0

// When setting up the NeoPixel library, we tell it how many pixels,
// and which pin to use to send signals. Note that for older NeoPixel
// strips you might need to change the third parameter -- see the
// strandtest example for more information on possible values.
Adafruit_NeoPixel pixels(NUM_PIXELS, LED_PIN, NEO_GRB + NEO_KHZ800);

/* constant values */
const uint8_t centerPixel = (NUM_PIXELS - 1) / 2; // Center of the LED strip

/* variable values */
double angle;       // Roll angle in degree
double decimalOnly; // only the decimal of the pixel number
double floatPixel;  // Pixel numer
double pixelPos;    // position of the pixel on the strip
float pixelColor;   // color value of the pixel
float angleOffset;  // offset from potentiometer
int8_t intPixel;    // pixel number without decimal
uint8_t zoom;       // zoom factor of the bubble
uint8_t axis;

/* EEPROM addresses */
// uint8_t eeID = 0;
// uint8_t eeZoom = 4;
// uint8_t eeAxis = 6;
// uint8_t eeCalib = 8;
uint8_t eeZoom = 0;
uint8_t eeAxis = eeZoom + sizeof(uint8_t);
// uint8_t eeCalib = eeAxis + sizeof(uint8_t);

uint32_t rgbcolor1;  // main pixel color
uint32_t rgbcolor2;  // trailing pixel color
uint32_t rgbcolorC0; // center marker
uint32_t rgbcolorC1; // main pixel when centered
uint32_t rgbcolorC2; // trailing pixel when centered

uint8_t debounce = 40;
bool pullup = true;
bool invert = true;
EasyButton button(BUTTON_PIN, debounce, pullup, invert);

void (*resetFunc)(void) = 0; // create a standard reset function

void resetEepromStart(void) {
  pixels.clear();
  bool reset = false;
  for (int i = 0; i <= centerPixel; i++) {
    button.read();
    if (button.isPressed()) {
      if ((i == centerPixel)) {
        pixels.clear();
        pixels.setPixelColor(centerPixel, pixels.Color(50, 0, 0));
        pixels.setPixelColor(centerPixel + 1, pixels.Color(50, 0, 0));
        pixels.show();
        for (int i = 0; i < EEPROM.length(); i++) {
          EEPROM.write(i, 0);
        }
        delay(10);
        pixels.setPixelColor(centerPixel, pixels.Color(0, 50, 0));
        pixels.setPixelColor(centerPixel + 1, pixels.Color(0, 50, 0));
        pixels.show();
        delay(500);
        pixels.clear();
        pixels.show();
        resetFunc(); // reset the Arduino via software function
      } else {
        pixels.setPixelColor(i, 50, 0, 0);
        pixels.setPixelColor(NUM_PIXELS - 1 - i, 50, 0, 0);
        pixels.show();
        delay(500);
      }
    } else {
      pixels.clear();
      pixels.setPixelColor(centerPixel, pixels.Color(0, 0, 50));
      pixels.setPixelColor(centerPixel + 1, pixels.Color(0, 0, 50));
      pixels.show();
      delay(500);
      resetFunc();
    }
  }
}

void changeZoom() {
  Serial.println("Button is clicked");
  /* change zoom on a short press of a button */
  if (zoom == 40) {
    zoom = 5;
    pixels.clear();
    pixels.setPixelColor(centerPixel + 1, 128, 0, 255);
    pixels.setPixelColor(centerPixel - 1, 128, 0, 255);
    pixels.show();
    EEPROM.update(eeZoom, 5);
    delay(500);
  } else if (zoom == 5) {
    zoom = 10;
    pixels.clear();
    pixels.setPixelColor(centerPixel + 3, 128, 0, 255);
    pixels.setPixelColor(centerPixel - 3, 128, 0, 255);
    pixels.show();
    EEPROM.update(eeZoom, 10);
    delay(500);
  } else if (zoom == 10) {
    zoom = 20;
    pixels.clear();
    pixels.setPixelColor(centerPixel + 5, 128, 0, 255);
    pixels.setPixelColor(centerPixel - 5, 128, 0, 255);
    pixels.show();
    EEPROM.update(eeZoom, 20);
    delay(500);
  } else if (zoom == 20) {
    zoom = 40;
    pixels.clear();
    pixels.setPixelColor(NUM_PIXELS - 1, 128, 0, 255);
    pixels.setPixelColor(0, 128, 0, 255);
    pixels.show();
    EEPROM.update(eeZoom, 40);
    delay(500);
  } else {
    zoom = 5;
    pixels.clear();
    pixels.setPixelColor(centerPixel + 3, 255, 0, 0);
    pixels.setPixelColor(centerPixel - 3, 255, 0, 0);
    pixels.show();
    EEPROM.update(eeZoom, 5);
    delay(500);
  }
  EEPROM.put(eeZoom, zoom);
  Serial.print("\n\nZoom changed to: ");
  Serial.println(zoom);
}

void changeAxis() {
  Serial.print("Button is held");
  /* change axis on a long press of a button */
  if (axis < 2) {
    axis++;
  } else {
    axis = 0;
  }
  if (axis == 0) {
    pixels.clear();
    pixels.setPixelColor(centerPixel - 2, 255, 0, 0);
    pixels.setPixelColor(centerPixel - 3, 255, 0, 0);
    pixels.show();
    delay(500);
  } else if (axis == 1) {
    pixels.clear();
    pixels.setPixelColor(centerPixel + 1, 0, 255, 0);
    pixels.setPixelColor(centerPixel - 1, 0, 255, 0);
    pixels.show();
    delay(500);
  } else if (axis == 2) {
    pixels.clear();
    pixels.setPixelColor(centerPixel + 2, 0, 0, 255);
    pixels.setPixelColor(centerPixel + 3, 0, 0, 255);
    pixels.show();
    delay(500);
  }
  EEPROM.put(eeAxis, axis);
  Serial.print("\n\nAxis changed to: ");
  Serial.println(axis);
}

/**************************************************************************/
/*
    Arduino setup function (automatically called at startup)
*/
/**************************************************************************/
void setup(void) {
  Serial.begin(115200);
  Serial.print("Serial Start");

  /* initializing EasyButton */
  button.begin();

  /* INITIALIZE NeoPixel strip object (REQUIRED) */
  pixels.begin();
  pixels.clear();
  /* dim white center pixel */
  pixels.setPixelColor(centerPixel, pixels.Color(10, 10, 10));
  pixels.show();
  /* EEPROM reset if btn pressed */
  if (button.isPressed()) {
    resetEepromStart();
  }

  Serial.println("Orientation Sensor Raw Data Test");
  Serial.println("");

  /* Initialise the sensor */
  if (!bno.begin()) {
    /* There was a problem detecting the BNO055 ... check your connections */
    Serial.print("Oops, no BNO055 detected ... Check your wiring or I2C ADDR!");
    while (1)
      ;
  }

  delay(1000);

  /* Display the current temperature */
  int8_t temp = bno.getTemp();
  Serial.print("Current Temperature: ");
  Serial.print(temp);
  Serial.println(" C");
  Serial.println("");

  bno.setExtCrystalUse(true);

  Serial.println(
      "Calibration status values: 0=uncalibrated, 3=fully calibrated");

  Serial.print("Zoom stored to EEPROM at: \t");
  Serial.println(eeZoom);
  EEPROM.get(eeZoom, zoom);
  Serial.print("Saved Zoom value is: ");
  Serial.println(zoom);
  if (zoom != 5 && zoom != 10 && zoom != 20 && zoom != 40) {
    Serial.println("no zoom value found");
    zoom = 5;
    EEPROM.put(eeZoom, zoom);
  }

  Serial.print("Axis stored to EEPROM at: \t");
  Serial.println(eeAxis);
  EEPROM.get(eeAxis, axis);
  Serial.print("Saved Axis value is: ");
  Serial.println(axis);
  if (axis != 0 && axis != 1 && axis != 2) {
    Serial.println("no axis value found");
    axis = 1;
    EEPROM.put(eeAxis, axis);
  }

  /* EasyButton callbacks*/
  button.onSequence(1, 300, changeZoom);
  button.onPressedFor(300, changeAxis);
}

/**************************************************************************/
/*
    Arduino loop function, called once 'setup' is complete (your own code
    should go here)
*/
/**************************************************************************/
void loop(void) {
  button.read();
  // Possible vector values can be:
  // - VECTOR_ACCELEROMETER - m/s^2
  // - VECTOR_MAGNETOMETER  - uT
  // - VECTOR_GYROSCOPE     - rad/s
  // - VECTOR_EULER         - degrees
  // - VECTOR_LINEARACCEL   - m/s^2
  // - VECTOR_GRAVITY       - m/s^2
  imu::Vector<3> euler = bno.getVector(Adafruit_BNO055::VECTOR_EULER);
  imu::Vector<3> grav = bno.getVector(
      Adafruit_BNO055::VECTOR_GRAVITY); // gravity for low mode detection

  /* Display the floating point data */
  // Serial.print("X: ");
  // Serial.print(euler.x());
  Serial.print(" Y: ");
  Serial.print(euler.y());
  // Serial.print(" Z: ");
  // Serial.print(euler.z());
  Serial.print("GravX: ");
  Serial.print(grav.x());
  Serial.print("GravY: ");
  Serial.print(grav.y());
  Serial.print("GravZ: ");
  Serial.print(grav.z());
  Serial.print("\t\t");

  /*
  // Quaternion data
  imu::Quaternion quat = bno.getQuat();
  Serial.print("qW: ");
  Serial.print(quat.w(), 4);
  Serial.print(" qX: ");
  Serial.print(quat.x(), 4);
  Serial.print(" qY: ");
  Serial.print(quat.y(), 4);
  Serial.print(" qZ: ");
  Serial.print(quat.z(), 4);
  Serial.print("\t\t");
  */

  /* Display calibration status for each sensor. */
  // uint8_t system, gyro, accel, mag = 0;
  // bno.getCalibration(&system, &gyro, &accel, &mag);
  // Serial.print("CALIBRATION: Sys=");
  // Serial.print(system, DEC);
  // Serial.print(" Gyro=");
  // Serial.print(gyro, DEC);
  // Serial.print(" Accel=");
  // Serial.print(accel, DEC);
  // Serial.print(" Mag=");
  // Serial.println(mag, DEC);

  /* assigns the axis to angle */
  switch (axis) {
  case 0:
    angle = euler.x();
    break;
  case 1:
    angle = euler.y();
    break;
  case 2:
    angle = euler.z();
    break;
  default:
    angle = euler.y();
    break;
  }

  /* Offset the angle with the potentiometer */
  float Pot = analogRead(POT_PIN);            // 0 to 1023
  angleOffset = (Pot / (1023 / 2) * 20) - 20; // offset bubble with pot
  // angleOffset = 0;                            //! Disables offset !

  /* angle to pixel with offset and zoom factored in */
  pixelPos = ((angle + angleOffset) / zoom) * -NUM_PIXELS;

  floatPixel = pixelPos + centerPixel + 1;
  intPixel = floatPixel;
  decimalOnly = floatPixel - intPixel;

  Serial.print(F("\tangle: "));
  Serial.print(angle, 8);

  /* Pixel Color changes in hue from green to red */
  pixelColor = (((abs(pixelPos) / 3) * (RED - CYAN)) + CYAN);
  if (pixelColor > GREEN) {
    pixelColor = GREEN;
  } else if (pixelColor < RED) {
    pixelColor = RED;
  }

  /* Hue values for the pixels. */
  rgbcolor1 = pixels.ColorHSV(pixelColor, 255, decimalOnly * 255);
  rgbcolor2 = pixels.ColorHSV(pixelColor, 255, 255 - (decimalOnly * 255));
  rgbcolorC0 = pixels.ColorHSV(BLUE, 255, 5);
  rgbcolorC1 = pixels.ColorHSV(BLUE, 255, decimalOnly * 255);
  rgbcolorC2 = pixels.ColorHSV(BLUE, 255, 255 - (decimalOnly * 255));

  /* Makes the pixel move according to the angle and change color accordingly */
  pixels.clear();               // Set all pixel to 'off'
  if (intPixel >= NUM_PIXELS) { // out of bounds make last pixel on.
    pixels.setPixelColor(centerPixel, rgbcolorC0);
    pixels.setPixelColor(NUM_PIXELS - 1, 255, 0, 0);
  } else if (intPixel <= 0) { // out of bounds make first pixel on.
    pixels.setPixelColor(centerPixel, rgbcolorC0);
    pixels.setPixelColor(0, 255, 0, 0);
  } else if (intPixel == centerPixel) { // center pixel stays blue
    pixels.setPixelColor(centerPixel, rgbcolorC1);
    pixels.setPixelColor(intPixel - 1, rgbcolor2);
  } else if (intPixel - 1 == centerPixel) { // center pixel stays blue
    pixels.setPixelColor(intPixel, rgbcolor1);
    pixels.setPixelColor(centerPixel, rgbcolorC2);
  } else {
    pixels.setPixelColor(centerPixel, rgbcolorC0);
    pixels.setPixelColor(intPixel, rgbcolor1);
    pixels.setPixelColor(intPixel - 1, rgbcolor2);
  }
  pixels.show(); // displays the pixels

  // Serial.print(F("Euler: "));
  // Serial.print((quat.toEuler().y()) * (180 / PI));
  // Serial.print(F(", "));
  // Serial.print((quat.toEuler().y()) * (180 / PI));
  // Serial.print(F(", "));
  // Serial.print((quat.toEuler().z()) * (180 / PI));
  // Serial.print(F(" "));

  // Serial.print("\tangle: ");
  // Serial.print(angle);
  // Serial.print("\tpixelPos: ");
  // Serial.print(pixelPos);
  // Serial.print("\tangleOffset: ");
  // Serial.print(angleOffset);

  /* Optional: Display calibration status */
  //  displayCalStatus();

  /* Optional: Display sensor status (debug only) */
  // displaySensorStatus();
  Serial.println(""); // new line for next sample
  delay(BNO055_SAMPLERATE_DELAY_MS);
}
