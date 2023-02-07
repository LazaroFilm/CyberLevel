/* === === === === === IMU === === === === === */
void setupIMU() {
  // // IMU Interrupt
  // attachInterrupt(INT_1, INT1Event_cb, CHANGE);

  // Init the sensor
  lsm6dsoxSensor.begin();

  // Enable accelerometer and gyroscope, and check success
  if (lsm6dsoxSensor.Enable_X() == LSM6DSOX_OK && lsm6dsoxSensor.Enable_G() == LSM6DSOX_OK) {
    Serial.println("Success enabling accelero and gyro");
  } else {
    Serial.println("Error enabling accelero and gyro");
  }

  // Read ID of device and check that it is correct
  uint8_t id;
  lsm6dsoxSensor.ReadID(&id);
  if (id != LSM6DSOX_ID) {
    Serial.println("Wrong ID for LSM6DSOX sensor. Check that device is plugged");
  } else {
    Serial.println("Receviced correct ID for LSM6DSOX sensor");
  }

  // Set accelerometer scale at +- 2G. Available values are +- 2, 4, 8, 16 G
  lsm6dsoxSensor.Set_X_FS(4);

  // Set gyroscope scale at +- 125 degres per second. Available values are +- 125, 250, 500, 1000, 2000 dps
  lsm6dsoxSensor.Set_G_FS(1000);

  // Set Accelerometer sample rate to 208 Hz. Available values are +- 12.0, 26.0, 52.0, 104.0, 208.0, 416.0, 833.0, 1667.0, 3333.0, 6667.0 Hz
  lsm6dsoxSensor.Set_X_ODR(416.0f);

  // Set Gyroscope sample rate to 208 Hz. Available values are +- 12.0, 26.0, 52.0, 104.0, 208.0, 416.0, 833.0, 1667.0, 3333.0, 6667.0 Hz
  lsm6dsoxSensor.Set_G_ODR(416.0f);

  // // Enable Interrupt pin when G data is ready. Hoping to use this to count the acutal frequency to send to Fusion.
  // lsm6dsoxSensor.Enable_DRDY_G_Int(LSM6DSOX_INT1_PIN);
}

void loopIMU() {
  // Read gyroscope
  uint8_t gyroStatus;
  lsm6dsoxSensor.Get_G_DRDY_Status(&gyroStatus);
  if (gyroStatus == 1) {  // Status == 1 means a new data is available
    int32_t rotation[3];
    lsm6dsoxSensor.Get_G_Axes(rotation);

    gx = rotation[0];
    gy = rotation[1];
    gz = rotation[2];
  } else {
    missingGData = true;
    // Serial.print("Gyr fail");
    // delay(100);
  }

  // Read accelerometer
  uint8_t acceleroStatus;
  lsm6dsoxSensor.Get_X_DRDY_Status(&acceleroStatus);
  if (acceleroStatus == 1) {  // Status == 1 means a new data is available
    int32_t acceleration[3];
    lsm6dsoxSensor.Get_X_Axes(acceleration);
    ax = acceleration[0];  //-
    ay = acceleration[1];
    az = acceleration[2];  //-
  } else {
    // Serial.print("Acc fail");
    // delay(100);
  }
}
