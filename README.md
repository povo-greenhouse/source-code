# Greenhouse Project
Welcome to the source code repository for the Greenhouse Project! This project is intended for the course `Embedded Systems for the Internet of Things` offered by at the Università di Trento. It designed to automate and monitor greenhouse conditions for optimal plant growth. Below you'll find all the essential information to run, build, and understand the project.

## Table of Contents
- [Overview](#overview)
- [Hardware And Software Requirements](#hardware-and-software-requirements)
- [Project Layout](#project-layout)
- [Supported Functionalities](#supported-functionalities)
- [Getting Started](#getting-started)
- [Possible Errors](#possible-errors)
- [Presentation and Demo Links](#presentation-and-demo-links)
- [Team Members & Contributions](#team-members--contributions)

---

## Overview
### Scheduling
The system's various functionalities are performed through tasks.
In order to execute them, a scheduler is used. 
Most of the system's tasks are periodic.

### Option Menu
The tower allows the user to tweak the various functionalities by using the built in option menu.
The user can access the menu either through the boosterpack's joystick, buttons and screen; or through the python application found [here](https://github.com/povo-greenhouse/uart_client) using an UART connection
### Water Management
The tower uses two water containers
- the Tank, where the plants are.
- the Reservoire, where water sits ready to be pumped in the tank
The water is managed through two pumps:
- the first one pumps water from the reservoire to the tank periodically or when the water level is too low
- the second one pumps the water from the tank back in the resevoire periodically or when the level is dangerously close to overflow 
To detect the water levels two sensors are used

### Environmental sensing
The tower has various sensors which are used to detect changes in the environment that might affect plant growth. 
These include 
- An ambient light sensor used to control the brightness of grow lights
- An air quality sensor used to detect air pollutants and notify the user when they reach an unsafe level
- An ambient temperature sensor that notifies the user whether the environment has ideal temperature for the plants
- Water level sensors that notify the user when the reservoire is empty and dictate the functionality of the [Water Management system](#water-management)

The notifications are transmitted to the IoT app and they activate a buzzer on the board.

### IoT APP
The tower connects to an Arduino WIFI through a parallel communication bridge through which it sends data. 
The arduino then connects to the app and sends data through UDP.
The type of data sent and displayed on the app is 
- sensing (air, water, temperature) above threshold
- pumps activated
The IoT app's code can be found [here](https://github.com/povo-greenhouse/IOT-extern-modules)
---

## Hardware And Software Requirements
### Hardware
- MSP432P401R Launchpad
- MSP432 Boosterpack MK2
- Arduino uno WiFi
- MQ135 air quality sensor
- 2 Water pumps
- 2 Water sensors
- LEDs
- 2N2222 Transistors, resistors, 5V and 3.3V power supplies
- USB cable for programming
- Computer with Git, a code editor, and necessary toolchains (Arduino IDE, PlatformIO, or similar)
- Cables & Breadboard/PCB

### Software
  - **msp.h**: header file for MSP430 microcontrollers that provides hardware register definitions and macros for low-level device control.
  - **driverlib.h**: a high-level API header for MSP430 microcontrollers that provides simplified, hardware-abstracted functions for peripheral control and configuration.
  - **grilib.h**: a graphics library header for MSP430 and similar devices that provides functions for drawing shapes, text, and images on graphical displays.
  - **Code Composer Studio 12.1.0**: An integrated development environment (IDE) for developing and debugging embedded applications on TI microcontrollers.
  - **Python 3.13.5**: A high-level, interpreted programming language known for its readability and broad library support.
    - **tkinter**: A standard Python library for building simple graphical user interfaces (GUIs).
    - **pyserial**: A Python library that enables serial communication between a computer and external devices like microcontrollers.

Note: It is important to have a version <=12.1.0 for code compser studio, else the software will not recognise the board.

---

## Project Layout
Common directories you may find:
```                                                                                                                
├── LICENSE
├── MakeFile
├── README.md
├── include
│   ├── IOT
│   │   └── IOT_communication.h
│   ├── LcdDriver
│   │   ├── Crystalfontz128x128_ST7735.c
│   │   ├── Crystalfontz128x128_ST7735.h
│   │   ├── HAL_MSP_EXP432P401R_Crystalfontz128x128_ST7735.c
│   │   └── HAL_MSP_EXP432P401R_Crystalfontz128x128_ST7735.h
│   ├── adc
│   │   └── adc.h
│   ├── environment_systems
│   │   ├── air_quality.h
│   │   ├── buzzer.h
│   │   └── temperature.h
│   ├── light_system
│   │   └── growing_light.h
│   ├── option_menu
│   │   ├── option_menu.h
│   │   ├── option_menu_input.h
│   │   └── options.h
│   ├── scheduling
│   │   ├── scheduler.h
│   │   └── timer.h
│   ├── uart_communication
│   │   └── uart_comm.h
│   └── water_management
│       ├── pump_management.h
│       ├── water_init.h
│       └── water_reading.h
├── lib
│   ├── HAL_I2C.c
│   └── HAL_I2C.h
├── msp432p401r.cmd
├── src
│   ├── IOT
│   │   └── IOT_communication.c
│   ├── adc
│   │   └── adc.c
│   ├── environment_systems
│   │   ├── air_quality.c
│   │   ├── buzzer.c
│   │   └── temperature.c
│   ├── light_system
│   │   └── growing_light.c
│   ├── main.c
│   ├── option_menu
│   │   ├── option_menu.c
│   │   ├── option_menu_input.c
│   │   └── options.c
│   ├── scheduling
│   │   ├── scheduler.c
│   │   └── timer.c
│   ├── uart_communication
│   │   └── uart_comm.c
│   └── water_management
│       ├── pump_management.c
│       ├── water_init.c
│       └── water_reading.c
├── startup_msp432p401r_ccs.c
├── system_msp432p401r.c
├── targetConfigs
│   ├── MSP432P401R.ccxml
│   └── readme.txt
├── test
│   ├── air_qual_test.c
│   ├── air_qual_test.h
│   ├── buzzer_test.c
│   ├── buzzer_test.h
│   ├── light_test.c
│   ├── light_test.h
│   ├── option_menu_test.c
│   ├── option_menu_test.h
│   ├── scheduling_test.c
│   ├── scheduling_test.h
│   ├── temp_test.c
│   ├── temp_test.h
│   └── test_all.c
├── test_script.sh
└── used_ports.txt
```

---
## Supported Functionalities
- Environmental sensing (air quality, water level, light level)
- Automatic Water management between tank and reservoire
- Adaptive artificial lighting
- Task Scheduling
- Changing functionality in an Option Menu
- IOT App used to check the status of the system
---

## Getting Started
1. Launch Code Composer Studio and open a new workspace in the directory you prefer. Open the terminal and clone the repository as follows:
```
git clone https://github.com/povo-greenhouse/source-code.git
```
2. Select `open an existing project`, `browse` and open the folder in which you cloned the repository. Select `source-code` and click `Finish`. This should open the project in code composer studio.
3. Connect the board and ensure it is recognised by the software.
4. Download the [driverlib library]((https://drive.google.com/file/d/1_5TsECed3wNJpIpllxYYdD06aFbkk7Fc/view?usp=sharing), extract the simplelink_msp432p4_sdk_3_40_01_02.zip and make note of its directory path.
5. Go to Project -> Properties -> Include Options. In the box `Add dir to #include search path (--include_path, -I)`, add the include folder:
```
${PROJECT_ROOT}/include
```
6. Add "simplelink_msp432p4_sdk_3_40_01_02/source".
7. Click ARM Linker and File Search Path
8. Add `${PROJECT_ROOT}/include`, `simplelink_msp432p4_sdk_3_40_01_02/source/ti/devices/msp432p4xx/driverlib/ccs/msp432p4xx_driverlib.lib` and `simplelink_msp432p4_sdk_3_40_01_02/source/ti/grlib/lib/ccs/m4/grlib.a` to "Include library file..." window
9. Run the project from code compser studio and see the magic!

### Running Option menu
If you would like to see and use the option, do the following steps before running the project on code composer studio:
1. clone the [Uart Client](https://github.com/povo-greenhouse/uart_client).
```
git clone https://github.com/povo-greenhouse/uart_client
```
2. Ensure the required libraries are installed, if it is not, run the following in the terminal:
```
pip install tkinter
pip install pyserial
```
3. Run the python script:
```
python3 ui.py
```
A window should appear with display and buttons to control the system.

### Running IOT communication
If you would like to use the application, do the following steps before running the project on code composer studio:
1. On the same or a different computer (if you prefer to see the wifi module in action), clone the [Application](https://github.com/povo-greenhouse/IOT-extern-modules.git)
```
git clone https://github.com/povo-greenhouse/uart_client
```
2. Clone the Arduino specific [IOT Communication]()
3. Burn the project onto the arduino and run the following pythons scripts and connect to the same wifi as your computer:
```
python3 ConnectionSetter.py
python3 gui_module.py
```
Run the project on code compser studio and enjoy your automated plant system!

---
## Possible Errors
```
Could not open ../.. .c
```
1. Follow the steps indicated in [Getting Started](#getting-started) and include the path required.
2. Ensure the files for driverlib and grlib are installed and extarcted, and that the path are correct.

---

## Presentation and Demo Links

- **PowerPoint Presentation:** [Link to PowerPoint](https://example.com/presentation)
- **Video Demo:** [Link to Video](https://youtube.com/example)

---

## Team Members & Contributions

| Member       | GitHub Profile | Contribution Summary                                                                     |
|--------------|----------------|------------------------------------------------------------------------------------------|
| zawarudoda02 | [Profile](https://github.com/zawarudoda02) | Scheduler, option menu, uart communication + lcd for display |
| n1vr1t1      | [Profile](https://github.com/n1vr1t1) | Temperature, air and light systems                                |
| Vins0656     | [Profile](https://github.com/Vins0656) | Water system and IOT communication                               |

> All team members contributed to coding, documentation, and testing, and are familiar with every code file in the project.

---
