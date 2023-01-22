// using adafruit / Adafruit_AHRS

#define ARDUINO_ARCH_NRF52840
// #define ENCODER_OPTIMIZE_INTERRUPTS
#include <Wire.h>
#include <Adafruit_Sensor_Calibration.h>
#include <Adafruit_AHRS.h>
Adafruit_Sensor *accelerometer, *gyroscope, *magnetometer;
#include "LSM9DS1.h"  // LSM9DS1 or LSM9DS0
#include <Adafruit_NeoPixel.h>
#include <EasyButton.h>
#include <Encoder.h>
#include <NanoBLEFlashPrefs.h>

/* === === === === === IMU === === === === === */

// pick your filter! slower == better quality output
Adafruit_NXPSensorFusion filter;  // slowest
// Adafruit_Madgwick filter;  // faster than NXP
// Adafruit_Mahony filter;  // fastest/smalleset

Adafruit_Sensor_Calibration_SDFat cal;
#define FILTER_UPDATE_RATE_HZ 100
#define PRINT_EVERY_N_UPDATES 10
#define AHRS_DEBUG_OUTPUT

uint32_t timestamp;

/* === === === === === Buttons / pot / encoder === === === === ===*/

// #define BUTTON_PIN D3  // Button switch
#define BUTTON_PIN D4  // Encodder button
// #define POT_PIN A7     // Potentiometer
#define ENC_PIN1 D5
#define ENC_PIN2 D6
Encoder wheelEnc(ENC_PIN1, ENC_PIN2);

/* === === === === === RGB LED === === === === === */

#define LED_PIN D2  // NeoPixel strip

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

/* === === === === === VALUES === === === === === */

/* constant values */
const uint8_t centerPixel = (NUM_PIXELS - 1) / 2;  // Center of the LED strip

/* variable values */

// CyberLevel
double angle;        // Roll angle in degree
double decimalOnly;  // only the decimal of the pixel number
double floatPixel;   // Pixel numer
double pixelPos;     // position of the pixel on the strip
float pixelColor;    // color value of the pixel
float angleOffset;   // offset from potentiometer
int8_t intPixel;     // pixel number without decimal
uint8_t zoom = 20;   // zoom factor of the bubble
uint8_t axis = 1;    // axis displayed

// LED colors
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

// Encoder
int8_t positionWheel = -999;

// Button
uint8_t debounce = 40;
bool pullup = true;
bool invert = true;
EasyButton button(BUTTON_PIN, debounce, pullup, invert);

void (*resetFunc)(void) = 0;  // create a standard reset function


/*
=== === === === === === === === === === === === === === === === === === === === === === === === === === === === === ===
=== === === === === === === === === === === ===  S E T U P  === === === === === === === === === === === === === === ===
=== === === === === === === === === === === === === === === === === === === === === === === === === === === === === ===
*/
void setup() {

//  Start Serial and wait for connection
  Serial.begin(115200, SERIAL_8N1);
  delay(10);
  if (!Serial) {
    Serial.print("Waiting for Serial...");
    delay(1000);
  }
  Serial.println(" === === === Seial started === === === ");

  setupFlash();  // Reads the prefs stored in the flash and loads them.

  // initialize EasyButton
  button.begin();

// Initialize IMU
  if (!cal.begin()) {
    Serial.println("Failed to initialize calibration helper");
  } else if (!cal.loadCalibration()) {
    Serial.println("No calibration loaded/found");
  }

  if (!init_sensors()) {
    Serial.println("Failed to find sensors");
    while (1) delay(10);
  }

  accelerometer->printSensorDetails();
  gyroscope->printSensorDetails();
  magnetometer->printSensorDetails();

  setup_sensors();
  filter.begin(FILTER_UPDATE_RATE_HZ);
  timestamp = millis();

  Wire.setClock(400000);  // 400KHz

  // Initialize NeoPixel strip
  pixels.begin();

  // Set RGB LED pins
  pinMode(LEDR, OUTPUT);
  pinMode(LEDG, OUTPUT);
  pinMode(LEDB, OUTPUT);

  // Turn RGB LEDs on
  rgb_led('i');

  // dim white center pixel
  pixels.clear();
  pixels.setPixelColor(centerPixel, pixels.Color(10, 10, 10));
  pixels.show();

  // EasyButton callbacks
  button.onSequence(1, 300, resetAngle);
  button.onPressedFor(300, changeZoom);
}

/*
=== === === === === === === === === === === === === === === === === === === === === === === === === === === === === ===
=== === === === === === === === === === === === L O O P === === === === === === === === === === === === === === === ===
=== === === === === === === === === === === === === === === === === === === === === === === === === === === === === ===
*/
void loop() {
  // Encoder
  long newWheel = wheelEnc.read();
  if (newWheel != positionWheel) {
    // Serial.print("Wheel = ");
    // Serial.print(newWheel);
    // Serial.println(", ");
    positionWheel = newWheel;
  }

  // Check button status
  button.read();

  /* === === === === IMU === === === === */

  loopIMU();

  /* ---------- CyberLevel ---------- */
  loopLED();

  loopFlash();  // checks for updated prefs and saves them to the flash

  /* === === === === === SERIAL PRINTS === === === === === */

//  Uncomment to DEBUG raw sensor data:
// plot("ax", (1000 * data.ax), false);
// plot("gx", (2 * data.gx), false);
// plot("mx", (data.mx), false);
// plot("ay", (1000 * data.ay), false);
// plot("gy", (data.gy), false);
// plot("my", (data.my), false);
// plot("az", (1000 * data.az), false);
// plot("gz", (data.gz), false);
// plot("mz", (data.mz), false);

// Serial.print("ax = ");
// Serial.print(1000 * data.ax);
// Serial.print("ay = ");
// Serial.print(1000 * data.ay);
// Serial.print("\t");
// Serial.print("\taz = ");
// Serial.print(1000 * data.az);
// Serial.print(" mg");
// Serial.print("\tgx = ");
// Serial.print(data.gx, 2);
// Serial.print("gy = ");
// Serial.println(data.gy, 2);
// Serial.print("\tgz = ");
// Serial.print(data.gz, 2);
// Serial.print(" deg/s\t");
// Serial.print("\tmx = ");
// Serial.print(1000 * data.mx);
// Serial.print("\tmy = ");
// Serial.print(1000 * data.my);
// Serial.print("\tmz = ");
// Serial.print(1000 * data.mz);
// Serial.println(" mG");

// plot("Angles", (angles.pitch * 10), false);
// Serial.print("\tAngles: ");
// Serial.print(angles.pitch);
// Serial.print(F("Euler: "));
// Serial.print((quat.toEuler().y()) * (180 / PI));
// Serial.print(F(", "));
// Serial.print((quat.toEuler().y()) * (180 / PI));
// Serial.print(F(", "));
// Serial.print((quat.toEuler().z()) * (180 / PI));
// Serial.print(F(" "));

// Serial.print(F("\tangle: "));
// Serial.print(angle, 8);

// Serial.print("\tpixelPos: ");
// Serial.print(pixelPos);
// Serial.print("\tangleOffset: ");
// Serial.print(angleOffset);
// plot("Raw_Pot", rawValue, false);
// plot("Smoothed_Pot", Pot, false);
// plot("Encoder", positionWheel, false);
// plot("Angle_Offset", angleOffset, false);
// Serial.print("\tintPixel: ");
// Serial.print(intPixel);
// Serial.print("\tcenterPixel: ");
// Serial.print(centerPixel);
// Serial.print("\tdelta: ");
// Serial.print(intPixel - centerPixel);
// Serial.print("\tdecimal: ");
// Serial.print(decimalOnly);

// Serial.print("\tPot: ");
// Serial.print(Pot);

/* Optional: Display calibration status */
//  displayCalStatus();

/* Optional: Display sensor status (debug only) */
// displaySensorStatus();

// Serial.println("");  // new line for next sample
// Serial.println("Min:-1023,Max:1023");
}