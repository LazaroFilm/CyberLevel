/*=== === === === === === === === === === === === === === 
  === === === === ===  FLSAH  === === === === === === ===
  === === === === === === === === === === === === === ===*/


typedef struct flashStruct {
  int flashZoom;
} flashPrefs;

NanoBLEFlashPrefs myFlashPrefs;
flashPrefs prefs;

bool unsavedZoom = false;  // Check if any setting needs to be saved to Flash

void setupFlash() {
  flashPrefs prefsOut;

  // Reads prefs saved in Flash
  int rc = myFlashPrefs.readPrefs(&prefsOut, sizeof(prefsOut));
  if (rc == FDS_SUCCESS) {
    if (prefsOut.flashZoom == 0) {
      zoom = 10;
      Serial.print("Flash Zoom eror: ");
      Serial.println(prefsOut.flashZoom);
    } else {
      zoom = prefsOut.flashZoom;
      Serial.print("Zoom read from Flash: ");
      Serial.println(zoom);
    }
    delay(100);
  } else {
    Serial.print("No preferences found. Return code: ");
    Serial.print(rc);
    Serial.print(", ");
    Serial.println(myFlashPrefs.errorString(rc));
  }
}

void loopFlash() {
  // Flash saving
  if (unsavedZoom) {
    Serial.println("Writing values to Flash");
    myFlashPrefs.writePrefs(&prefs, sizeof(prefs));
    delay(100);
    unsavedZoom = false;
  }
}