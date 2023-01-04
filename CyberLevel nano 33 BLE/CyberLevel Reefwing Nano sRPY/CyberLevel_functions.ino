
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

void plot(String label, int value, bool last) {
  Serial.print(label);  // May be an empty string.
  if (label != "") Serial.print(":");
  Serial.print(value);
  if (last == false) Serial.print(",");
  else Serial.println();
}

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
}