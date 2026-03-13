# ONR

##Flight software for 2017 CSBF inflatable flight

The 60Mcuft balloon flight from Ft Sumner to 160,000ft tested the communication capability of a 1-meter inflatable spherical primary dish illuminated by a phased array.  Flight electronics adjust 3 phase shifters for pointing control based on balloon IMU position and ground station position.

Communication was UPLINK through CSBF CIP package 16-bit commands, and DOWNLINK by 9600 baud serial.


##Repo includes:
----------------
Flight servers for transmitter, IMU, CIP communications

ARM Cortex-M4 firmware code for GMSK radio (using a CC1100 1-GHz transceiver)

Firmware code for ground station CIP simulator

GUIs for ground control and simulators

![alt text](https://github.com/abegyoung/ONR/blob/main/gui/groundControl/images/main.png?raw=true)

