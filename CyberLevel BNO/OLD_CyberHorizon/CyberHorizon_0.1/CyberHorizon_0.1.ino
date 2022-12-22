// BNO055
#include <Adafruit_BNO055.h>
#include <Adafruit_Sensor.h>
#include <EEPROM.h>
#include <Wire.h>
#include <utility/imumaths.h>

// NeoPixel
#include <Adafruit_NeoPixel.h>
//#ifdef __AVR__
// #include <avr/power.h> // Required for 16 MHz Adafruit Trinket
//#endif

/*
This driver uses the Adafruit unified sensor library (Adafruit_Sensor),
  which provides a common 'type' for sensor data and some helper functions.

  To use this driver you will also need to download the Adafruit_Sensor
  library and include it in your libraries folder.

  You should also assign a unique ID to this sensor for use with
  the Adafruit Sensor API so that you can identify this particular
  sensor in any data logs, etc.  To assign a unique ID, simply
  provide an appropriate value in the constructor below (12345
  is used by default in this example).

  Connections
  ===========
  Connect SCL to analog 5
  Connect SDA to analog 4
  Connect VDD to 3-5V DC
  Connect GROUND to common ground

  History
  =======
  2015/MAR/03  - First release (KTOWN)
  2015/AUG/27  - Added calibration and system status helpers
  2015/NOV/13  - Added calibration save and restore
*/

/* Set the delay between fresh samples */
#define DELAYVAL 100 // Time (in milliseconds) to pause between pixels
#define BNO055_SAMPLERATE_DELAY_MS (0)

// Check I2C device address and correct line below (by default address is 0x29
// or 0x28)
//                                   id, address
Adafruit_BNO055 bno = Adafruit_BNO055(55, 0x28);

/**************************************************************************/
// NeoPixel
/**************************************************************************/

// Which pin on the Arduino is connected to the NeoPixels?
#define PIN 6 // On Trinket or Gemma, suggest changing this to 1

// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS 16

#define URED (65536)
#define PURPLE (65536 * 5 / 6) // 54613.3333333
#define BLUE (65536 * 2 / 3)   // 43,690.6666667
#define CYAN (65536 / 2)       // 32768
#define GREEN (65536 / 3)      // 21845.3333333
#define YELLOW (65536 / 6)     // 10922.6666667
#define RED (0)

// When setting up the NeoPixel library, we tell it how many pixels,
// and which pin to use to send signals. Note that for older NeoPixel
// strips you might need to change the third parameter -- see the
// strandtest example for more information on possible values.
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

float Bubble;
float Gravity;
float Decimal;
float FloatPixel;
float PixelPos;
float PixelColor;
int IntPixel;
int CenterPixel;
int Brightness;
bool HighMode;
uint32_t rgbcolor1;
uint32_t rgbcolor2;

/**************************************************************************/
/*
    Displays some basic information on this sensor from the unified
    sensor API sensor_t type (see Adafruit_Sensor for more information)
*/
/**************************************************************************/
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

/**************************************************************************/
/*
    Display some basic info about the sensor status
*/
/**************************************************************************/
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

/**************************************************************************/
/*
    Display sensor calibration status
*/
/**************************************************************************/
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
  Serial.print("Sys:");
  Serial.print(system, DEC);
  Serial.print(" G:");
  Serial.print(gyro, DEC);
  Serial.print(" A:");
  Serial.print(accel, DEC);
  Serial.print(" M:");
  Serial.print(mag, DEC);
}

/**************************************************************************/
/*
    Display the raw calibration offset and radius data
*/
/**************************************************************************/
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

/**************************************************************************/
/*
    Arduino setup function (automatically called at startup)
*/
/**************************************************************************/
void setup(void) {
  CenterPixel = (NUMPIXELS - 1) / 2;

  pixels.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
  pixels.clear();
  pixels.setPixelColor(CenterPixel, pixels.Color(10, 10, 10));
  pixels.show();

  Serial.begin(115200);
  delay(1000);
  Serial.println("Orientation Sensor Test");
  Serial.println("");

  /* Initialise the sensor */
  if (!bno.begin()) {
    /* There was a problem detecting the BNO055 ... check your connections */
    Serial.print("Oops, no BNO055 detected ... Check your wiring or I2C ADDR!");
    while (1)
      ;
  }

  int eeAddress = 0;
  long bnoID;
  bool foundCalib = false;

  EEPROM.get(eeAddress, bnoID);

  adafruit_bno055_offsets_t calibrationData;
  sensor_t sensor;

  /*
    Look for the sensor's unique ID at the beginning oF EEPROM.
    This isn't foolproof, but it's better than nothing.
  */
  bno.getSensor(&sensor);
  if (bnoID != sensor.sensor_id) {
    Serial.println("\nNo Calibration Data for this sensor exists in EEPROM");
    delay(500);
  } else {
    Serial.println("\nFound Calibration for this sensor in EEPROM.");
    eeAddress += sizeof(long);
    EEPROM.get(eeAddress, calibrationData);

    displaySensorOffsets(calibrationData);

    Serial.println("\n\nRestoring Calibration data to the BNO055...");
    bno.setSensorOffsets(calibrationData);

    Serial.println("\n\nCalibration data loaded into BNO055");
    foundCalib = true;
  }

  delay(1000);

  /* Display some basic information on this sensor */
  displaySensorDetails();

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

  eeAddress = 0;
  bno.getSensor(&sensor);
  bnoID = sensor.sensor_id;

  EEPROM.put(eeAddress, bnoID);

  eeAddress += sizeof(long);
  EEPROM.put(eeAddress, newCalib);
  Serial.println("Data stored to EEPROM.");

  bno.setMode(OPERATION_MODE_IMUPLUS);

  Serial.println("\n--------------------------------\n");
  delay(500);
}

void loop() {
  /* Get a new sensor event */
  sensors_event_t event;
  // sensors_event_t event, euler, gravity;

  bno.getEvent(&event);
  // bno.getEvent(&euler, Adafruit_BNO055::VECTOR_EULER);
  // bno.getEvent(&gravity, Adafruit_BNO055::VECTOR_GRAVITY);

  /* Display the floating point data */
  //  Serial.print("X: ");
  //  Serial.print(event.orientation.x, 4);
  Serial.print("Y: ");
  Serial.print(event.orientation.y, 4);
  //  Serial.print("\tZ: ");
  //  Serial.print(event.orientation.z, 4);

  /* Optional: Display calibration status */
  //  displayCalStatus();

  /* Optional: Display sensor status (debug only) */
  // displaySensorStatus();

  //    /* Wait the specified delay before requesting new data */
  //    delay(BNO055_SAMPLERATE_DELAY_MS);

  // NeoPixel

  Bubble = event.orientation.y;
  Gravity = event.acceleration.y;

  PixelPos = (Bubble / 20) * NUMPIXELS;
  FloatPixel = PixelPos + CenterPixel + 1;
  IntPixel = FloatPixel;
  Decimal = FloatPixel - IntPixel;
  Brightness = 255;

  // High or Low mode
  if (Gravity >= 0) {
    HighMode = true;
    Serial.print("HighMode");
  } else {
    HighMode = false;
    Serial.print("LowMode");
  }

  // Pixel Color changes in hue from ured to blue
  PixelColor = (((abs(PixelPos) / 3) * (URED - BLUE)) + BLUE);
  if (PixelColor > URED) {
    PixelColor = URED;
  }

  // HSV values for the pixels.
  rgbcolor1 = pixels.ColorHSV(PixelColor, 255, Decimal * 255);
  rgbcolor2 = pixels.ColorHSV(PixelColor, 255, 255 - (Decimal * 255));

  // Serial.print("\tBubble: ");
  // Serial.print(Bubble, 5);
  // Serial.print("\tGravity: ");
  // Serial.print(Gravity);
  // Serial.print("\tPixelPos: ");
  // Serial.print(PixelPos);
  // Serial.print("\tColor: ");
  // Serial.print(PixelColor);
  // Serial.print("\tFloatPixel: ");
  // Serial.print(FloatPixel, 5);
  // Serial.print("\tIntPixel: ");
  // Serial.print(IntPixel);
  // Serial.print("\tDecimal: ");
  // Serial.print(Decimal, 5);

  pixels.clear(); // Set all pixel colors to 'off'

  if (IntPixel > NUMPIXELS - 1) { // out of bounds make last pixel on.
    pixels.setPixelColor(CenterPixel, pixels.Color(0, 5, 0));
    pixels.setPixelColor(NUMPIXELS - 1, 255, 0, 0);
  } else if (IntPixel < 1) { // out of bounds make last pixel on.
    pixels.setPixelColor(CenterPixel, pixels.Color(0, 5, 0));
    pixels.setPixelColor(0, 255, 0, 0);
  } else {
    pixels.setPixelColor(CenterPixel, pixels.Color(0, 5, 0));
    pixels.setPixelColor(IntPixel, rgbcolor1);
    pixels.setPixelColor(IntPixel - 1, rgbcolor2);
  }

  pixels.show();
  /* New line for the next sample */
  Serial.println("");
}
