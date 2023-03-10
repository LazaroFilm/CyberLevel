# CyberLevel
*A gyro balanced digital level for Steadicam.*
---

![CyberLevel prototypes](https://raw.githubusercontent.com/LazaroFilm/CyberLevel/main/public/img/CyberLevel%20.jpg "CyberLevel")

Using a microcontroller and a IMU with at least an accelerometer and a gyroscope fused together to control a strip of addressable RGB LEDs.
The CyberLevel shows a virtual spirit level to give a simple and fast fisual feedback of the yaw (horizon) of the camera. With the benefit of being gyro stabilized, it isn't impacted by acceleration forces from the accelerometer. 

### Two versions under work:
- BNO055: 
- Nano 33 ble: 

## 1 - BNO055
Uses a Pro Micro MCU and a BNO055 to get the AHRS information. The benefit is the simplicity of the BNO doing all the fusion inside the board. The down side is the extra board. Also requires a power regulator to accept 16+V. The Fusion algo being a "black box" with to user control, I cannot tune it to match our needs in steadicam, and I can't predict it's behavior in case of issues. Additionally, the calibration is tedious and "black boxed" as well.

## 2 - Nano 33 ble
This uses the Arduino Nano 33 ble as MCU/IMU. The benefits are a single board is necessary with everything already there, Powerful chip, IMU and 20v max input. The only downside of this is that the fusion needs to be programmed. 

Found a great repo called [Nextgen_AHRS](https://bitbucket.org/David_Such/nexgen_ahrs/src/main/) that already implements sensor fusion on the Nano 33 ble, the code works and I was able to implement it into the CyberLevel. Using the ```Fusion``` mode gives the best results, however it still doesnt behave as well as the BNO055, especially when "knocked", it takes too long for the bubble to re-center itself.

## 3 - created version FF (Femme /Fusion) 
Now using a [newer Fusion algorithm](https://github.com/xioTechnologies/Fusion) that includes rejection of gyro data during fast moves.
I'm also using FemmeVerbeek's [IMU library](https://github.com/FemmeVerbeek/Arduino_LSM9DS1) library to benefit from the better calibration and exposed controls.
The resulta are promising, i hd to use X-IMU3 GUI app to troubleshoot the orientation issues of the sensors. 
I'm fine tuning the calibration manually, using the log data from X-IMU3 gui. i have also noticed an issue with spiked in the Y axis gyro when ar rest, which are interfering with the sensor calibration and the Fusion reset when at rest function. i will need to find a way to remove those spikes, either by solving the issue itself, or isolating apikes when previous data is at rest. 