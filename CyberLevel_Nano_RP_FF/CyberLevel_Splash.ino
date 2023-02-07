uint16_t i = 0;
uint16_t j = 0;

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle() {
  pixels.setBrightness(25);
  if (j < 256 * 5) {  // 5 cycles of all colors on wheel
    for (i; i < pixels.numPixels(); i++) {
      pixels.setPixelColor(i, Wheel(((i * 256 / pixels.numPixels()) + j) & 255));
    }
    Serial.println(j);
    pixels.show();
    // delay(10);
  } else {
    splash = false;
  }
  // pixels.show();
  j++;
}


// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if (WheelPos < 85) {
    return pixels.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if (WheelPos < 170) {
    WheelPos -= 85;
    return pixels.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return pixels.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}
