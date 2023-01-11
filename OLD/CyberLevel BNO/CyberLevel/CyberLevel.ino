#include <Adafruit_BNO055.h>
#include <Adafruit_NeoPixel.h>
#include <Adafruit_Sensor.h>
#include <EEPROM.h>
#include <EasyButton.h>
#include <Wire.h>
#include <utility/imumaths.h>

/*
  Connections
  ===========
  BNO055:
  Connect SCL to analog 5
  Connect SDA to analog 4
  Connect VDD to 3-5V DC
  Connect GROUND to common ground
  ===========
  NeoPixel strip: Connect to Digital pin 6
  ===========
  Button: Connect to Digital 5
  ===========
  Potentiometer: Connect to Analog 0 (pin 18)
*/

/* Set the delay between fresh samples */
#define BNO055_SAMPLERATE_DELAY_MS (10)

// Check I2C device address and correct line below (by default address is 0x29
// or 0x28)
//                                   id, address
Adafruit_BNO055 bno = Adafruit_BNO055(55, 0x28);

// Assigning pins
#define LED_PIN 6     // NeoPixel strip
#define BUTTON_PIN 5  // Button switch
#define POT_PIN 18    // Potentiometer

// How many NeoPixels are attached to the Arduino?
#define NUM_PIXELS 20

// Colors on the hue wheel
#define U_RED (65536)             // 65536
#define PURPLE (65536 * 4.5 / 6)  // 54613.3333333
#define BLUE (65536 * 2 / 3)      // 43690.6666667
#define CYAN (65536 / 2)          // 32768
#define GREEN (65536 / 3)         // 21845.3333333
#define YELLOW (65536 / 6)        // 10922.6666667
#define RED (0)                   // 0

// When setting up the NeoPixel library, we tell it how many pixels,
// and which pin to use to send signals. Note that for older NeoPixel
// strips you might need to change the third parameter -- see the
// strandtest example for more information on possible values.
Adafruit_NeoPixel pixels(NUM_PIXELS, LED_PIN, NEO_GRB + NEO_KHZ800);

/* constant values */
const uint8_t centerPixel = (NUM_PIXELS - 1) / 2;  // Center of the LED strip
const int numReadings = 10;

/* variable values */
double angle;        // Roll angle in degree
double decimalOnly;  // only the decimal of the pixel number
double floatPixel;   // Pixel numer
double pixelPos;     // position of the pixel on the strip
float pixelColor;    // color value of the pixel
float angleOffset;   // offset from potentiometer
int8_t intPixel;     // pixel number without decimal
uint8_t zoom;        // zoom factor of the bubble
uint8_t axis;        // axis displayed
  // pot smoothing
float readings[numReadings];  // the readings from the analog input
int readIndex = 0;            // the index of the current reading
float total = 0;              // the running total
float Pot = 0;                // the pot average

/* EEPROM addresses */
uint8_t eeID = 0;
// uint8_t eeZoom = 4;
// uint8_t eeAxis = 6;
// uint8_t eeCalib = 8;
uint8_t eeZoom = eeID + sizeof(long);
uint8_t eeAxis = eeZoom + sizeof(uint8_t);
uint8_t eeCalib = eeAxis + sizeof(uint8_t);

uint32_t bgcolor;     // background color
uint32_t rgbcolor0;   // leading pixel color
uint32_t rgbcolor1;   // main pixel color
uint32_t rgbcolor2;   // trailing pixel color
uint32_t rgbcolorS;   // background of center pixels
uint32_t rgbcolorS0;  // leading pixel when on side
uint32_t rgbcolorS1;  // main pixel when on side
uint32_t rgbcolorS2;  // trailing pixel when on side
uint32_t rgbcolorC;   // background of center pixels
uint32_t rgbcolorC0;  // leading pixel when centered
uint32_t rgbcolorC1;  // main pixel when centered
uint32_t rgbcolorC2;  // trailing pixel when centered

uint8_t debounce = 40;
bool pullup = true;
bool invert = true;
EasyButton button(BUTTON_PIN, debounce, pullup, invert);

void (*resetFunc)(void) = 0;  // create a standard reset function

void displaySensorDetails(void) {
  sensor_t sensor;
  bno.getSensor(&sensor);
  Serial.println("------------------------------------");
  Serial.print("Sensor:       ");
  Serial.println(sensor.name);
  Serial.print("Driver Ver:   ");
  Serial.println(sensor.version);
  Serial.print("Unique ID:    ");
  Serial.println(sensor.sensor_id);
  Serial.print("Max Value:    ");
  Serial.print(sensor.max_value);
  Serial.println(" xxx");
  Serial.print("Min Value:    ");
  Serial.print(sensor.min_value);
  Serial.println(" xxx");
  Serial.print("Resolution:   ");
  Serial.print(sensor.resolution);
  Serial.println(" xxx");
  Serial.println("------------------------------------");
  Serial.println("");
  delay(500);
}

void displaySensorStatus(void) {
  /* Get the system status values (mostly for debugging purposes) */
  uint8_t system_status, self_test_results, system_error;
  system_status = self_test_results = system_error = 0;
  bno.getSystemStatus(&system_status, &self_test_results, &system_error);

  /* Display the results in the Serial Monitor */
  Serial.println("");
  Serial.print("System Status: 0x");
  Serial.println(system_status, HEX);
  Serial.print("Self Test:     0x");
  Serial.println(self_test_results, HEX);
  Serial.print("System Error:  0x");
  Serial.println(system_error, HEX);
  Serial.println("");
  delay(500);
}

void displayCalStatus(void) {
  /* Get the four calibration values (0..3) */
  /* Any sensor data reporting 0 should be ignored, */
  /* 3 means 'fully calibrated" */
  uint8_t system, gyro, accel, mag;
  system = gyro = accel = mag = 0;
  bno.getCalibration(&system, &gyro, &accel, &mag);

  /* The data should be ignored until the system calibration is > 0 */
  Serial.print("\t");
  if (!system) {
    Serial.print("! ");
  }

  /* Display the individual values */
  pixels.clear();
  Serial.print("Sys:");
  Serial.print(system, DEC);
  Serial.print(" G:");
  Serial.print(gyro, DEC);
  Serial.print(" A:");
  Serial.print(accel, DEC);
  Serial.print(" M:");
  Serial.print(mag, DEC);

  // pixels colors for calibration
  pixels.clear();
  if (system == 0) {
    pixels.setPixelColor(0, pixels.Color(255, 0, 0));
  } else if (system == 1) {
    pixels.setPixelColor(0, pixels.Color(0, 0, 255));
  } else if (system == 2) {
    pixels.setPixelColor(0, pixels.Color(0, 255, 0));
  } else if (system == 3) {
    pixels.setPixelColor(0, pixels.Color(100, 100, 100));
  } else {
    pixels.setPixelColor(0, pixels.Color(0, 0, 0));
  }
  if (gyro == 0) {
    pixels.setPixelColor(2, pixels.Color(255, 0, 0));
  } else if (gyro == 1) {
    pixels.setPixelColor(2, pixels.Color(0, 0, 255));
  } else if (gyro == 2) {
    pixels.setPixelColor(2, pixels.Color(0, 255, 0));
  } else if (gyro == 3) {
    pixels.setPixelColor(2, pixels.Color(100, 100, 100));
  } else {
    pixels.setPixelColor(2, pixels.Color(0, 0, 0));
  }
  if (accel == 0) {
    pixels.setPixelColor(4, pixels.Color(255, 0, 0));
  } else if (accel == 1) {
    pixels.setPixelColor(4, pixels.Color(0, 0, 255));
  } else if (accel == 2) {
    pixels.setPixelColor(4, pixels.Color(0, 255, 0));
  } else if (accel == 3) {
    pixels.setPixelColor(4, pixels.Color(100, 100, 100));
  } else {
    pixels.setPixelColor(4, pixels.Color(0, 0, 0));
  }
  if (mag == 0) {
    pixels.setPixelColor(6, pixels.Color(255, 0, 0));
  } else if (mag == 1) {
    pixels.setPixelColor(6, pixels.Color(0, 0, 255));
  } else if (mag == 2) {
    pixels.setPixelColor(6, pixels.Color(0, 255, 0));
  } else if (mag == 3) {
    pixels.setPixelColor(6, pixels.Color(100, 100, 100));
  } else {
    pixels.setPixelColor(6, pixels.Color(0, 0, 0));
  }
  pixels.show();
}

void displaySensorOffsets(const adafruit_bno055_offsets_t &calibData) {
  Serial.print("Accelerometer: ");
  Serial.print(calibData.accel_offset_x);
  Serial.print(" ");
  Serial.print(calibData.accel_offset_y);
  Serial.print(" ");
  Serial.print(calibData.accel_offset_z);
  Serial.print(" ");

  Serial.print("\nGyro: ");
  Serial.print(calibData.gyro_offset_x);
  Serial.print(" ");
  Serial.print(calibData.gyro_offset_y);
  Serial.print(" ");
  Serial.print(calibData.gyro_offset_z);
  Serial.print(" ");

  Serial.print("\nMag: ");
  Serial.print(calibData.mag_offset_x);
  Serial.print(" ");
  Serial.print(calibData.mag_offset_y);
  Serial.print(" ");
  Serial.print(calibData.mag_offset_z);
  Serial.print(" ");

  Serial.print("\nAccel Radius: ");
  Serial.print(calibData.accel_radius);

  Serial.print("\nMag Radius: ");
  Serial.print(calibData.mag_radius);
}

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
        resetFunc();  // reset the Arduino via software function
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
    zoom = 10;
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
    axis = 1;
  }
  // if (axis == 0) {
  //   pixels.clear();
  //   pixels.setPixelColor(centerPixel - 2, 255, 0, 0);
  //   pixels.setPixelColor(centerPixel - 3, 255, 0, 0);
  //   pixels.show();
  //   delay(500);
  // } else
  if (axis == 1) {
    pixels.clear();
    pixels.setPixelColor(centerPixel + 1, 0, 255, 0);
    pixels.setPixelColor(centerPixel - 1, 0, 255, 0);
    pixels.show();
    delay(500);
  } else {
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

void setup(void) {
  Serial.begin(115200);

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

  delay(100);


  // smoothing: initialize all the readings to 0:
  // int initialPot = analogRead(POT_PIN);
  for (int thisReading = 0; thisReading < numReadings; thisReading++) {
    readings[thisReading] = 0;
    Serial.print("Pot Array:");
    Serial.print(thisReading);
    Serial.print(" => ");
    Serial.println(readings[thisReading]);
  }

  Serial.println("Orientation Sensor Test");
  Serial.println("");

  /* Initialise the sensor */
  if (!bno.begin()) {
    /* There was a problem detecting the BNO055 ... check your connections */
    Serial.print("Oops, no BNO055 detected ... Check your wiring or I2C ADDR!");
    while (1)
      ;
  }

  /* addressed for the EEPROM storage */

  long bnoID;
  bool foundCalib = false;

  EEPROM.get(eeID, bnoID);

  adafruit_bno055_offsets_t calibrationData;
  sensor_t sensor;

  /*
    Look for the sensor's unique ID at the beginning oF EEPROM.
    This isn't foolproof, but it's better than nothing.s
  */
  bno.getSensor(&sensor);
  if (bnoID != sensor.sensor_id) {
    Serial.println("\nNo Calibration Data for this sensor exists in EEPROM");
    pixels.setPixelColor(centerPixel,
                         pixels.Color(255, 0, 0));  // dim whit center pixel
    pixels.show();
    delay(500);
  } else {
    Serial.println("\nFound Calibration for this sensor in EEPROM.");
    eeCalib;
    EEPROM.get(eeCalib, calibrationData);

    displaySensorOffsets(calibrationData);

    Serial.println("\n\nRestoring Calibration data to the BNO055...");
    bno.setSensorOffsets(calibrationData);

    Serial.println("\n\nCalibration data loaded into BNO055");
    foundCalib = true;
    //set op mode:
    bno.setMode(OPERATION_MODE_IMUPLUS);
  }

  delay(1000);

  /* Display some basic information on this sensor */
  // displaySensorDetails();

  /* Optional: Display current status */
  displaySensorStatus();

  /* Crystal must be configured AFTER loading calibration data into BNO055. */
  bno.setExtCrystalUse(true);

  sensors_event_t event;
  bno.getEvent(&event);
  /* always recal the mag as It goes out of calibration very often */
  if (foundCalib) {
    Serial.println("Move sensor slightly to calibrate magnetometers");
    while (bno.isFullyCalibrated()) {
      bno.getEvent(&event);
      delay(BNO055_SAMPLERATE_DELAY_MS);
    }
  } else {
    Serial.println("Please Calibrate Sensor: ");
    while (!bno.isFullyCalibrated()) {
      bno.getEvent(&event);

      Serial.print("X: ");
      Serial.print(event.orientation.x, 4);
      Serial.print("\tY: ");
      Serial.print(event.orientation.y, 4);
      Serial.print("\tZ: ");
      Serial.print(event.orientation.z, 4);

      /* Optional: Display calibration status */
      displayCalStatus();

      /* New line for the next sample */
      Serial.println("");

      /* Wait the specified delay before requesting new data */
      delay(BNO055_SAMPLERATE_DELAY_MS);
    }
  }

  Serial.println("\nFully calibrated!");
  Serial.println("--------------------------------");
  Serial.println("Calibration Results: ");
  adafruit_bno055_offsets_t newCalib;
  bno.getSensorOffsets(newCalib);
  displaySensorOffsets(newCalib);

  Serial.println("\n\nStoring calibration data to EEPROM...");

  bno.getSensor(&sensor);
  bnoID = sensor.sensor_id;

  EEPROM.put(eeID, bnoID);
  EEPROM.put(eeCalib, newCalib);

  // Serial.print("\n\nbnoID stored to EEPROM at: ");
  // Serial.println(eeID);
  Serial.print("ID is: ");
  Serial.println(bnoID);

  // Serial.print("Zoom stored to EEPROM at: \t");
  // Serial.println(eeZoom);
  EEPROM.get(eeZoom, zoom);
  Serial.print("Saved Zoom value is: ");
  Serial.println(zoom);
  if (zoom != 5 && zoom != 10 && zoom != 20 && zoom != 40) {
    Serial.println("no zoom value found");
    zoom = 5;
    EEPROM.put(eeZoom, zoom);
  }

  // Serial.print("Axis stored to EEPROM at: \t");
  // Serial.println(eeAxis);
  EEPROM.get(eeAxis, axis);
  Serial.print("Saved Axis value is: ");
  Serial.println(axis);
  if (axis != 1 && axis != 2) {
    Serial.println("no axis value found");
    axis = 1;
    EEPROM.put(eeAxis, axis);
  }

  // Serial.print("Calibration stored to EEPROM at: \t");
  // Serial.println(eeCalib);
  // Serial.print("New calibration is: ");
  // Serial.println(newCalib);

  Serial.print("\Mode: ");
  Serial.print(bno.getMode());

  Serial.println("\n--------------------------------\n");
  // delay(500);

  /* EasyButton callbacks*/
  button.onSequence(1, 300, changeZoom);
  button.onPressedFor(300, changeAxis);
}

void loop() {
  /* Get a new sensor event */
  button.read();
  sensors_event_t event;
  bno.getEvent(&event);
  imu::Quaternion quat = bno.getQuat();  // pulling fused quaternion
  // imu::Vector<3> grav = bno.getVector(
  //     Adafruit_BNO055::VECTOR_GRAVITY); // gravity for low mode detection

  /* assigns the axis to angle */
  switch (axis) {
    case 0:
      angle = (quat.toEuler().x()) * (180 / PI);
      break;
    case 1:
      angle = (quat.toEuler().y()) * (180 / PI);
      break;
    case 2:
      angle = (quat.toEuler().z()) * (180 / PI);
      break;
    default:
      angle = (quat.toEuler().y()) * (180 / PI);
      break;
  }

  /* Offset the angle with the potentiometer */
  //float Pot = analogRead(POT_PIN);            // 0 to 1023
  /* smoothing the pot input*/
  // subtract the last reading:
  total = total - readings[readIndex];
  // read from the sensor:
  readings[readIndex] = analogRead(POT_PIN);
  // add the reading to the total:
  total = total + readings[readIndex];
  // advance to the next position in the array:
  readIndex = readIndex + 1;

  // if we're at the end of the array...
  if (readIndex >= numReadings) {
    // ...wrap around to the beginning:
    readIndex = 0;
  }
  // calculate the average:
  Pot = total / numReadings;
  // send it to the computer as ASCII digits
  // Serial.print(" -- Pot : ");
  // Serial.println(Pot);

  angleOffset = (Pot / (1023 / 2) * 10) - 10;  // offset bubble with pot
  // angleOffset = 0;                            //! Disables offset !

  /* angle to pixel with offset and zoom factored in */
  pixelPos = ((angle + angleOffset) / zoom) * NUM_PIXELS;

  floatPixel = pixelPos + centerPixel + 1;
  intPixel = floatPixel;
  decimalOnly = floatPixel - intPixel;

  // Serial.print(F("\tangle: "));
  // Serial.print(angle, 8);

  /* Pixel Color changes in hue from green to red */
  pixelColor = (((abs(pixelPos) / 6) * (RED - PURPLE)) + PURPLE);
  if (pixelColor > GREEN) {
    pixelColor = GREEN;
  } else if (pixelColor < RED) {
    pixelColor = RED;
  }

  /* Hue values for the pixels. */
  bgcolor = pixels.Color(1, 1, 1);
  rgbcolor0 = pixels.ColorHSV(pixelColor, 255, (decimalOnly * 255) * 0.5);          // leading pixel
  rgbcolor1 = pixels.ColorHSV(pixelColor, 255, 255 * 0.5);                          // main pixel
  rgbcolor2 = pixels.ColorHSV(pixelColor, 255, (255 - (decimalOnly * 255)) * 0.5);  // trailing pixel
  // rgbcolorS = pixels.Color(1, 1, 1);
  rgbcolorS = pixels.ColorHSV(BLUE, 255, 0);
  rgbcolorS0 = pixels.ColorHSV(BLUE, 255, decimalOnly * 255);
  rgbcolorS1 = pixels.ColorHSV(BLUE, 255, 255);
  rgbcolorS2 = pixels.ColorHSV(BLUE, 255, 255 - (decimalOnly * 255));
  rgbcolorC = pixels.ColorHSV(PURPLE, 255, 1);
  rgbcolorC0 = pixels.ColorHSV(PURPLE, 255, (decimalOnly * 255) * 0.08);
  rgbcolorC1 = pixels.ColorHSV(PURPLE, 255, 255 * 0.08);
  rgbcolorC2 = pixels.ColorHSV(PURPLE, 255, (255 - (decimalOnly * 255)) * 0.08);

  /* Makes the pixel move according to the angle and change color accordingly */
  pixels.clear();  // Set all pixel to 'off'
  // pixels.fill(bgcolor, 0, NUM_PIXELS);  // background color

  if (intPixel >= NUM_PIXELS) {  // out of bounds make last pixel on.
    pixels.setPixelColor(centerPixel, rgbcolorC);
    pixels.setPixelColor(centerPixel - 1, rgbcolorS);
    pixels.setPixelColor(centerPixel + 1, rgbcolorS);
    pixels.setPixelColor(NUM_PIXELS - 1, 255, 0, 0);
  } else if (intPixel <= 0) {  // out of bounds make first pixel on.
    pixels.setPixelColor(centerPixel, rgbcolorC);
    pixels.setPixelColor(centerPixel - 1, rgbcolorS);
    pixels.setPixelColor(centerPixel + 1, rgbcolorS);
    pixels.setPixelColor(0, 255, 0, 0);
  } else if (intPixel == centerPixel - 2) {  // center pixel stays blue
    pixels.setPixelColor(intPixel, rgbcolor1);
    pixels.setPixelColor(intPixel - 1, rgbcolor2);
    pixels.setPixelColor(centerPixel - 1, rgbcolorS0);
    pixels.setPixelColor(centerPixel, rgbcolorC);
    pixels.setPixelColor(centerPixel + 1, rgbcolorS);
  } else if (intPixel == centerPixel - 1) {  // center pixel stays blue
    pixels.setPixelColor(intPixel - 1, rgbcolor2);
    pixels.setPixelColor(centerPixel - 1, rgbcolorS1);
    pixels.setPixelColor(centerPixel, rgbcolorC0);
    pixels.setPixelColor(centerPixel + 1, rgbcolorS);
  } else if (intPixel == centerPixel) {
    pixels.setPixelColor(centerPixel - 1, rgbcolorS2);
    pixels.setPixelColor(centerPixel, rgbcolorC1);
    pixels.setPixelColor(centerPixel + 1, rgbcolorS0);
  } else if (intPixel == centerPixel + 1) {  // center pixel stays blue
    pixels.setPixelColor(intPixel + 1, rgbcolor0);
    pixels.setPixelColor(centerPixel - 1, rgbcolorS);
    pixels.setPixelColor(centerPixel, rgbcolorC2);
    pixels.setPixelColor(centerPixel + 1, rgbcolorS1);
  } else if (intPixel == centerPixel + 2) {  // center pixel stays blue
    pixels.setPixelColor(intPixel, rgbcolor1);
    pixels.setPixelColor(intPixel + 1, rgbcolor0);
    pixels.setPixelColor(centerPixel - 1, rgbcolorS);
    pixels.setPixelColor(centerPixel, rgbcolorC);
    pixels.setPixelColor(centerPixel + 1, rgbcolorS2);
  } else {
    pixels.setPixelColor(intPixel + 1, rgbcolor0);
    pixels.setPixelColor(intPixel, rgbcolor1);
    pixels.setPixelColor(intPixel - 1, rgbcolor2);
    pixels.setPixelColor(centerPixel - 1, rgbcolorS);
    pixels.setPixelColor(centerPixel, rgbcolorC);
    pixels.setPixelColor(centerPixel + 1, rgbcolorS);
  }
  pixels.show();  // displays the pixels

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
  Serial.print("\tangleOffset: ");
  Serial.print(angleOffset);
  // Serial.print("\tintPixel: ");
  // Serial.print(intPixel);
  // Serial.print("\tcenterPixel: ");
  // Serial.print(centerPixel);
  // Serial.print("\tdelta: ");
  // Serial.print(intPixel - centerPixel);
  // Serial.print("\tdecimal: ");
  // Serial.print(decimalOnly);

  /* Optional: Display calibration status */
  //  displayCalStatus();

  /* Optional: Display sensor status (debug only) */
  // displaySensorStatus();
  Serial.println("");  // new line for next sample
  delay(BNO055_SAMPLERATE_DELAY_MS);
}