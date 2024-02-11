# CyberLevel
A gyro-compensated digital horizon for Steadicam and camera operators.

official website: www.lazarocreations.com/cyberlevel

-----
# User Manual:
## 1 - Power up:
To turn on the CyberLevel, simply connect power, either from a 12V power source through the 2 pin Lemo connector, or via USB-C. The CyberLevel will start automatically.

DO NOT CONNECT BOTH POWER SOURCES AT THE SAME TIME!

![splash screen](https://github.com/LazaroFilm/CyberLevel_releases/assets/18351100/da1c6c43-7fa4-4b6c-be02-7c130f806d6c)

If the splach screen doesn't appear, try power cycling the CyberLevel by unplugging then re-plugging it. 
## 2 - Home Screen:
The Home screen displays your horizon level, as well as the battery level when powered via 12v 2pin Lemo.
- **Top gray bar** is the Tare button. Long press to enter the Tare page. 
- **Bottom gray bar** is the Menu button. Long press to enter the menu. 

![home screen](https://github.com/LazaroFilm/CyberLevel_releases/assets/18351100/2e5c45fa-7b6a-41f4-8fc7-ff8f7b655da6)

## 3 - Tare:
If your level isn't aligning with the level of your camera, you can tare it here. 
- Press and hold the top `[ zero out ]` blue button to start the tare sequence. You will see a countdown. Keep your level in as still as possible.
- When it reaches 0, the CyberLevel will read the the position and save it. This stays saved even after powering off.
- You can `[ cancel ]` the tare anytime during the countdown, the tare will remain unchanged.

![tare menu](https://github.com/LazaroFilm/CyberLevel_releases/assets/18351100/55e04baf-36d2-468c-9190-b4c43252a6f3)

## 4 - Menu:
Changes in the menu are saved when returning to the Home Screen by pressing the top of bottom buttons. The settings will not be saved if you power off before returning to the Home Screen.

### <img src="https://raw.githubusercontent.com/FortAwesome/Font-Awesome/6.x/svgs/solid/circle-half-stroke.svg" width="20" height="20">  Brightness
Slide the two sliders to change the brightness of the LCD screen and LED bar. 

![menu brightness](https://github.com/LazaroFilm/CyberLevel_releases/assets/18351100/496d5e16-014c-4c57-901e-6de32285107d)

### <img src="https://raw.githubusercontent.com/FortAwesome/Font-Awesome/6.x/svgs/solid/gear.svg" width="20" height="20"> Settings
Here you can change the following settings:
- **sensit.**: The value indicates the range the level will display on each sides. For instance, `[ 10 ]` will display tilt between -10° and +10° on the LCD screen and the 30 LEDs of the bar.
- **Larry**: This speacial feature shows the lateral linear acceleration with blue lights on the LED bar. This is useful to detect the side stepping of the camera when moving forward. Note: You will need to power cycle your CyberLevel after enabling it.
- **flip**: These two switches will flip the direction of the bubble on your LCD screen and LED bar. This is useful to mount them upside down for instance.

![menu settings](https://github.com/LazaroFilm/CyberLevel_releases/assets/18351100/35437039-a192-4239-980a-725e0cbc681b)

### <img src="https://raw.githubusercontent.com/FortAwesome/Font-Awesome/6.x/svgs/solid/battery-half.svg" width="20" height="20"> Battery

- **batt**: Displays your current power source voltage. The same reading is displayed on the Home screen.
- **warning**: Use the slider to select your low battery warning level. The warning wil show yellow LEDs 0.3v before your warning then blink red when below. A quick tap on the slider resets to 12v warning level.
- **offset**: Due to voltage drop, your CyberLevel may display a different voltage than your camera's voltage reading. You can offset the CyberLevel's reading to match the camera's. Use the slider so the batt reading from above matches your camera. A quick tap on the slider resets to 0.

![menu battery](https://github.com/LazaroFilm/CyberLevel_releases/assets/18351100/7859ac10-4eca-4b57-bbbf-8076ff9f7598)

### <img src="https://raw.githubusercontent.com/FortAwesome/Font-Awesome/6.x/svgs/solid/file.svg" width="20" height="20">  File
This menu displays information about your Cyberlevel, such as Serial number and Software version. 
- **[RESET <img src="https://raw.githubusercontent.com/FortAwesome/Font-Awesome/6.x/svgs/solid/triangle-exclamation.svg" width="15" height="15">] button**: Long press to reset the saved settings to default. Once pressed the CyberLevel will restart automatically
- **[update <img src="https://raw.githubusercontent.com/FortAwesome/Font-Awesome/6.x/svgs/solid/download.svg" width="15" height="15">] button**: Long press to load a new firmware update to your CyberLevel.

![menu file](https://github.com/LazaroFilm/CyberLevel_releases/assets/18351100/884600d8-76b0-4e61-89da-3c5eb88c02de)

## 5 - Updating the firmware:
The latest firmware update is available at: https://github.com/LazaroFilm/CyberLevel_releases/releases/latest
- Download the latest firmware file `CyberLevel_x.x.x_firmware.uf2` to your computer.
- Connect your CyberLevel to your computer via USB-C (make sure it is not connectoed to another power source via the 2 pin Lemo!)
- In the menu, navigate to the bottom tab. Then long press the `[ update ]` button.
- A drive named <img src="https://raw.githubusercontent.com/FortAwesome/Font-Awesome/6.x/svgs/solid/hard-drive.svg" width="15" height="15"> `RPI-RP2` will appear on your computer.
- Drag and drop your uf2 firmware file into the drive.
- The drive will automatically be removed from your computer and your CyberLevel will restart. (If the CyberLevel doesn't turn on by itself after the update, you may need to power cycle it manually)
- Done!
  
