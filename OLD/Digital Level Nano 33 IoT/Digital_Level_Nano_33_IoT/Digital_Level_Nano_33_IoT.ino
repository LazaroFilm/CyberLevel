#include "Arduino_LSM6DS3.h"
#include "MadgwickAHRS.h"
#include <FlashAsEEPROM.h>

 
// initialize a Madgwick filter:
Madgwick filter;
// sensor's sample rate is fixed at 104 Hz:
const float sensorRate = 104.00;
 
// values for orientation:
float roll = 0.0;
float pitch = 0.0;
float heading = 0.0;
 
void setup() {
  Serial.begin(9600);
  //reading FlashAsEEPROM
  if (!EEPROM.isValid()) {
    Serial.println("EEPROM is empty");
    EEPROM.write(0, 1);
    EEPROM.commit();
  } else {
    Serial.print(EEPROM.read(0));
  }
  // attempt to start the IMU:
  if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU");
    // stop here if you can't access the IMU:
    while (true);
  }
  // start the filter to run at the sample rate:
  filter.begin(sensorRate);
}
 
void loop() {
  // values for acceleration and rotation:
  float xAcc, yAcc, zAcc;
  float xGyro, yGyro, zGyro;
 
  // check if the IMU is ready to read:
  if (IMU.accelerationAvailable() && IMU.gyroscopeAvailable()) {
    // read accelerometer and gyrometer:
    IMU.readAcceleration(xAcc, yAcc, zAcc);
    IMU.readGyroscope(xGyro, yGyro, zGyro);
   
    // update the filter, which computes orientation:
    filter.updateIMU(xGyro, yGyro, zGyro, xAcc, yAcc, zAcc);
   
    // print the heading, pitch and roll
    roll = filter.getRoll();
    pitch = filter.getPitch();
    heading = filter.getYaw();
   
    // print the filter's results:
    Serial.print(heading);
    Serial.print(",");
    Serial.print(pitch);
    Serial.print(",");
    Serial.println(roll);
  }
}
