// for Arduino Nano RP2040 Connect https://docs.arduino.cc/hardware/nano-rp2040-connect

// using adafruit/Adafruit_LSM6DS https://github.com/adafruit/Adafruit_LSM6DS

// and xioTechnologies/Fusion https://github.com/xioTechnologies/Fusion

#define ARDUINO_NANO_RP2040_CONNECT
// #define ENCODER_OPTIMIZE_INTERRUPTS
#include <Wire.h>
#include <WiFiNINA.h>
#include <Adafruit_NeoPixel.h>
#include <EasyButton.h>
#include <Encoder.h>
// #include <NanoBLEFlashPrefs.h>
//IMU/Fusion
#include <Adafruit_LSM6DSOX.h>
// #include <Arduino_LSM9DS1.h>  //Femme's version
#include "Fusion.h"

/* === === === === === IMU/Fusion === === === === === */
// IMU
Adafruit_LSM6DSOX sox;

// #define SAMPLE_PERIOD (0.01f)  // replace this with actual sample period
#define SAMPLE_RATE (243)  // replace this with actual sample rate


/* === === === === === Buttons / pot / encoder === === === === ===*/

// #define BUTTON_PIN D3  // Button switch
#define BUTTON_PIN 4  // Encodder button
// #define POT_PIN A7     // Potentiometer
#define ENC_PIN1 5
#define ENC_PIN2 6
Encoder wheelEnc(ENC_PIN1, ENC_PIN2);

/* === === === === === RGB LED === === === === === */

#define LED_PIN 2  // NeoPixel strip

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


//IMU Fusion
// Define calibration (replace with actual calibration data)
const FusionMatrix gyroscopeMisalignment = { 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f };
const FusionVector gyroscopeSensitivity = { 1.0f, 1.0f, 1.0f };
const FusionVector gyroscopeOffset = { -0.364562f, -0.792673f, -0.117901f };
const FusionMatrix accelerometerMisalignment = { 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f };
const FusionVector accelerometerSensitivity = { 1.0f, 1.0f, 1.0f };
const FusionVector accelerometerOffset = { -0.016507f, -0.213300f, -0.088512f };
const FusionMatrix softIronMatrix = { 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f };
const FusionVector hardIronOffset = { 0.0f, 0.0f, 0.0f };

const FusionAhrsSettings settings = {
  .gain = 0.2f,
  .accelerationRejection = 10.0f,
  .magneticRejection = 20.0f,
  .rejectionTimeout = 5 * SAMPLE_RATE, /* 5 seconds */
};

// IMU data to Fusion
float ax, ay, az, gx, gy, gz;

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
int8_t positionWheel;

// Button
uint8_t debounce = 40;
bool pullup = true;
bool invert = true;
EasyButton button(BUTTON_PIN, debounce, pullup, invert);

void (*resetFunc)(void) = 0;  // create a standard reset function

// Fusion
FusionOffset offset;
FusionAhrs ahrs;

/*
=== === === === === === === === === === === === === === === === === === === === === === === === === === === === === ===
=== === === === === === === === === === === ===  S E T U P  === === === === === === === === === === === === === === ===
=== === === === === === === === === === === === === === === === === === === === === === === === === === === === === ===
*/
void setup() {

  //  Start Serial and wait for connection
  Serial.begin(115200, SERIAL_8N1);
  delay(100);
  if (!Serial) {
    Serial.print("Waiting for Serial...");
    delay(1000);
  }
  Serial.println(" === === === Seial started === === === ");

  // setupFlash();  // Reads the prefs stored in the flash and loads them.

  /* === === === === Initialize EasyButton === === === === */

  button.begin();

  // EasyButton callbacks
  button.onSequence(1, 300, resetAngle);
  button.onPressedFor(300, changeZoom);

  /* === === === === Initialize IMU === === === === */

  setupIMU();

  FusionOffsetInitialise(&offset, SAMPLE_RATE);
  FusionAhrsInitialise(&ahrs);

  setupFusion();

  // Set AHRS algorithm settings
  FusionAhrsSetSettings(&ahrs, &settings);

  /* === === === === Initialize NeoPixel === === === === */

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

  // while (1) {
  //   delay(100);
  // }
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
    positionWheel = newWheel;
  }

  // Check button status
  button.read();

  /* === === === === IMU === === === === */

  loopIMU();
  loopFusion();

  /* === === === === CyberLevel === === === === */

  // Serial.print("Angle: ");
  // Serial.print(bubbleAngle)
  loopLED();

  /* === === === === Flash === === === === */

  // loopFlash();  // checks for updated prefs and saves them to the flash

  loopPrint();

}