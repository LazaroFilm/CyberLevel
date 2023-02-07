// for Arduino Nano RP2040 Connect https://docs.arduino.cc/hardware/nano-rp2040-connect

// using adafruit/Adafruit_LSM6DS https://github.com/adafruit/Adafruit_LSM6DS

// and xioTechnologies/Fusion https://github.com/xioTechnologies/Fusion


#define ENABLE_SERIAL
#define XIMU
// #define SERIAL_STUDIO

// #include <Sheduler.h>
#include <WiFiNINA.h>
#include <Adafruit_NeoPixel.h>
#include <EasyButton.h>
#include <LSM6DSOXSensor.h>
#include <Fusion.h>
#include "qdec.h"

/* === === === === === IMU/Fusion === === === === === */
// IMU
LSM6DSOXSensor lsm6dsoxSensor = LSM6DSOXSensor(&Wire, LSM6DSOX_I2C_ADD_L);
#define INT_1 4  // Interrupt pin for IMU

// #define SAMPLE_PERIOD (0.01f)  // replace this with actual sample period
#define SAMPLE_RATE (416)  // replace this with actual sample rate
// #define LSM6DSOX_INT1_CTRL = uint8_t int1_drdy_g


/* === === === === === PINS === === === === ===*/
#define LED_PIN 6            // D13 GPIO6 NeoPixel strip
#define BUTTON_PIN 2         // D2 Encodder button
const int ROTARY_PIN_A = 7;  // the first pin connected to the rotary encoder
const int ROTARY_PIN_B = 9;  // the second pin connected to the rotary encoder

/* === === === === === Encoder === === === === ===*/
::SimpleHacks::QDecoder qdec(ROTARY_PIN_A, ROTARY_PIN_B, false);  // the library class...
volatile int rotaryCount = 0;
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

Adafruit_NeoPixel pixels(NUM_PIXELS, LED_PIN, NEO_GRB + NEO_KHZ800);

/* === === === === === VALUES === === === === === */

/* constant values */
const uint8_t centerPixel = (NUM_PIXELS - 1) / 2;  // Center of the LED strip


//IMU Fusion
unsigned long timestamp = micros();  // replace this with actual gyroscope timestamp

// IMU data to Fusion
float ax, ay, az, gx, gy, gz;
float cax, cay, caz, cgx, cgy, cgz;
float quatw, quatx, quaty, quatz;
float yaw, pitch, roll;
int fusionTime;

unsigned long lastEventTime, eventTime = micros();  // interruptCounter = 1;
long count = 0;
unsigned long loopTime;
float freq;
bool missingGData = false;

// CyberLevel
bool splash = true;  // is it displaying plash?
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

#ifdef ENABLE_SERIAL
  //Start Serial and wait for connection
  Serial.begin(115200, SERIAL_8N1);
  delay(100);
  if (!Serial) {
    Serial.print("Waiting for Serial...");
    delay(1000);
  }
  Serial.println(" === === === Seial started === === === ");
#endif

  // Default clock is 100kHz. LSM6DSOX also supports 400kHz, let's use it
  Wire.begin();
  Wire.setClock(400000);

  // setupFlash();  // Reads the prefs stored in the flash and loads them.

  setupEncoder();



  // while (1) {
  //   delay(100);
  // }

  // Scheduler.startLoop(loop1);
}

void setup1() {
  setupIMU();
  setupFusion();
}

/*
=== === === === === === === === === === === === === === === === === === === === === === === === === === === === === ===
=== === === === === === === === === === === === L O O P === === === === === === === === === === === === === === === ===
=== === === === === === === === === === === === === === === === === === === === === === === === === === === === === ===
*/

// void loop1() {

// }
void loop() {

  // Encoder checks if it was turned
  loopEncoder();

  // Check button status
  button.read();

  /* === === === === IMU === === === === */

  loopIMU();
  loopFusion();

  /* === === === === CyberLevel === === === === */
  if (false) {
    rainbowCycle();
  } else {
    pixels.setBrightness(255);
    loopLevel();
  }
  // Serial.print(" boop ");

  /* === === === === Flash === === === === */

  // loopFlash();  // checks for updated prefs and saves them to the flash

#ifdef ENABLE_SERIAL
// loopPrint();
#ifdef XIMU
  printXIMU();
#endif
#ifdef SERIAL_STUDIO
  printSerialStudio();
#endif
#endif
  eventTime = micros();
  loopTime = eventTime - lastEventTime;
  lastEventTime = eventTime;
  freq = 1000000.0f / loopTime;
}
