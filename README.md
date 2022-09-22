# Built Robotics Design Challenge - Remote Accessible 24VDC Relay

Project Completed by Jack McNamara

Project Commencement Date: 14/09/2022

# Project Title

Remotely Actuated Relay System Design

# Project Description

The goal of this challenge is to design and prototype a remotely actuated relay that may be wired
into a low power circuit to open that circuit when commanded to do so.

# Firmware Installation
1.	Remove cover from relay assembly
2.	Set DIP Switches 5, 6 and 7 on Arduino Board to ON position
3.	Connect Micro-USB  to USB A cable to Arduino board and to computer
4.	Check COM port the Arduino is connected to in Windows Device Manager>Ports (COM & LPT) (Should appear as CH340 Device or similar)
5.	Open Arduino sketch in Arduino IDE
6.	Under File>Preferences>Additional Board Managers add the URL: http://arduino.esp8266.com/stable/package_esp8266com_index.json and click Save Changes
7.	Set the board to “Generic ESP8266 Board” under Tools>Board
8.	Set the COM Port to the determined COM Port under Tools>Port
9.	Upload the sketch to the Arduino board
10.	Remove the Micro-USB cable from the Arduino board
11.	Set the DIP Switch 7 to OFF position
12.	Connect power supply to board
13. Press ESP Reset Button on Board

# GUI Usage
1. Ensure computer has an internet connection
2. Open remote-access-gui folder
3. Run remote-access-gui.exe

# Non-GUI Usage
## Switching On
1. Ensure computer has an internet connection
2. Open Web Browser
3. Got to https://us01.proxy.teleduino.org/api/1.0/328.php?k=75977832B9FA59765C7C3EB33CCDB29C&r=setDigitalOutput&pin=2&output=1&expire_time=0&save=0


## Switching On
1. Ensure computer has an internet connection
2. Open Web Browser
3. Got to https://us01.proxy.teleduino.org/api/1.0/328.php?k=75977832B9FA59765C7C3EB33CCDB29C&r=setDigitalOutput&pin=2&output=0&expire_time=0&save=0
