
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

  plot("Ax", ax * 1000, false);
  plot("Ay", ay * 1000, false);
  plot("Az", az * 1000, false);

  plot("Gx", gx, false);
  plot("Gy", gy, false);
  plot("Gz", gz, false);


  // plot("Angle", angle, false);

  // Serial.print("\tpixelPos: ");
  // Serial.print(pixelPos);
  // Serial.print("\tangleOffset: ");
  // Serial.print(angleOffset);
  // plot("Raw_Pot", rawValue, false);
  // plot("Smoothed_Pot", Pot, false);
  // plot("Encoder", positionWheel, false);
  // plot("Angle_Offset", angleOffset, false);
  // plot("Pixel", (pixelPos), false);
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

  Serial.println("");  // new line for next sample
  // Serial.println("Min:-90,Max:90");
  // Serial.println("Min:-1025,Max:1025");