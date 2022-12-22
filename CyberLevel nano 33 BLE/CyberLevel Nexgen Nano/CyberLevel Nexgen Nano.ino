#include <EasyButton.h>
#include <Wire.h>
#include <Adafruit_NeoPixel.h>
#include <NexgenAHRS.h>


/* IMU */

LSM9DS1 imu;
EulerAngles angles;

int loopFrequency = 0;
const long displayPeriod = 0;
unsigned long previousMillis = 0;

/* Pixel + buttons */

// Assigning pins
#define LED_PIN D2    // NeoPixel strip
#define BUTTON_PIN 5  // Button switch
#define POT_PIN 18    // Potentiometer

// How many NeoPixels are attached to the Arduino?
#define NUM_PIXELS 16

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
uint8_t zoom = 20;        // zoom factor of the bubble
uint8_t axis = 1;        // axis displayed
  // pot smoothing
float readings[numReadings];  // the readings from the analog input
int readIndex = 0;            // the index of the current reading
float total = 0;              // the running total
float Pot = 0;                // the pot average

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

/* ---------------------------------------------------------------------------------------------------- */

void setup() {

  /* initializing EasyButton */
  button.begin();

  // Initialise the LSM9DS1 IMU
  imu.begin();

  /* INITIALIZE NeoPixel strip object (REQUIRED) */
  pixels.begin();
  pixels.clear();
  //dim white center pixel
  pixels.setPixelColor(centerPixel, pixels.Color(10, 10, 10));
  pixels.show();

  //  Positive magnetic declination - Sydney, AUSTRALIA
  imu.setDeclination(12.717);
  imu.setFusionAlgorithm(SensorFusion::FUSION);
  imu.setFusionPeriod(0.01f);   // Estimated sample period = 0.01 s = 100 Hz
  imu.setFusionThreshold(0.2f);  // Stationary threshold = 0.5 degrees per second
  imu.setFusionGain(0.5);          // Default Fusion Filter Gain - try 7.5 for a much quicker response

  //  Start Serial and wait for connection
  Serial.begin(115200);
  while (!Serial)
    ;

  if (imu.connected()) {
    Serial.println("LSM9DS1 IMU Connected.");
    imu.start();
  } else {
    Serial.println("LSM9DS1 IMU Not Detected.");
    while (1)
      ;
  }
}

void loop() {
  //  Check for new IMU data and update angles
  angles = imu.update();

  //  Wait for new sample - 7 ms delay provides a 100Hz sample rate / loop frequency
  delay(7);

  //  Display sensor data every displayPeriod, non-blocking.
  // if (millis() - previousMillis >= displayPeriod) {

  //   Serial.print("Roll: ");
  //   Serial.print(angles.roll);
  //   Serial.print("\tPitch: ");
  //   Serial.print(angles.pitch);
  //   Serial.print("\tYaw: ");
  //   Serial.print(angles.yaw);
  //   Serial.print("\tHeading: ");
  //   Serial.print(angles.heading);
  //   Serial.print("\tLoop Frequency: ");
  //   Serial.print(loopFrequency);
  //   Serial.print(" Hz");
  //   Serial.println("");

  //   loopFrequency = 0;
  //   previousMillis = millis();
  // }
  // loopFrequency++;


  /* ---------- above from Nexgen, below from cyberlevel ---------- */
  angle = angles.pitch;

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

  // angleOffset = (Pot / (1023 / 2) * 10) - 10;  // offset bubble with pot
  angleOffset = 0;  //! Disables offset !

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

  Serial.print(F("\tangle: "));
  Serial.print(angle, 8);

  Serial.print("\tpixelPos: ");
  Serial.print(pixelPos);
  // Serial.print("\tangleOffset: ");
  // Serial.print(angleOffset);
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
}