# Introduction
The goal of this project is to build an automatic hydroponics tower and the software to manage it.

# Supported Functionalities
- Environmental sensing (air quality, water level, light level)
- Automatic Water management between tank and reservoire
- Adaptive artificial lighting
- Task Scheduling
- Changing functionality in an Option Menu
- IOT App used to check the status of the system

# Dependencies
## Hardware
- MSP432P401R Launchpad
- MSP432 Boosterpack MK2
- Arduino uno WiFi
## Software
- Code Composer Studio 12.1.0
- driverlib
- I2C library
- grlib for display
- display driver
- python 3.13.5 and libraries for uart communication
  - tkinter
  - pyserial

# Overview
## Scheduling
The system's various functionalities are performed through tasks.
In order to execute them, a scheduler is used. 
Most of the system's tasks are periodic.

## Option Menu
The tower allows the user to tweak the various functionalities by using the built in option menu.
The user can access the menu either through the boosterpack's joystick, buttons and screen; or through the python application found [here](https://github.com/povo-greenhouse/uart_client) using an UART connection
## Water Management
The tower uses two water containers
- the Tank, where the plants are.
- the Reservoire, where water sits ready to be pumped in the tank
The water is managed through two pumps:
- the first one pumps water from the reservoire to the tank periodically or when the water level is too low
- the second one pumps the water from the tank back in the resevoire periodically or when the level is dangerously close to overflow 
To detect the water levels two sensors are used

## Environmental sensing
The tower has various sensors which are used to detect changes in the environment that might affect plant growth. 
These include 
- An ambient light sensor used to control the brightness of grow lights
- An air quality sensor used to detect air pollutants and notify the user when they reach an unsafe level
- An ambient temperature sensor that notifies the user whether the environment has ideal temperature for the plants
- Water level sensors that notify the user when the reservoire is empty and dictate the functionality of the [Water Management system](#water-management)

The notifications are transmitted to the IoT app and they activate a buzzer on the board.

## IoT APP
The tower connects to an Arduino WIFI through a parallel communication bridge through which it sends data. 
The arduino then connects to the app and sends data through UDP.
The type of data sent and displayed on the app is 
- sensing (air, water, temperature) above threshold
- pumps activated
The IoT app's code can be found [here](https://github.com/povo-greenhouse/IOT-extern-modules)


