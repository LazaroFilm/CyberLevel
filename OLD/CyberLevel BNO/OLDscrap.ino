// Serial.print(F("\tpixelPos: "));
// Serial.print(pixelPos, 8);
// Serial.print(F("\tfloatPixel: "));
// Serial.print(floatPixel, 8);
// Serial.print(F("\tdecimalOnly: "));
// Serial.print(decimalOnly, 8);

/* Pressing the button to change settings */

// button = digitalRead(BUTTON_PIN);
// if (button == LOW) {
//   if (zoom == 40) {
//     zoom = 5;
//     pixels.clear();
//     pixels.setPixelColor(centerPixel + 1, 128, 0, 255);
//     pixels.setPixelColor(centerPixel - 1, 128, 0, 255);
//     pixels.show();
//     EEPROM.put(eeZoom, 5);
//     delay(500);
//   } else if (zoom == 5) {
//     zoom = 10;
//     pixels.clear();
//     pixels.setPixelColor(centerPixel + 3, 128, 0, 255);
//     pixels.setPixelColor(centerPixel - 3, 128, 0, 255);
//     pixels.show();
//     EEPROM.put(eeZoom, 10);
//     delay(500);
//   } else if (zoom == 10) {
//     zoom = 20;
//     pixels.clear();
//     pixels.setPixelColor(centerPixel + 5, 128, 0, 255);
//     pixels.setPixelColor(centerPixel - 5, 128, 0, 255);
//     pixels.show();
//     EEPROM.put(eeZoom, 20);
//     delay(500);
//   } else if (zoom == 20) {
//     zoom = 40;
//     pixels.clear();
//     pixels.setPixelColor(NUM_PIXELS - 1, 128, 0, 255);
//     pixels.setPixelColor(0, 128, 0, 255);
//     pixels.show();
//     EEPROM.put(eeZoom, 40);
//     delay(500);
//   } else {
//     zoom = 5;
//     pixels.clear();
//     pixels.setPixelColor(centerPixel + 3, 255, 0, 0);
//     pixels.setPixelColor(centerPixel - 3, 255, 0, 0);
//     pixels.show();
//     EEPROM.put(eeZoom, 5);
//     delay(500);
//   }
// }

// //!!! Button presses. Long change axis, short change zoom
// currentState = digitalRead(BUTTON_PIN);
// if (lastState == HIGH && currentState == LOW) // button is pressed
//   pressedTime = millis();
// else if (lastState == LOW && currentState == HIGH) { // button is released
//   releasedTime = millis();

//   long pressDuration = releasedTime - pressedTime;

//   if (pressDuration < SHORT_PRESS_TIME) {
//     Serial.println("A short press is detected");
//   } else if (pressDuration >= SHORT_PRESS_TIME) {
//     Serial.println("A long press is detected");
//   }
// }

// // save the the last state
// lastState = currentState;
// if (digitalRead(BUTTON_PIN) == HIGH) {
//   if (buttonActive == false) {
//     buttonActive = true;
//     buttonTimer = millis();
//   }
//   if ((millis() - buttonTimer > longPressTime) &&
//       (longPressActive == false)) {
//     longPressActive = true;
//     /* change axis on a long press of a button */
//     if (axis < 2) {
//       axis++;
//     } else {
//       axis = 0;
//     }
//     if (axis == 0) {
//       pixels.clear();
//       pixels.setPixelColor(centerPixel - 2, 255, 0, 0);
//       pixels.setPixelColor(centerPixel - 3, 255, 0, 0);
//       pixels.show();
//       delay(500);
//     } else if (axis == 1) {
//       pixels.clear();
//       pixels.setPixelColor(centerPixel + 1, 0, 255, 0);
//       pixels.setPixelColor(centerPixel - 1, 0, 255, 0);
//       pixels.show();
//       delay(500);
//     } else if (axis == 2) {
//       pixels.clear();
//       pixels.setPixelColor(centerPixel + 2, 0, 0, 255);
//       pixels.setPixelColor(centerPixel + 3, 0, 0, 255);
//       pixels.show();
//       delay(500);
//     }
//   }
// } else {
//   if (buttonActive == true) {
//     if (longPressActive == true) {
//       longPressActive = false;
//     } else {
//       if (zoom == 40) {
//         zoom = 5;
//         pixels.clear();
//         pixels.setPixelColor(centerPixel + 1, 128, 0, 255);
//         pixels.setPixelColor(centerPixel - 1, 128, 0, 255);
//         pixels.show();
//         EEPROM.put(eeZoom, 5);
//         delay(500);
//       } else if (zoom == 5) {
//         zoom = 10;
//         pixels.clear();
//         pixels.setPixelColor(centerPixel + 3, 128, 0, 255);
//         pixels.setPixelColor(centerPixel - 3, 128, 0, 255);
//         pixels.show();
//         EEPROM.put(eeZoom, 10);
//         delay(500);
//       } else if (zoom == 10) {
//         zoom = 20;
//         pixels.clear();
//         pixels.setPixelColor(centerPixel + 5, 128, 0, 255);
//         pixels.setPixelColor(centerPixel - 5, 128, 0, 255);
//         pixels.show();
//         EEPROM.put(eeZoom, 20);
//         delay(500);
//       } else if (zoom == 20) {
//         zoom = 40;
//         pixels.clear();
//         pixels.setPixelColor(NUM_PIXELS - 1, 128, 0, 255);
//         pixels.setPixelColor(0, 128, 0, 255);
//         pixels.show();
//         EEPROM.put(eeZoom, 40);
//         delay(500);
//       } else {
//         zoom = 5;
//         pixels.clear();
//         pixels.setPixelColor(centerPixel + 3, 255, 0, 0);
//         pixels.setPixelColor(centerPixel - 3, 255, 0, 0);
//         pixels.show();
//         EEPROM.put(eeZoom, 5);
//         delay(500);
//       }
//     }
//     buttonActive = false;
//   }
// }