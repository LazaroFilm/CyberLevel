/* === === === === === Fusion === === === === === */


void setupFusion() {
}
void loopFusion() {

  // Acquire latest sensor data
  const int timestamp = millis();  // replace this with actual gyroscope timestamp
  // Serial.print(timestamp);
  // Serial.print("\t");
  FusionVector gyroscope = { gx, gy, gz };      // replace this with actual gyroscope data in degrees/s
  FusionVector accelerometer = { ax, ay, az };  // replace this with actual accelerometer data in g
  // FusionVector magnetometer = { mx, my, mz };   // replace this with actual magnetometer data in arbitrary units

  // Apply calibration
  gyroscope = FusionCalibrationInertial(gyroscope, gyroscopeMisalignment, gyroscopeSensitivity, gyroscopeOffset);  // The default values are saved in the main sketch.
  // gyroscope = FusionCalibrationInertial(gyroscope, FUSION_IDENTITY_MATRIX, FUSION_VECTOR_ONES, gyroscopeOffset);  // of use forced default as per https://github.com/xioTechnologies/Fusion/issues/58#issuecomment-1377670775
  accelerometer = FusionCalibrationInertial(accelerometer, accelerometerMisalignment, accelerometerSensitivity, accelerometerOffset);
  // magnetometer = FusionCalibrationMagnetic(magnetometer, softIronMatrix, hardIronOffset);

  // Update gyroscope offset correction algorithm
  gyroscope = FusionOffsetUpdate(&offset, gyroscope);

  // Calculate delta time (in seconds) to account for gyroscope sample clock error
  static int previousTimestamp;
  const float deltaTime = (float)(timestamp - previousTimestamp) / 1000;
  previousTimestamp = timestamp;
  // Serial.print(deltaTime);

  // Update gyroscope AHRS algorithm
  FusionAhrsUpdateNoMagnetometer(&ahrs, gyroscope, accelerometer, deltaTime);
  // FusionAhrsUpdate(&ahrs, gyroscope, accelerometer, magnetometer, deltaTime);

  // Print algorithm outputs
  const FusionEuler euler = FusionQuaternionToEuler(FusionAhrsGetQuaternion(&ahrs));
  // const FusionVector earth = FusionAhrsGetEarthAcceleration(&ahrs);

  angle = euler.angle.pitch;

  fusionTime = millis();
  cax = gyroscope.axis.x;
  cay = gyroscope.axis.y;
  caz = gyroscope.axis.z;
  cgx = accelerometer.axis.x;
  cgy = accelerometer.axis.y;
  cgz = accelerometer.axis.z;

  const FusionQuaternion quaternion = FusionAhrsGetQuaternion(&ahrs);
  quatw = quaternion.element.w;
  quatx = quaternion.element.x;
  quaty = quaternion.element.y;
  quatz = quaternion.element.z;
}
