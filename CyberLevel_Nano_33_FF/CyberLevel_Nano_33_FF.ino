// using FemmeVerbeek/Arduino_LSM9DS1 https://github.com/FemmeVerbeek/Arduino_LSM9DS1
// and xioTechnologies/Fusion https://github.com/xioTechnologies/Fusion

<<<<<<< HEAD:CyberLevel_Nano_RP_FF/CyberLevel_Nano_RP_FF.ino
// #define ARDUINO_NANO_RP2040_CONNECT
// #define ENCODER_USE_INTERRUPTS
// #define ENCODER_PULLUP
=======
#define ARDUINO_ARCH_NRF52840
>>>>>>> 8ec4699e8cadc6ddf4270507bd4b5601b78b97e6:CyberLevel_Nano_33_FF/CyberLevel_Nano_33_FF.ino
// #define ENCODER_OPTIMIZE_INTERRUPTS
// #include <Wire.h>
// #define INPUT_PULLUP 0x2
#include <WiFiNINA.h>
#include <Adafruit_NeoPixel.h>
#include <EasyButton.h>
// #include <Encoder.h>
// #include <NanoBLEFlashPrefs.h>
//IMU/Fusion
<<<<<<< HEAD:CyberLevel_Nano_RP_FF/CyberLevel_Nano_RP_FF.ino
#include <Adafruit_LSM6DSOX.h>
// #include <Arduino_LSM9DS1.h>  //Femme's version
#include <Fusion.h>
#include "qdec.h"
=======
#include <Arduino_LSM9DS1.h>  //Femme's version
#include "Fusion.h"
>>>>>>> 8ec4699e8cadc6ddf4270507bd4b5601b78b97e6:CyberLevel_Nano_33_FF/CyberLevel_Nano_33_FF.ino

/* === === === === === IMU/Fusion === === === === === */
// IMU
// #define SAMPLE_PERIOD (0.01f)  // replace this with actual sample period
<<<<<<< HEAD:CyberLevel_Nano_RP_FF/CyberLevel_Nano_RP_FF.ino
#define SAMPLE_RATE (104)  // replace this with actual sample rate
=======
#define SAMPLE_RATE (476)  // replace this with actual sample rate
>>>>>>> 8ec4699e8cadc6ddf4270507bd4b5601b78b97e6:CyberLevel_Nano_33_FF/CyberLevel_Nano_33_FF.ino


/* === === === === === PINS === === === === ===*/
#define LED_PIN 6     // D13 GPIO6 NeoPixel strip
#define BUTTON_PIN 2  // D2 Encodder button
// #define ENC_PIN1 7    // D8 Encoder pinsx
// #define ENC_PIN2 9    // D9 Encoder pins
const int ROTARY_PIN_A = 7;  // the first pin connected to the rotary encoder
const int ROTARY_PIN_B = 9;  // the second pin connected to the rotary encoder

/* === === === === === Encoder === === === === ===*/
// Encoder knob(ENC_PIN1, ENC_PIN2);
::SimpleHacks::QDecoder qdec(ROTARY_PIN_A, ROTARY_PIN_B, false);  // the library class...
// Stores a relative value based on the clockwise / counterclockwise events
volatile int rotaryCount = 0;
// Used in loop() function to track the value of rotaryCount from the
// prior time that loop() executed (to detect changes in the value)
int lastLoopDisplayedRotaryCount = 0;

// How many NeoPixels are attached to the Arduino?
#define NUM_PIXELS 30

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
const FusionVector gyroscopeOffset = { 0.000020f, 0.000153f, -0.000067f };
// const FusionVector gyroscopeOffset = FUSION_VECTOR_ZERO;
const FusionMatrix accelerometerMisalignment = { 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f };
const FusionVector accelerometerSensitivity = { 0.1f, 0.1f, 0.1f };
const FusionVector accelerometerOffset = { -0.009338f, 0.003083f, -0.006804f };
// const FusionVector accelerometerOffset = FUSION_VECTOR_ZERO;
const FusionMatrix softIronMatrix = { 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f };
const FusionVector hardIronOffset = { 0.0f, 0.0f, 0.0f };

const FusionAhrsSettings settings = {
<<<<<<< HEAD:CyberLevel_Nano_RP_FF/CyberLevel_Nano_RP_FF.ino
  .gain = 0.1f,
=======
  .gain = 0.25f,
>>>>>>> 8ec4699e8cadc6ddf4270507bd4b5601b78b97e6:CyberLevel_Nano_33_FF/CyberLevel_Nano_33_FF.ino
  .accelerationRejection = 10.0f,
  .magneticRejection = 20.0f,
  .rejectionTimeout = 5 * SAMPLE_RATE, /* 5 seconds */
};

// IMU data to Fusion
<<<<<<< HEAD:CyberLevel_Nano_RP_FF/CyberLevel_Nano_RP_FF.ino
float ax, ay, az, gx, gy, gz;
float cax, cay, caz, cgx, cgy, cgz;
float quatw, quatx, quaty, quatz;
int fusionTime;
=======
float ax, ay, az, gx, gy, gz, mx, my, mz;
>>>>>>> 8ec4699e8cadc6ddf4270507bd4b5601b78b97e6:CyberLevel_Nano_33_FF/CyberLevel_Nano_33_FF.ino

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
// int8_t oldKnob;
int newValue;
bool unsavedZoom = false;  // Check if any setting needs to be saved to Flash

// Button
uint8_t buttonDebounce = 40;
bool buttonPullup = true;
bool buttonInvert = true;
EasyButton button(BUTTON_PIN, buttonDebounce, buttonPullup, buttonInvert);

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

  // initialize the rotary encoder
  qdec.begin();
  // attach an interrupts to each pin, firing on any change in the pin state
  // no more polling for state required!
  attachInterrupt(digitalPinToInterrupt(ROTARY_PIN_A), IsrForQDEC, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ROTARY_PIN_B), IsrForQDEC, CHANGE);
  pinMode(ROTARY_PIN_A, INPUT_PULLUP);
  pinMode(ROTARY_PIN_B, INPUT_PULLUP);

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
  // long newKnob = knob.read();
  // if (newKnob != oldKnob) {
  //   oldKnob = newKnob;
  //   Serial.println("Knob moved!");
  // }

  newValue = rotaryCount;
  if (newValue != lastLoopDisplayedRotaryCount) {
    // Also get the difference since the last loop() execution
    int difference = newValue - lastLoopDisplayedRotaryCount;

    // act on the change: e.g., modify PWM to be faster/slower, etc.
    lastLoopDisplayedRotaryCount = newValue;
    Serial.print("Change: ");
    Serial.print(newValue);
    Serial.print(" (");
    Serial.print(difference);
    Serial.println(")");
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

<<<<<<< HEAD:CyberLevel_Nano_RP_FF/CyberLevel_Nano_RP_FF.ino
  // loopFlash();  // checks for updated prefs and saves them to the flash

  // loopPrint();
  // printXIMU();
=======
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

  // Serial.print(F("\tAngle: "));
  // Serial.print(Angle, 8);

  // plot("Ax", ax * 1000, false);
  // plot("Ay", ay * 1000, false);
  // plot("Az", az * 1000, false);

  // plot("Gx", gx, false);
  // plot("Gy", gy, false);
  // plot("Gz", gz, false);


  // plot("Angle", angle, false);

  // Serial.print("\tpixelPos: ");
  // Serial.print(pixelPos);
  // Serial.print("\tangleOffset: ");
  // Serial.print(angleOffset);
  // plot("Raw_Pot", rawValue, false);
  // plot("Smoothed_Pot", Pot, false);
  // plot("Encoder", positionWheel, false);
  // plot("Angle_Offset", angleOffset, false);
  plot("Pixel", (pixelPos), false);
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
  Serial.println("Min:-90,Max:90");
  // Serial.println("Min:-1025,Max:1025");
>>>>>>> 8ec4699e8cadc6ddf4270507bd4b5601b78b97e6:CyberLevel_Nano_33_FF/CyberLevel_Nano_33_FF.ino
}