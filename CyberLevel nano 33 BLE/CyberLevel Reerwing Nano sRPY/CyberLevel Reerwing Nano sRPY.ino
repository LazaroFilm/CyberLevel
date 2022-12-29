#include <EasyButton.h>
#include <Wire.h>
#include <Adafruit_NeoPixel.h>
#include <ReefwingAHRS.h>
// #include <NexgenAHRS.h>

/* === === === === === IMU === === === === === */

LSM9DS1 imu;
EulerAngles angles;

// int loopFrequency = 0;
// const long displayPeriod = 0;
// unsigned long previousMillis = 0;

/* === === === === === pixel / buttons / pot === === === === ===*/

#define BUTTON_PIN D3  // Button switch
#define POT_PIN A7     // Potentiometer

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
const int numReadings = 12;

/* variable values */
double angle;        // Roll angle in degree
double decimalOnly;  // only the decimal of the pixel number
double floatPixel;   // Pixel numer
double pixelPos;     // position of the pixel on the strip
float pixelColor;    // color value of the pixel
float angleOffset;   // offset from potentiometer
int8_t intPixel;     // pixel number without decimal
uint8_t zoom = 20;   // zoom factor of the bubble
uint8_t axis = 1;    // axis displayed
uint32_t previousMillis = 0;
uint32_t Millis = 0;
// pot smoothing
float readings[numReadings];  // the readings from the analog input
int readIndex = 0;            // the index of the current reading
float total = 0;              // the running total
float Pot = 0;                // the pot average
// Led colors
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
// button
uint8_t debounce = 40;
bool pullup = true;
bool invert = true;
EasyButton button(BUTTON_PIN, debounce, pullup, invert);

void (*resetFunc)(void) = 0;  // create a standard reset function

/*=== === === === === === === === === === === === === === === === === === === === === === === === ===
=== === === === === === === === === === === === === === === === === === === === === === === === === */

/* _________ BUTTON ACTIONS _________ */

void changeZoom() {
  Serial.println("Button is clicked");
  /* change zoom on a short press of a button */
  if (zoom == 40) {
    zoom = 5;
    pixels.clear();
    pixels.setPixelColor(centerPixel + 1, 128, 0, 255);
    pixels.setPixelColor(centerPixel - 1, 128, 0, 255);
    pixels.show();
    // EEPROM.update(eeZoom, 5);
    delay(500);
  } else if (zoom == 5) {
    zoom = 10;
    pixels.clear();
    pixels.setPixelColor(centerPixel + 3, 128, 0, 255);
    pixels.setPixelColor(centerPixel - 3, 128, 0, 255);
    pixels.show();
    // EEPROM.update(eeZoom, 10);
    delay(500);
  } else if (zoom == 10) {
    zoom = 20;
    pixels.clear();
    pixels.setPixelColor(centerPixel + 5, 128, 0, 255);
    pixels.setPixelColor(centerPixel - 5, 128, 0, 255);
    pixels.show();
    // EEPROM.update(eeZoom, 20);
    delay(500);
  } else if (zoom == 20) {
    zoom = 40;
    pixels.clear();
    pixels.setPixelColor(NUM_PIXELS - 1, 128, 0, 255);
    pixels.setPixelColor(0, 128, 0, 255);
    pixels.show();
    // EEPROM.update(eeZoom, 40);
    delay(500);
  } else {
    zoom = 10;
    pixels.clear();
    pixels.setPixelColor(centerPixel + 3, 255, 0, 0);
    pixels.setPixelColor(centerPixel - 3, 255, 0, 0);
    pixels.show();
    // EEPROM.update(eeZoom, 5);
    delay(500);
  }
  // EEPROM.put(eeZoom, zoom);
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
  // EEPROM.put(eeAxis, axis);
  Serial.print("\n\nAxis changed to: ");
  Serial.println(axis);
}

// Callback function to be called when the button is pressed.
// void onPressed() {
//   Serial.println("Button pressed");
// }

void rgb_led(char color) {
  if (color == 'o') {
    digitalWrite(LEDR, HIGH);
    digitalWrite(LEDG, HIGH);
    digitalWrite(LEDB, HIGH);
  } else if (color == 'i') {
    digitalWrite(LEDR, LOW);
    digitalWrite(LEDG, LOW);
    digitalWrite(LEDB, LOW);
  } else if (color == 'r') {
    digitalWrite(LEDR, LOW);
    digitalWrite(LEDG, HIGH);
    digitalWrite(LEDB, HIGH);
  } else if (color == 'g') {
    digitalWrite(LEDR, HIGH);
    digitalWrite(LEDG, LOW);
    digitalWrite(LEDB, HIGH);
  } else if (color == 'b') {
    digitalWrite(LEDR, HIGH);
    digitalWrite(LEDG, HIGH);
    digitalWrite(LEDB, LOW);
  } else if (color == 'y') {
    digitalWrite(LEDR, LOW);
    digitalWrite(LEDG, LOW);
    digitalWrite(LEDB, HIGH);
  } else if (color == 'p') {
    digitalWrite(LEDR, LOW);
    digitalWrite(LEDG, HIGH);
    digitalWrite(LEDB, LOW);
  } else if (color == 'm') {
    digitalWrite(LEDR, HIGH);
    digitalWrite(LEDG, LOW);
    digitalWrite(LEDB, LOW);
  } else {
    digitalWrite(LEDR, HIGH);
    digitalWrite(LEDG, HIGH);
    digitalWrite(LEDB, HIGH);
  };
}

void pixelBubble() {
  if (intPixel >= NUM_PIXELS) {  // out of bounds make last pixel on.
    rgb_led('r');
    pixels.setPixelColor(centerPixel, rgbcolorC);
    pixels.setPixelColor(centerPixel - 1, rgbcolorS);
    pixels.setPixelColor(centerPixel + 1, rgbcolorS);
    pixels.setPixelColor(NUM_PIXELS - 1, 255, 0, 0);
  } else if (intPixel <= 0) {  // out of bounds make first pixel on.
    rgb_led('r');
    pixels.setPixelColor(centerPixel, rgbcolorC);
    pixels.setPixelColor(centerPixel - 1, rgbcolorS);
    pixels.setPixelColor(centerPixel + 1, rgbcolorS);
    pixels.setPixelColor(0, 255, 0, 0);
  } else if (intPixel == centerPixel - 2) {  // on the edge
    rgb_led('g');
    pixels.setPixelColor(intPixel, rgbcolor1);
    pixels.setPixelColor(intPixel - 1, rgbcolor2);
    pixels.setPixelColor(centerPixel - 1, rgbcolorS0);
    pixels.setPixelColor(centerPixel, rgbcolorC);
    pixels.setPixelColor(centerPixel + 1, rgbcolorS);
  } else if (intPixel == centerPixel - 1) {  // side center
    rgb_led('b');
    pixels.setPixelColor(intPixel - 1, rgbcolor2);
    pixels.setPixelColor(centerPixel - 1, rgbcolorS1);
    pixels.setPixelColor(centerPixel, rgbcolorC0);
    pixels.setPixelColor(centerPixel + 1, rgbcolorS);
  } else if (intPixel == centerPixel) {  // dead center
    rgb_led('p');
    pixels.setPixelColor(centerPixel - 1, rgbcolorS2);
    pixels.setPixelColor(centerPixel, rgbcolorC1);
    pixels.setPixelColor(centerPixel + 1, rgbcolorS0);
  } else if (intPixel == centerPixel + 1) {  // side center
    rgb_led('b');
    pixels.setPixelColor(intPixel + 1, rgbcolor0);
    pixels.setPixelColor(centerPixel - 1, rgbcolorS);
    pixels.setPixelColor(centerPixel, rgbcolorC2);
    pixels.setPixelColor(centerPixel + 1, rgbcolorS1);
  } else if (intPixel == centerPixel + 2) {  // on the edge
    rgb_led('g');
    pixels.setPixelColor(intPixel, rgbcolor1);
    pixels.setPixelColor(intPixel + 1, rgbcolor0);
    pixels.setPixelColor(centerPixel - 1, rgbcolorS);
    pixels.setPixelColor(centerPixel, rgbcolorC);
    pixels.setPixelColor(centerPixel + 1, rgbcolorS2);
  } else {
    rgb_led('y');
    pixels.setPixelColor(intPixel + 1, rgbcolor0);
    pixels.setPixelColor(intPixel, rgbcolor1);
    pixels.setPixelColor(intPixel - 1, rgbcolor2);
    pixels.setPixelColor(centerPixel - 1, rgbcolorS);
    pixels.setPixelColor(centerPixel, rgbcolorC);
    pixels.setPixelColor(centerPixel + 1, rgbcolorS);
  }
}

/* 
=== === === === === === === === === === === === === === === === === === === === === === === === === === === === === === 
=== === === === === === === === === === === ===  S E T U P  === === === === === === === === === === === === === === ===
=== === === === === === === === === === === === === === === === === === === === === === === === === === === === === ===
*/
void setup() {
  //  Start Serial and wait for connection
  Serial.begin(115200);
  // delay(1000);
  // if (!Serial) {
  //   Serial.print("Waiting for Serial...");
  //   delay(1000);
  // }
  Serial.println(" === === === Seial started === === === ");

  // initialize EasyButton
  button.begin();
  // Initialize the LSM9DS1 IMU
  imu.begin();
  // Initialize NeoPixel strip
  pixels.begin();
  // Set RGB LED pins
  pinMode(LEDR, OUTPUT);
  pinMode(LEDG, OUTPUT);
  pinMode(LEDB, OUTPUT);
  // Turn RGB LEDs on
  rgb_led('i');

  //dim white center pixel
  pixels.clear();
  pixels.setPixelColor(centerPixel, pixels.Color(10, 10, 10));
  pixels.show();

  //  magnetic declination (neg: northern emi, pos: southern emi)
  imu.setDeclination(-12.79);
  imu.setGyroResolution(Gscale::GFS_2000DPS); // setting the Gyro scale: GFS_245DPS | GFS_500DPS | GFS_2000DPS
  imu.setFusionAlgorithm(SensorFusion::FUSION);  // Sensor furion type: MADGWICK | MAHONY | COMPLEMENTARY | FUSION | CLASSIC | NONE
  //  Fusion settings
  imu.setFusionPeriod(0.1f);     // Estimated sample period = 0.01 s = 100 Hz
  imu.setFusionThreshold(2.0f);  // Stationary threshold = 0.5 degrees per second
  imu.setFusionGain(0.4);        // Default Fusion Filter Gain 0.5 - try 7.5 for a much quicker response


  if (!imu.connected()) {
    Serial.println("LSM9DS1 IMU Not Connected.");
    while (!imu.connected())
      ;
  }

  Serial.println("LSM9DS1 IMU Connected.");
  //  Paste your calibration bias offset HERE
  //  This information comes from the testAndCalibrate.ino
  //  sketch in the library examples sub-directory.
  imu.loadAccBias(-0.002869, -0.027893, 0.018921);
  imu.loadGyroBias(0.321503, -0.478516, 0.463562);
  imu.loadMagBias(0.149658, 0.249390, -0.256348);
  //  This sketch assumes that the LSM9DS1 is already calibrated,
  //  If so, start processing IMU data. If not, run the testAndCalibrate
  //  sketch first.
  imu.start();

  // /* EasyButton callbacks*/
  button.onSequence(1, 300, changeZoom);
  button.onPressedFor(300, changeAxis);

  Serial.print("Gyro Resolution: ");
  Serial.print(imu.getGyroResolution());
}

/* 
=== === === === === === === === === === === === === === === === === === === === === === === === === === === === === === 
=== === === === === === === === === === === === L O O P === === === === === === === === === === === === === === === ===
=== === === === === === === === === === === === === === === === === === === === === === === === === === === === === ===
*/
void loop() {
  Millis == millis();

  // Check button status
  button.read();

  // Check for new IMU data and update angles
  angles = imu.update();

  // Wait for new sample - 7 ms delay provides a 100Hz sample rate / loop frequency
  delay(7);

  /* ---------- above from Reefwing, below from cyberlevel ---------- */
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
  // send it to the computer as ASCII digit

  angleOffset = (Pot / (1023 / 2) * 10) - 10;  // offset bubble with pot
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

  pixelBubble();  // lights up the pixels to make the bubble.

  pixels.show();  // displays the pixels


  /* === === === === === SERIAL PRINTS === === === === === */


  // Serial.print("Loop Time: ");
  // Serial.println(Millis - previousMillis, 8);
  // Serial.println("Millis: ");
  // Serial.print(Millis);
  previousMillis = Millis;

  //  Uncomment to DEBUG raw sensor data:
  SensorData data = imu.rawData();
  //  Serial.print("ax = "); Serial.print(1000*data.ax);
  Serial.print("ay = ");
  Serial.print(1000 * data.ay);
  Serial.print("\t");
  //  Serial.print("\taz = "); Serial.print(1000*data.az); Serial.print(" mg");
  //  Serial.print("\tgx = "); Serial.print( data.gx, 2);
  Serial.print("gy = ");
  Serial.println(data.gy, 2);
  //  Serial.print("\tgz = "); Serial.print( data.gz, 2); Serial.print(" deg/s\t");
  //  Serial.print("\tmx = "); Serial.print(1000*data.mx );
  //  Serial.print("\tmy = "); Serial.print(1000*data.my );
  //  Serial.print("\tmz = "); Serial.print(1000*data.mz ); Serial.println(" mG");

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
}