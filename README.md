# NodeMCU Controlled Timed Charger
This is a hobby project that involves designing a circuit (using an MCU) for **turning ON/OFF** an AC appliance for a given period of time.

## Features of the Device:

* Control AC Appliances by simply connecting to the AC Female Socket provided *(tested only on smartphone chargers, will definitely fail if used with appliances like an oven or iron)* 

* Once turned on by plugging in to power and pressing the pushbutton, the device keeps the appliance ON for the default timer period (can be altered in the source code)

* Pause/Resume: Device comes with a *pause/resume* button, if pressed - the device timer is paused and the power to the appliance is interrupted until the resume button is pressed.

    *Useful in scenarios as in - if a call is received during the charging of a smartphone, the user can pause the device, unplug the phone, complete the call, plug it back in and resume the device timer.* 

* Wifi Control: The device is capable of creating a WiFi Access Point with the SSID NodeMCU (SSID and Password can be altered in code), the user can login to the network and visit the Device Control Panel to 
    * stop the current Timer duration 
    * set a new Custom Timer duration

* DeepSleep: Once a charging timer period is completed, the device waits for a given standby period (alterable in code) for new commands, if no input is received the device goes into deepsleep mode during which it consumes only about 20uA of current as it turns off all components except the RTC.

* Shutdown Beeps: When a timer duration ends, the device warns the user by creating a sequence of beeps.

## Instructions for replicating

* Setup up the circuit as shown in the schematic given below. The Schematic can be found as a PDF in the folder `circuit-schematics`.


![](circuit-schematics\simple-schematic.png)


* Install Arduino-IDE and setup the libraries for flashing code into NodeMCU devices using the instructions given in `arduino-ide-libraries\readme.txt`

* The code for the MCU operations can be found in `code\nodemcu-charger\nodemcu-charger.ino`. Create desirable tweaks to the code, Compile it and then upload to your device.

## Notes for Operation

* Don't feed voltage greater than 3.3v voltages to the NodeMCU I/O pins

* The default Serial baudrate is set to `9600`.

* The NodeMCU Web control panel can be accessed at `192.168.2.1`.

