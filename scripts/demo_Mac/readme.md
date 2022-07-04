# How to configure the autostart process on Mac?
#### 1. Go to the settings.sh file and change the ENDDEVICE and HUB variables to the mounting points of your USB devices.
#### You can find it out by typing ``` ls /dev/tty* | grep usb ``` into the console.
#### 2. Write the SSID and the Passwort of your WiFi Router.
#### 3. Write the file path of the matterHub project directory.


# How to launch the program on Mac?
#### 1. Click on autostart.sh and start the script on the Terminal. Alternatively, you can also start it by using the Terminal command ``` sh autostart.sh ``` but don't forget to iterate to the folder using ``` cd path_of_the_demo_mac_folder ```.
#### 2. If you want to clean up the esp32 and m5stack before flashing, type in yes or y to clean up else click enter to skip.
#### 3. A new window will appear. If you can see the percentage of the Hub window is going up, it means the Hub is in the flashing process. Now you can continue by clicking enter in the autostart.sh window.
#### 4. Now the same thing for the Enddevice. A window will appear. When you can see the percentage of the Enddevice window is going up, it's time to click enter in the autostart.sh window.
#### 5. Wait until the autostart.sh terminal window says that the autostart process is completed.

#### Warning: Your devices must plugged in before starting the program!
#### If you run matterHub for the first time or have deleted the build directory, the Enddevice and Hub build the dependencies. Be patient, this could take a bit!


# Troubleshooting
#### If your Hub Console gives you an ``` Error: -1 ``` after this matter command: ``` matter switch binding unicast 1 333 1 ``` autostart.sh probably tried to insert the code while matter was executing something else.
#### Type in ``` matter switch binding unicast 1 333 1 ``` and the result should be  ``` Done ```.


#### If you get a Ram or Flash Error on the console for the m5stack, you have to install this tool:
https://blog.squix.org/2021/08/ch9102-driver-issues-failed-to-write-to-target-ram.html


#### If you get a Flash Error, you can try to click on the buttons to reboot and reload the respective device.


#### If you have any other troubles or Error messages like a CMake Error, go to the matterHub Project directory --> src and delete the build directory. After deleting the build directory run autostart.sh and wait until Enddevice and Hub build their dependencies. Be patient, this could take a bit!
