# CyberLevel_releases
Download the latest firmware update for your CyberLevel: 
https://github.com/LazaroFilm/CyberLevel_releases/releases/latest

# User Manual:
## 1 - Power up:
To turn on the CyberLevel, simply connect power, either from a 12V power source through the 2 pin Lemo connector, or via USB-C. DO NOT CONNECT BOTH AT THE SAME TIME!
The CyberLevel will automatically start

![splash screen](https://github.com/LazaroFilm/CyberLevel_releases/assets/18351100/da1c6c43-7fa4-4b6c-be02-7c130f806d6c)

If the splach screen doesn't appear, try power cycling the CyberLevel by unplugging then re-plugging it. 
## 2 - Home Screen:
The Home screen displays your horizon level, as well as the battery level if on a 12V power source.
- The top gray bar is the Tare button.
- The bottom gray bar is the Menu button.
- 
![home screen](https://github.com/LazaroFilm/CyberLevel_releases/assets/18351100/2e5c45fa-7b6a-41f4-8fc7-ff8f7b655da6)

## 3 - Tare:
If your level isn't aligning with the level of your camera, you can tare it. 
- Press and hold the top blue button to start the tare sequence. You will see a countdown. Keep your level in as still as possible.
- When it reaches 0, the CyberLevel will read the the position and save it. This stays saved even after powering off.
- You can cancel the tare anytime during the countdown.

![tare menu](https://github.com/LazaroFilm/CyberLevel_releases/assets/18351100/55e04baf-36d2-468c-9190-b4c43252a6f3)

## 4 - Menu:
Changes in the menu are saved when returning to the Home Screen by pressing the top of bottom buttons. The settings will not be saved if you power off before returning to the Home Screen.

### - Brightness
Slide the two sliders to change the brightness of the LCD screen and LED bar. 

![menu brightness](https://github.com/LazaroFilm/CyberLevel_releases/assets/18351100/496d5e16-014c-4c57-901e-6de32285107d)

### - Settings
Here you can change the following settings:
- Sensit.: The value indicates the range the level will display: for instance 10° will display tilt between -10° and +10°.
- Larry: This speacial feature shows the lateral linear acceleration with blue lights on the LED bar. Note: you need to power cycle your CyberLevel after enabling it.
- flip: These two switches will flip the direction of the bubble on your LCD screen and LED bar. This is useful to mount them upside down for instance.

![menu settings](https://github.com/LazaroFilm/CyberLevel_releases/assets/18351100/35437039-a192-4239-980a-725e0cbc681b)

## - Battery
- batt: Displays your current power source voltage.
- warning: Use the slider to select your low battery warning level. The warning wil show yellow LEDs 0.3v before your warning then blink red when below. A quick tap on the slider resets to 12v warning level.
- offset: Due to voltage drop, your CyberLevel may display a different voltage than your camera's voltage reading. You can offset the CyberLevel's reading to match the camera's. Use the slider so the batt reading from above matches your camera. A quick tap on the slider resets to 0.

![menu battery](https://github.com/LazaroFilm/CyberLevel_releases/assets/18351100/7859ac10-4eca-4b57-bbbf-8076ff9f7598)

## - File
This menu gives displays information about your Cyberlevel, such as Serial number and Software version. 
- Reset button: Long press to reset the saved settings to default. Once pressed the CyberLevel will restart automatically
- Update button: Long press to load a new firmware update to your CyberLevel.

![menu file](https://github.com/LazaroFilm/CyberLevel_releases/assets/18351100/884600d8-76b0-4e61-89da-3c5eb88c02de)

# 5 - To update:
- Download the latest firmware file `CyberLevel_x.x.x_firmware.uf2` to your computer.
- Connect your CyberLevel to your computer via USB-C (make sure it is not connectoed to another power source!)
- In the menu, navigate to the bottom tab. Then long press the `[ update ]` button.
- A drive camed `RPI-RP2` will appear on your computer.
- Drag and drop your firmware file into the drive (do not touch the other files in the drive!).
- Your CyberLevel will automaticall restart when updated. (you may need to turn it off and on again one more time after the update)
- Done!
  
