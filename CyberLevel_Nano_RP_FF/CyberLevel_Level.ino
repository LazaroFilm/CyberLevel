void setupLevel() {
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
  pixels.setBrightness(255);
  pixels.show();
}

void loopLevel() {
  // angleOffset = (Pot / (1023 / 2) * 10) - 10;  // offset bubble with pot
  angleOffset = newValue * 0.1;  // Enc
  // angleOffset = 0;  //! Disables offset !

  /* angle to pixel with offset and zoom factored in */
  if (angle <= 90 && angle >= -90) {
    pixelPos = ((angle + angleOffset) / zoom) * NUM_PIXELS;
  } else if (angle > 90) {
    pixelPos = NUM_PIXELS - (((angle - 180 + angleOffset) / zoom) * NUM_PIXELS);
  } else if (angle < 90) {
    pixelPos = NUM_PIXELS - (((angle + 180 + angleOffset) / zoom) * NUM_PIXELS);
  }

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

  // lights up the pixels to make the bubble.
  if (intPixel >= NUM_PIXELS - 1) {  // out of bounds make last pixel on.
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

  if (missingGData) {
    pixels.setPixelColor(0, pixels.Color(255, 0, 0));
    missingGData = false;
  }

  pixels.show();  // displays the pixels
}