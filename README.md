# CyberLevel
A gyro-compensated digital horizon for Steadicam and camera operators.

official website: [www.lazarocreations.com/cyberlevel](www.lazarocreations.com/cyberlevel)

-----
# User Manual:
## 1. POWER UP:
- Connect the LED strip to the CyberLevel with the 3 pin connector on the left.
- Connect power through the 2 pin connector (max 18v), or via USB-C (5v). The CyberLevel will start automatically after a few seconds.

![splash screen](https://github.com/LazaroFilm/CyberLevel_releases/assets/18351100/da1c6c43-7fa4-4b6c-be02-7c130f806d6c)

If the splash screen doesn’t appear after a few seconds, try power cycling the CyberLevel by unplugging then re-plugging it.

## 2. LED STRIP
The LED strip can be attached on the bezel of your monitor with a 1/2 inch wide velcro strip.
![Screenshot 2024-03-11 at 10 16 31 AM](https://github.com/LazaroFilm/CyberLevel/assets/18351100/e3fcec75-c91b-4217-bcee-7851ae1e7227)

The LED strip has 30 RGB pixels. The center pixel is always purple. The two pixel level indicator will travel across the strip according to your tilt.
![Screenshot 2024-03-11 at 10 17 08 AM](https://github.com/LazaroFilm/CyberLevel/assets/18351100/d1878251-eaf2-45f5-bcc3-ebe423ab7fc9)
The first and last pixels on the strip will turn yellow then red when the battery warning is triggered. You can set your battery warning threshold in the menu. 

## 3. HOME SCREEN:
The Home screen displays your horizon level. The battery indicator is only active when powered via the Lemo connector.
- **Top gray bar** is the Zero Out button. Long press to enter the Zero Out page.
- **Bottom gray bar** is the Menu button. Long press to enter the Menu.

![home screen](https://github.com/LazaroFilm/CyberLevel_releases/assets/18351100/2e5c45fa-7b6a-41f4-8fc7-ff8f7b655da6)

## 4. ZERO OUT:
On the Home Page, press and hold the top gray bar to enter the Zero Out page.

Use this function if your level isn’t aligning with the level of your camera.
Press and hold the top `[ zero out ]` blue button to start the sequence. You will see a countdown. Keep your level in as still as possible during the procedure.
When it reaches 0 and the LED strip displays greed LEDs all across, the CyberLevel will read the the current position as the zero out offset. This stays saved even after powering off.
You can `[ cancel ]` the action anytime during the countdown and the zero out will remain unchanged.

![tare menu](https://github.com/LazaroFilm/CyberLevel_releases/assets/18351100/55e04baf-36d2-468c-9190-b4c43252a6f3)

## 5. MENU
Press and hold the bottom gray bar to enter the Menu.
Return to the Home Screen by tapping either of the two gray bars. 
You must return to the Home Screen to save your settings. 

### <img src="https://raw.githubusercontent.com/FortAwesome/Font-Awesome/6.x/svgs/solid/circle-half-stroke.svg" width="20" height="20">  Brightness
Slide the two sliders to change the brightness of the LCD screen and LED strip.

![menu brightness](https://github.com/LazaroFilm/CyberLevel_releases/assets/18351100/496d5e16-014c-4c57-901e-6de32285107d)

### <img src="https://raw.githubusercontent.com/FortAwesome/Font-Awesome/6.x/svgs/solid/gear.svg" width="20" height="20"> Settings
- **sensit.**: The value indicates the angle range the level will display. For instance, [ 10 ] will display tilt between -10° and +10° on the LCD screen and the 30 LEDs of the strip.
- **flip**: These two switches will flip the direction of the bubble on your LCD screen and LED strip.

![menu_settings](https://github.com/LazaroFilm/CyberLevel/assets/18351100/043f49d1-0df1-44c7-b6e4-b0b2e3a714be)

### <img src="https://raw.githubusercontent.com/FortAwesome/Font-Awesome/6.x/svgs/solid/battery-half.svg" width="20" height="20"> Battery

- **batt**: Displays your current power source voltage. The same reading is displayed on the Home screen. This reading changes with the offset value.
- **warning**: Set the battery warning threshold. The LED strip will show yellow LEDs when the battery reaches 0.3v above the threshold, then it will blink red when the value is reached. A quick tap on the slider resets it to 12v.
- **offset**: Due to voltage drop on Steadicam rigs, your CyberLevel may display a slightly voltage than your camera’s voltage reading. You can offset the CyberLevel’s reading to match the camera’s. Use the slider so the batt reading above matches your camera. A quick tap on the slider resets to 0v.

![menu battery](https://github.com/LazaroFilm/CyberLevel_releases/assets/18351100/7859ac10-4eca-4b57-bbbf-8076ff9f7598)

### <img src="https://raw.githubusercontent.com/FortAwesome/Font-Awesome/6.x/svgs/solid/file.svg" width="20" height="20">  File
This menu displays information about your CyberLevel, such as Serial number and Software version.
- **[RESET <img src="https://raw.githubusercontent.com/FortAwesome/Font-Awesome/6.x/svgs/solid/triangle-exclamation.svg" width="15" height="15">] button**: Long press to reset the saved settings to default. Once pressed the CyberLevel will restart automatically
- **[update <img src="https://raw.githubusercontent.com/FortAwesome/Font-Awesome/6.x/svgs/solid/download.svg" width="15" height="15">] button**: Long press to load a new firmware update to your CyberLevel.

![menu file](https://github.com/LazaroFilm/CyberLevel_releases/assets/18351100/884600d8-76b0-4e61-89da-3c5eb88c02de)

## 5 - UPDATING FIRMWARE:
Future firmware updates will be available here::
https://github.com/LazaroFilm/CyberLevel/releases/latest

- Download the latest firmware file to your computer.
- Connect your CyberLevel to your computer via USB-C (make sure it is not connected to another power source via the 2 pin Lemo!)
- In the menu, navigate to the bottom tab. Then long press the [ update ] button.
- A drive named <img src="https://raw.githubusercontent.com/FortAwesome/Font-Awesome/6.x/svgs/solid/hard-drive.svg" width="15" height="15"> `RPI-RP2` will appear on your computer.
- Drag and drop your uf2 firmware file into the drive.
- The drive will automatically be removed from your computer and your CyberLevel will restart.
(If the CyberLevel doesn’t turn on by itself after the update, you may need to power cycle it manually)


## 6. PINOUT

- **LED Strip 3 pin Lemo: 0B.303**

| Pins |            |
| ---: | ---------- |
|    1 | GND        |
|    2 | 5v         |
|    3 | LED Signal |


- **Power 2 pin Lemo: 0B.302**

| Pins |                |
| ---: | -------------- |
|    1 | GND            |
|    2 | V+ (5v to 18v) |



## 7. CONTACT

Contact us with any questions at lazarocreations@gmail.com
Visit us on the web at [www.lazarocreations.com/cyberlevel](www.lazarocreations.com/cyberlevel)
