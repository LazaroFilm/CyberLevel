
void loopPrint() {
  /* === === === === === SERIAL PRINTS === === === === === */

  //  Uncomment to DEBUG raw sensor data:
  // plot("ax", (1000 * data.ax), false);
  // plot("gx", (2 * data.gx), false);
  // plot("mx", (data.mx), false);
  // plot("ay", (1000 * data.ay), false);
  // plot("gy", (data.gy), false);
  // plot("my", (data.my), false);
  // plot("az", (1000 * data.az), false);
  // plot("gz", (data.gz), false);
  // plot("mz", (data.mz), false);

  // Serial.print("ax = ");
  // Serial.print(1000 * data.ax);
  // Serial.print("ay = ");
  // Serial.print(1000 * data.ay);
  // Serial.print("\t");
  // Serial.print("\taz = ");
  // Serial.print(1000 * data.az);
  // Serial.print(" mg");
  // Serial.print("\tgx = ");
  // Serial.print(data.gx, 2);
  // Serial.print("gy = ");
  // Serial.println(data.gy, 2);
  // Serial.print("\tgz = ");
  // Serial.print(data.gz, 2);
  // Serial.print(" deg/s\t");
  // Serial.print("\tmx = ");
  // Serial.print(1000 * data.mx);
  // Serial.print("\tmy = ");
  // Serial.print(1000 * data.my);
  // Serial.print("\tmz = ");
  // Serial.print(1000 * data.mz);
  // Serial.println(" mG");

  // Serial.print(F("\tAngle: "));
  // Serial.print(Angle, 8);

  // plot("Ax", ax, false);
  // plot("Ay", ay, false);
  // plot("Az", az, false);

  // plot("Gx", gx, false);
  // plot("Gy", gy, false);
  // plot("Gz", gz, false);


  plot("Angle", angle, false);

  // Serial.print("\tpixelPos: ");
  // Serial.print(pixelPos);
  // Serial.print("\tangleOffset: ");
  // Serial.print(angleOffset);
  // plot("Raw_Pot", rawValue, false);
  // plot("Smoothed_Pot", Pot, false);
  plot("Encoder", newValue, false);
  plot("Angle_Offset", angleOffset, false);
  plot("Pixel", (pixelPos), false);
  // Serial.print("\tintPixel: ");
  // Serial.print(intPixel);
  // Serial.print("\tcenterPixel: ");
  // Serial.print(centerPixel);
  // Serial.print("\tdelta: ");
  // Serial.print(intPixel - centerPixel);
  // Serial.print("\tdecimal: ");
  // Serial.print(decimalOnly);

  // Serial.print("\tPot: ");
  // Serial.print(Pot);

  /* Optional: Display calibration status */
  //  displayCalStatus();

  /* Optional: Display sensor status (debug only) */
  // displaySensorStatus();
  Serial.print("loop time: " + String(loopTime) + "us\t" + String(freq) + "Hz");

  Serial.println("");  // new line for next sample
  // Serial.println("Min:-90,Max:90");
  // Serial.println("Min:-1025,Max:1025");
}

void printXIMU() {
  /* Accellerometer / Gyroscope message to x-IMU3 software
  I,1000000,0.0000,0.0000,0.0000,0.0000,0.0000,1.0000\r\n */

  Serial.print("I,");
  Serial.print(fusionTime);
  Serial.print("00,");
  Serial.print(cgx, 4);
  Serial.print(",");
  Serial.print(cgy, 4);
  Serial.print(",");
  Serial.print(cgz, 4);
  Serial.print(",");
  Serial.print(cax, 4);
  Serial.print(",");
  Serial.print(cay, 4);
  Serial.print(",");
  Serial.print(caz, 4);
  Serial.print("\r\n");

  // Serial.print("I,");
  // Serial.print(fusionTime);
  // Serial.print("00,");
  // Serial.print(gx, 4);
  // Serial.print(",");
  // Serial.print(gy, 4);
  // Serial.print(",");
  // Serial.print(gz, 4);
  // Serial.print(",");
  // Serial.print(ax, 4);
  // Serial.print(",");
  // Serial.print(ay, 4);
  // Serial.print(",");
  // Serial.print(az, 4);
  // Serial.print("\r\n");

  /* Quaternion message to x-IMU3 software
  Q,1000000,1.0000,0.0000,0.0000,0.0000\r\n */

  Serial.print("Q,");
  Serial.print(fusionTime);
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

void printSerialStudio() {
  /*KAANSATQRO,2051,2,5,26,10,101.26,27,32,1001,21.1619,86.8515,10,4,1.23,9.81,0.23,0,0,0*/
  Serial.print("/*");
  Serial.print("CyberLevel,"); // 1

  Serial.print(cgx); //2
  Serial.print(",");
  Serial.print(cgy); //3
  Serial.print(",");
  Serial.print(cgz); //4
  Serial.print(",");

  Serial.print(cax); //5
  Serial.print(",");
  Serial.print(cay); //6
  Serial.print(",");
  Serial.print(caz); //7
  Serial.print(",");

  Serial.print(yaw);
  Serial.print(",");
  Serial.print(pitch);
  Serial.print(",");
  Serial.print(roll);
  // Serial.print(",");

  // Serial.print(quatw);
  // Serial.print(",");
  // Serial.print(quatx);
  // Serial.print(",");
  // Serial.print(quaty);
  // Serial.print(",");
  // Serial.print(quatz);
  Serial.println("*/");
}
