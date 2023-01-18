/******************************************************************
  @file       serialRollPitchYaw.ino
  @brief      Print roll, pitch, yaw and heading angles
  @author     David Such
  @copyright  Please see the accompanying LICENSE file.

  Code:        David Such
  Version:     1.0.0
  Date:        22/02/22

  1.0.0 Original Release.           22/02/22

  This sketch is congigured to work with the MADGWICK, MAHONY,
  CLASSIC and COMPLEMENTARY Sensor Fusion options. Set the 
  algorithm that you wish to use with:

  imu.setFusionAlgorithm(SensorFusion::MADGWICK);

  If you want to test the FUSION (Madgwick v2) algoritm, then use
  the fusionAHRS example sketch or add the FUSION specific
  configuration to this sketch.

******************************************************************/

#include <NexgenAHRS.h>

LSM9DS1 imu;
EulerAngles angles;

int loopFrequency = 0;
const long displayPeriod = 10;
unsigned long previousMillis = 0;

void setup() {
  // Initialise the LSM9DS1 IMU
  imu.begin();

  //  Positive magnetic declination - Sydney, AUSTRALIA
  imu.setDeclination(-12.79);
  imu.setFusionAlgorithm(SensorFusion::FUSION);
  imu.setFusionPeriod(0.01f);    // Estimated sample period = 0.01 s = 100 Hz
  imu.setFusionThreshold(0.5f);  // Stationary threshold = 0.5 degrees per second
  imu.setFusionGain(0.25);        // Default Fusion Filter Gain

  //  Start Serial and wait for connection
  Serial.begin(115200);
  while (!Serial)
    ;

  if (imu.connected()) {
    Serial.println("LSM9DS1 IMU Connected.");

    //  Paste your calibration bias offset HERE
    //  This information comes from the testAndCalibrate.ino
    //  sketch in the library examples sub-directory.
    imu.loadAccBias(0.015320, -0.026245, 0.017761);
    imu.loadGyroBias(1.076660, -0.037384, 0.456085);
    imu.loadMagBias(-0.052368, 0.489746, 0.324463);

    //  This sketch assumes that the LSM9DS1 is already calibrated,
    //  If so, start processing IMU data. If not, run the testAndCalibrate
    //  sketch first.
    imu.start();
  } else {
    Serial.println("LSM9DS1 IMU Not Detected.");
    while (1)
      ;
  }
}

void loop() {
  //  Check for new IMU data and update angles
  angles = imu.update();

  //  Display sensor data every displayPeriod, non-blocking.
  if (millis() - previousMillis >= displayPeriod) {

    //  Uncomment to DEBUG raw sensor data:
    // SensorData data = imu.rawData();
    // Serial.print("ax = ");
    // Serial.print(1000 * data.ax);
    // Serial.print(" ay = ");
    // Serial.print(1000 * data.ay);
    // Serial.print(" az = ");
    // Serial.print(1000 * data.az);
    // Serial.println(" mg");
    // Serial.print("gx = ");
    // Serial.print(data.gx, 2);
    // Serial.print(" gy = ");
    // Serial.print(data.gy, 2);
    // Serial.print(" gz = ");
    // Serial.print(data.gz, 2);
    // Serial.println(" deg/s");
    // Serial.print("mx = ");
    // Serial.print(1000 * data.mx);
    // Serial.print(" my = ");
    // Serial.print(1000 * data.my);
    // Serial.print(" mz = ");
    // Serial.print(1000 * data.mz);
    // Serial.println(" mG");

    Serial.print("Roll: ");
    Serial.print(angles.roll);
    Serial.print("\tPitch: ");
    Serial.print(angles.pitch);
    Serial.print("\tYaw: ");
    Serial.print(angles.yaw);
    Serial.print("\tHeading: ");
    Serial.print(angles.heading);
    Serial.print("\tLoop Frequency: ");
    Serial.print(loopFrequency);
    Serial.print(" Hz");
    Serial.print("");
    Serial.println("");


    loopFrequency = 0;
    previousMillis = millis();
  }

  loopFrequency++;
}