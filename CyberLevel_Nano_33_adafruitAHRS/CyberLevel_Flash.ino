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
    zoom = prefsOut.flashZoom;
    Serial.print("Zoom read from Flash: ");
    Serial.print(prefsOut.flashZoom);
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
    unsavedZoom = false;
  }
}