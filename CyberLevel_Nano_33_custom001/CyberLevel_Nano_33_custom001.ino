// using ErniW / Arduino-IMU-and-AHRS-tests

#define ARDUINO_ARCH_NRF52840
// #define ENCODER_OPTIMIZE_INTERRUPTS

#include <Wire.h>
#include <Arduino_LSM9DS1.h>
#include <Adafruit_AHRS.h>
#include <Adafruit_NeoPixel.h>
#include <NanoBLEFlashPrefs.h>
#include <EasyButton.h>
#include <Encoder.h>
// #include <CyberLevel_functions.ino>

/* === === === === === Flash === === === === === */

typedef struct flashStruct {
  int flashZoom;
} flashPrefs;

NanoBLEFlashPrefs myFlashPrefs;
flashPrefs prefs;

/* === === === === === IMU === === === === === */

Adafruit_Madgwick filter;

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

// IMU Constants
int updateRate = 200;
int printEveryUpdate = 5;
float mag_hardIron[3] = { 49.29, -34.35, -58.99 };
float mag_softIron[9] = {
  0.995, 0.025, 0.003,
  0.025, 0.989, -0.006,
  0.003, -0.006, 1.017
};
float accel_calibration[3] = { -0.01, 0.06, 0.01 };
float gyro_calibration[3] = { -0.55, 0.52, 1.15 };


/* variable values */
// IMU
unsigned long timestamp = 0;
int counter = 0;
float ax, ay, az, gx, gy, gz, mx, my, mz = 0;

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

// Flash
bool unsavedZoom = false;  // Check if any setting needs to be saved to Flash

/*
=== === === === === === === === === === === === === === === === === === === === === === === === === === === === === ===
=== === === === === === === === === === === ===  S E T U P  === === === === === === === === === === === === === === ===
=== === === === === === === === === === === === === === === === === === === === === === === === === === === === === ===
*/
void setup() {
  // flash
  flashPrefs prefsOut;

  //  Start Serial and wait for connection
  Serial.begin(115200, SERIAL_8N1);
  delay(1000);
  if (!Serial) {
    Serial.print("Waiting for Serial...");
    delay(1000);
  }
  Serial.println(" === === === Seial started === === === ");

  // Reads prefs saved in Flash
  int rc = myFlashPrefs.readPrefs(&prefsOut, sizeof(prefsOut));
  if (rc == FDS_SUCCESS) {
    zoom = prefsOut.flashZoom;
    Serial.print("Zoom read from Flash: ");
    Serial.print(prefsOut.flashZoom);
  } else {
    Serial.print("No preferences found. Return code: ");
    Serial.print(rc);
    Serial.print(", ");
    Serial.println(myFlashPrefs.errorString(rc));
  }

  // initialize EasyButton
  button.begin();

  // Initialize the LSM9DS1 IMU
  if (!IMU.begin()) {
    Serial.println("LSM9DS1 initialization error");
    while (1)
      ;
  }
  filter.begin(updateRate);

  Wire1.begin();  //?? WHAT ARE THESE??
  Wire1.beginTransmission(0x1e);
  Wire1.write(0x20);
  Wire1.write(0x9e);
  Wire1.endTransmission();

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

  //if(millis() - timestamp >= 1000 / updateRate){

  if (IMU.accelerationAvailable()) IMU.readAcceleration(ax, ay, az);
  if (IMU.gyroscopeAvailable()) IMU.readGyroscope(gx, gy, gz);
  if (IMU.magneticFieldAvailable()) IMU.readMagneticField(mx, my, mz);

  calibrate(&ax, &ay, &az, &gx, &gy, &gz, &mx, &my, &mz);

  //filter.update(gx, gy, gz, ax, ay, az, mx, my, mz);
  filter.update(-gy, -gx, gz, -ay, -ax, az, mx, my, mz);

  timestamp = millis();

  // counter++;

  // if (counter == printEveryUpdate) {

  //   float w, x, y, z;
  //   // filter.getQuaternion(&w, &x, &y, &z);

  //   // Serial.print("Quaternion: ");
  //   // Serial.print(w);
  //   // Serial.print(',');
  //   // Serial.print(x);
  //   // Serial.print(',');
  //   // Serial.print(y);
  //   // Serial.print(',');
  //   // Serial.println(z);

  //   float roll = filter.getRoll();
  //   float pitch = filter.getPitch();
  //   float yaw = filter.getYaw();

  //   Serial.print("Orientation: ");
  //   Serial.print(yaw);
  //   Serial.print(',');
  //   Serial.print(pitch);
  //   Serial.print(',');
  //   Serial.println(roll);

  //   counter = 0;
  // }
  //}


  /* ---------- above from Reefwing, below from cyberlevel ---------- */
  angle = filter.getPitch();

  // angleOffset = (Pot / (1023 / 2) * 10) - 10;  // offset bubble with pot
  angleOffset = positionWheel * 0.1;  // Enc
  // angleOffset = 0;  //! Disables offset !

  /* angle to pixel with offset and zoom factored in */
  pixelPos = ((angle + angleOffset) / zoom) * NUM_PIXELS;

  floatPixel = pixelPos + centerPixel + 1;
  intPixel = floatPixel;
  decimalOnly = floatPixel - intPixel;

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

  pixelBubble();  // lights up the pixels to make the bubble.

  pixels.show();  // displays the pixels

  /* Flash saving */
  if (unsavedZoom) {
    Serial.println("Writing values to Flash");
    myFlashPrefs.writePrefs(&prefs, sizeof(prefs));
    unsavedZoom = false;
  }

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