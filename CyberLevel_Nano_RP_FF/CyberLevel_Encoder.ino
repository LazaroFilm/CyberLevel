
// This is the ISR (interrupt service routine).
// This ISR fires when the pins connected to the rotary encoder change
// state.  Therefore, the update() function is called to update the
// decoder state and then (if an event was indicated), the rotaryCount
// variable is updated.
// CAUTION: some Arduion functions will not work if called from an ISR.
void IsrForQDEC(void) {  // do absolute minimum possible in any ISR ...
  // placing the `using namespace` line into a function limits
  // allows accessing the types and functions of that namespace,
  // but only while in this function.
  using namespace ::SimpleHacks;
  QDECODER_EVENT event = qdec.update();
  if (event & QDECODER_EVENT_CW) {
    rotaryCount = rotaryCount + 1;
  } else if (event & QDECODER_EVENT_CCW) {
    rotaryCount = rotaryCount - 1;
  }
  return;
}

void encoderLoop() {
  newValue = rotaryCount;
  if (newValue != lastLoopDisplayedRotaryCount) {
    // Also get the difference since the last loop() execution
    int difference = newValue - lastLoopDisplayedRotaryCount;

    // act on the change: e.g., modify PWM to be faster/slower, etc.
    lastLoopDisplayedRotaryCount = newValue;
    Serial.print("Change: ");
    Serial.print(newValue);
    Serial.print(" (");
    Serial.print(difference);
    Serial.println(")");
  }
}