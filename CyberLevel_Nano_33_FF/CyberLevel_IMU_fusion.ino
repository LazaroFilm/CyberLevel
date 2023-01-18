/* === === === === === IMU === === === === === */

void setupIMU() {
  if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU!");
    while (1)
      ;
  }
  /*******************    For an improved accuracy run the DIY_Calibration_Accelerometer sketch first.     ****************
********************         Copy/Replace the lines below by the code output of the program              ****************/
  // Accelerometer code
  //  Number of calibration samples 2000
  //  Full Scale setting 2 = 4g
  //  Output Data Rate (ODR) setting 4 = 242Hz (actual value)
  IMU.setAccelFS(2);
  IMU.setAccelODR(3);
  IMU.setAccelOffset(0.002186, 0.010529, 0.017599);
  IMU.setAccelSlope(0.995454, 1.006846, 0.999106);


  // Gyroscope code
  // Full Scale setting 3 = 2000°/s
  // Output Data Rate (ODR) setting 4 = 243Hz (actual value)
  // Number of calibration samples 2000
  // Calibrate Offset (keep board still during measurement)
  IMU.setGyroFS(3);
  IMU.setGyroODR(3);
  IMU.setGyroOffset(2.099704, 0.203151, -0.2691);
  IMU.setGyroSlope(1.296983, 1.206265, 1.185898);

  IMU.setMagnetFS(0);
  IMU.setMagnetODR(8);
  IMU.setMagnetOffset(0, 0, 0);    //  uncalibrated
  IMU.setMagnetSlope(1, 1, 1);     //  uncalibrated
                                   /***********************************************************************************************************************************
*******  FS  Full Scale         range 0:±2g | 1:±24g | 2: ±4g | 3: ±8g  (default=2)                                           ******
*******  ODR Output Data Rate   range 0:off | 1:10Hz | 2:50Hz | 3:119Hz | 4:238Hz | 5:476Hz, (default=3)(not working 6:952Hz) ******
************************************************************************************************************************************/
  IMU.accelUnit = GRAVITY;         // or  METERPERSECOND2
  IMU.gyroUnit = DEGREEPERSECOND;  //   DEGREEPERSECOND  RADIANSPERSECOND  REVSPERMINUTE  REVSPERSECOND
  IMU.magnetUnit = MICROTESLA;     //   GAUSS   MICROTESLA   NANOTESLA

  Serial.println("Gyroscope in degrees/second \n");
  Serial.print("Accelerometer Full Scale = ±");
  Serial.print(IMU.getAccelFS());
  Serial.println("g");
  Serial.print("Accelerometer sample rate = ");
  Serial.print(IMU.getAccelODR());  // alias  AccelerationSampleRate());
  Serial.println(" Hz \n");

  // Serial.println("Magnetic Field in µT");
  // Serial.print("Magnetometer Full Scale = ±");
  // Serial.print(IMU.getMagnetFS());
  // Serial.println("µT");
  // Serial.print("Magnetic field sample rate = ");
  // Serial.print(IMU.getMagnetODR());  // alias IMU.magneticFieldSampleRate in library version 1.01
  // Serial.println(" Hz");
  // delay(4000);

  // Serial.println(" X \t Y \t Z ");
}

void loopIMU() {
  float x, y, z;

  if (IMU.accelAvailable())  // alias IMU.accelerationAvailable in library version 1.01
  {
    IMU.readAccel(x, y, z);  // alias IMU.readAcceleration  in library version 1.01
    ax = x;
    ay = y;
    az = z;
  } else {
    // Serial.print("Accel data missing");
    // while (1) {
    //   ;
    // }
  }

  if (IMU.gyroAvailable())  // alias IMU.gyroscopeAvailable
  {
    IMU.readGyro(x, y, z);  // alias IMU.readGyroscope
    gx = -x;
    gy = -y;
    gz = -z;
  } else {
    // Serial.print("Gyro data missing");
    // while (1) {
    //   ;
    // }
  }

  // if (IMU.magnetAvailable())  // alias IMU.magneticFieldAvailable in library version 1.01
  // {
  //   IMU.readMagnet(x, y, z);  // alias IMU.readMagneticField in library version 1.01
  //   mx = x;
  //   my = y;
  //   mz = z;
  //   } else {
  //   Serial.print("Mag data missing");
  // }

  // Serial.print("Accel: ");
  // Serial.print(ax);
  // Serial.print('\t');
  // Serial.print(ay);
  // Serial.print('\t');
  // Serial.print(az);

  // Serial.print("\tGyro: ");
  // Serial.print(gx);
  // Serial.print('\t');
  // Serial.print(gy);
  // Serial.print('\t');
  // Serial.print(gz);

  // Serial.print("\tMag: ");
  // Serial.print(mx);
  // Serial.print('\t');
  // Serial.print(my);
  // Serial.print('\t');
  // Serial.print(mz);
}

/* === === === === === Fusion === === === === === */


void setupFusion() {
}
void loopFusion() {

  // const FusionVector accelerometer = { ax, ay, az };  // replace this with actual accelerometer data in g
  // const FusionVector gyroscope = { gx, gy, gz };      // replace this with actual gyroscope data in degrees/s

  // Acquire latest sensor data
  const clock_t timestamp = clock();            // replace this with actual gyroscope timestamp
  FusionVector gyroscope = { gx, gy, gz };      // replace this with actual gyroscope data in degrees/s
  FusionVector accelerometer = { ax, ay, az };  // replace this with actual accelerometer data in g
  FusionVector magnetometer = { mx, my, mz };   // replace this with actual magnetometer data in arbitrary units

  // Apply calibration
  gyroscope = FusionCalibrationInertial(gyroscope, gyroscopeMisalignment, gyroscopeSensitivity, gyroscopeOffset);
  accelerometer = FusionCalibrationInertial(accelerometer, accelerometerMisalignment, accelerometerSensitivity, accelerometerOffset);
  magnetometer = FusionCalibrationMagnetic(magnetometer, softIronMatrix, hardIronOffset);

  // Update gyroscope offset correction algorithm
  gyroscope = FusionOffsetUpdate(&offset, gyroscope);

  // Calculate delta time (in seconds) to account for gyroscope sample clock error
  static clock_t previousTimestamp;
  const float deltaTime = (float)(timestamp - previousTimestamp) / (float)CLOCKS_PER_SEC;
  previousTimestamp = timestamp;

  // Update gyroscope AHRS algorithm
  FusionAhrsUpdateNoMagnetometer(&ahrs, gyroscope, accelerometer, deltaTime);
  // FusionAhrsUpdate(&ahrs, gyroscope, accelerometer, magnetometer, deltaTime);

  // Print algorithm outputs
  const FusionEuler euler = FusionQuaternionToEuler(FusionAhrsGetQuaternion(&ahrs));
  // const FusionVector earth = FusionAhrsGetEarthAcceleration(&ahrs);

  angle = euler.angle.roll;

  int time = millis();
  // Accellerometer / Gyroscope message to x-IMU3 software
  // I,1000000,0.0000,0.0000,0.0000,0.0000,0.0000,1.0000\r\n
  Serial.print("I,");
  Serial.print(time);
  Serial.print("00,");
  Serial.print(gx, 4);
  Serial.print(",");
  Serial.print(gy, 4);
  Serial.print(",");
  Serial.print(gz, 4);
  Serial.print(",");
  Serial.print(ax, 4);
  Serial.print(",");
  Serial.print(ay, 4);
  Serial.print(",");
  Serial.print((az - 1), 4);
  Serial.print("\r\n");


  const FusionQuaternion quaternion = FusionAhrsGetQuaternion(&ahrs);
  float quatw = quaternion.element.w;
  float quatx = quaternion.element.x;
  float quaty = quaternion.element.y;
  float quatz = quaternion.element.z;

  // Quaternion message to x-IMU3 software
  // Q,1000000,1.0000,0.0000,0.0000,0.0000\r\n
  Serial.print("Q,");
  Serial.print(time);
  Serial.print("00,");
  Serial.print(quatw, 4);
  Serial.print(",");
  Serial.print(quatx, 4);
  Serial.print(",");
  Serial.print(quaty, 4);
  Serial.print(",");
  Serial.print(quatz, 4);
  Serial.print("\r\n");
}
