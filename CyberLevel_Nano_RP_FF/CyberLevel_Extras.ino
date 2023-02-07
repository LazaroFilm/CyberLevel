
/*=== === === === === === === === === === === === === === === === === === ===
  === === === === === === === ===  Functions  === === === === === === === === 
  === === === === === === === === === === === === === === === === === === ===*/

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
  unsavedZoom = true;
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

void plot(String label, float value, bool last) {
  Serial.print(label);  // May be an empty string.
  if (label != "") Serial.print(":");
  Serial.print(value);
  if (last == false) Serial.print(",");
  else Serial.println();
}

void resetAngle() {
  Serial.println("Resetting angle adjustment");
  // Zero out encoder
  rotaryCount = 0;
  newValue = 0;
  //Resetting Fusion
  FusionAhrsReset(&ahrs);
}

void rgb_led(char color) {  // RGB led on the Nano 33 BLE board
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
