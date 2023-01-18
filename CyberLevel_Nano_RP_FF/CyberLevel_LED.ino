void loopLED() {
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
}