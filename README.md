# Fork by Egor812 - Readme Translated to English

# BT-PPM Module
Bluetooth receiver for Headtracker on FRSky Para protocol

Based on BTWifiModule by dlktdr (https://github.com/dlktdr/BTWifiModule)

## Description
We receive data from HeadTracker via Bluetooth using the FRSky Para protocol. In my case, https://github.com/ysoldak/HeadTracker is used on Seed Studio nRF52840 Sense. We send data to the PPM trainer connector.

Needed by those who don't want to solder a BT receiver into the equipment.

## Soldering Connector for Trainer
Seed Studio XIAO-ESP32C3

Jack 3.5 4pin
|= = = >
ABCD
D - PPM - D0
B and C - GND
A - not used

## Hardware settings
Model-Setup-Trainer mode Master/Jack
Sys-Trainer - channels and trimmers.

## Setting up the receiver
Connect via USB. In serial monitor:
Enter the command AT+DISC?
Find the transmitter address in the list. Enter AT+CONaddress (e.g. AT+CONDCB116851608)
Now this receiver will automatically connect to this transmitter

## Note
Performance tested only on ESP32C3
