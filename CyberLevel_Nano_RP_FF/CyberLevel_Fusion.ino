/* === === === === === Fusion === === === === === */

// Define calibration (replace with actual calibration data)
const FusionMatrix gyroscopeMisalignment = {
    1.0f, 0.0f, 0.0f,  //  xx   xy  xz
    0.0f, 1.0f, 0.0f,  //  yx   yy  yz
    // 0.0f, 1.0f, 0.0665711556829035f,  //  yx   yy  yz
    0.0f, 0.0f, 1.0f   //  zx   zy  zz
  };

  // 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f
// };

const FusionVector gyroscopeSensitivity = { 0.001f, 0.001f, 0.001f };
const FusionVector gyroscopeOffset = { -0.06f, 0.01f, 0.2f };
// const FusionVector gyroscopeOffset = FUSION_VECTOR_ZERO;
const FusionMatrix accelerometerMisalignment = { 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f };
const FusionVector accelerometerSensitivity = { 0.001f, 0.001f, 0.001f };
// const FusionVector accelerometerOffset = { 0.007310f, -0.056019f, -0.002881f };
const FusionVector accelerometerOffset = FUSION_VECTOR_ZERO;

const FusionMatrix softIronMatrix = { 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f };
const FusionVector hardIronOffset = FUSION_VECTOR_ZERO;

const FusionAhrsSettings settings = {
  .gain = 0.025f,
  .accelerationRejection = 10.0f,
  .magneticRejection = 20.0f,
  .rejectionTimeout = 5 * SAMPLE_RATE, /* 5 seconds */
};

void setupFusion() {
  // Setup Fusion
  FusionOffsetInitialise(&offset, SAMPLE_RATE);
  FusionAhrsInitialise(&ahrs);
  FusionAhrsSetSettings(&ahrs, &settings);
}
void loopFusion() {

  // Acquire latest sensor data
  const int timestamp = millis();               // replace this with actual gyroscope timestamp
  FusionVector gyroscope = { gx, gy, gz };      // gyroscope data in degrees/s
  FusionVector accelerometer = { ax, ay, az };  // accelerometer data in g

  // Apply calibration
  gyroscope = FusionCalibrationInertial(FusionAxesSwap(gyroscope, FusionAxesAlignmentPXPYPZ), gyroscopeMisalignment, gyroscopeSensitivity, gyroscopeOffset);  // The default values are saved in the main sketch.
  // gyroscope = FusionCalibrationInertial(FusionAxesSwap(gyroscope, FusionAxesAlignmentNXPYNZ), FUSION_IDENTITY_MATRIX, gyroscopeSensitivity, FUSION_VECTOR_ZERO);  // use forced default as per https://github.com/xioTechnologies/Fusion/issues/58#issuecomment-1377670775
  // accelerometer = FusionCalibrationInertial(FusionAxesSwap(accelerometer, FusionAxesAlignmentNXPYNZ), accelerometerMisalignment, accelerometerSensitivity, accelerometerOffset);
  accelerometer = FusionCalibrationInertial(FusionAxesSwap(accelerometer, FusionAxesAlignmentPXPYPZ), FUSION_IDENTITY_MATRIX, accelerometerSensitivity, FUSION_VECTOR_ZERO);

  // Update gyroscope offset correction algorithm
  gyroscope = FusionOffsetUpdate(&offset, gyroscope);

  // Calculate delta time (in seconds) to account for gyroscope sample clock error
  static int previousTimestamp;
  const float deltaTime = (float)(timestamp - previousTimestamp) / 1000;
  previousTimestamp = timestamp;
  // Serial.print(deltaTime);

  // Update gyroscope AHRS algorithm
  FusionAhrsUpdateNoMagnetometer(&ahrs, gyroscope, accelerometer, deltaTime);  // delta time in sec.
  // FusionAhrsUpdate(&ahrs, gyroscope, accelerometer, magnetometer, deltaTime);

  // Print algorithm outputs
  const FusionEuler euler = FusionQuaternionToEuler(FusionAhrsGetQuaternion(&ahrs));
  // const FusionVector earth = FusionAhrsGetEarthAcceleration(&ahrs);


  yaw = euler.angle.yaw;
  pitch = euler.angle.pitch;
  roll = euler.angle.roll;
  
  angle = pitch;

  fusionTime = millis();
  cgx = gyroscope.axis.x;
  cgy = gyroscope.axis.y;
  cgz = gyroscope.axis.z;
  cax = accelerometer.axis.x;
  cay = accelerometer.axis.y;
  caz = accelerometer.axis.z;

  const FusionQuaternion quaternion = FusionAhrsGetQuaternion(&ahrs);
  quatw = quaternion.element.w;
  quatx = quaternion.element.x;
  quaty = quaternion.element.y;
  quatz = quaternion.element.z;
}
