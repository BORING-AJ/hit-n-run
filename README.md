<img width="1280" height="640" alt="git (1)" src="https://github.com/user-attachments/assets/8920b256-2ba8-4988-b824-5351134eb4bd" />



#Hit-N-Run 🎯


## Basic Details
### Team Name: PADAKALAM


### Team Members
- Member 2: AJITH ARAVIND T A - [GEC KOZHIKODE]
- Member 3: ARJUN C - [GEC KOZHIKODE]

### Project Description
Hit-N-Run is an autonomous two-wheel car that detects obstacles using an IR sensor. When an obstacle is detected, it stops, triggers a random audio clip on a connected phone, then reverses and turns before continuing.

### The Problem (that doesn't exist)
The serious problem of cars not having dramatic reactions when they hit things.

### The Solution (that nobody asked for)
A car that intentionally hits obstacles, stops, makes your phone play a random audio clip, and then runs away from the obstacle.

## Technical Details
### Technologies/Components Used
For Software:
C++
HTML/CSS
JavaScript
Arduino IDE
ESP8266 Wi-Fi Web Server

For Hardware:
ESP8266 NodeMCU V3
L293D motor driver
2 × BO DC motors
IR obstacle sensor
2 × 18650 Li-ion batteries
Battery holder
2 × XL4015 buck converters
Perfboard
Robot car chassis
Smartphone

### Implementation
For Software:
The ESP8266 runs the autonomous car logic and hosts a Wi-Fi webpage. The IR sensor detects obstacles, while the phone handles random audio playback. The car waits for the audio to finish before continuing its movement sequence.

Installation
Install Arduino IDE.
Install ESP8266 board support.
Open IR_Hit_And_Turn_Car.ino.
Select the ESP8266 NodeMCU board.
Upload the code.

# Run
Power the car.
Connect the phone to the IR_CAR Wi-Fi network.
Open 192.168.4.1 in a browser.
Select an audio folder or MP3 files.
Enable sound.
Start the car.

### Project Documentation
For Software:

# Screenshots (Add at least 3)
<img width="963" height="1280" alt="WhatsApp Image 2026-09-12 at 04 22 49 (1)" src="https://github.com/user-attachments/assets/792be60a-0ba5-4502-acbb-3b7c0b1e0be6" />





# Diagrams
HIT
 ↓
STOP
 ↓
RANDOM PHONE AUDIO
 ↓
REVERSE
 ↓
TURN LEFT
 ↓
FORWARD

For Hardware:

# Schematic & Circuit

Power;
2 × 18650 Battery Pack
        |
        +----> XL4015 ----> 5 V ----> ESP8266
        |
        +----> XL4015 ----> 6 V ----> L293D Motor Supply
                                      |
                                      +----> Left Motor
                                      |
                                      +----> Right Motor

All GNDs connected together

circuit operation;
IR Sensor
    |
    | Obstacle detected
    ↓
ESP8266
    |
    +----> L293D ----> Motors
    |
    +----> Wi-Fi ----> Phone

### Project Demo
# Video
https://drive.google.com/drive/folders/1ThnIVlz2DvqX8lErsPWn2XPdVw4aib3D?usp=drive_link




## Team Contributions
- Ajith Aravind T A: PROJECT DESIGN,CODING,circuit design
- Arjun c: hardware integration,assembly


---
Made with ❤️ at TinkerHub Useless Projects 

![Static Badge](https://img.shields.io/badge/TinkerHub-24?color=%23000000&link=https%3A%2F%2Fwww.tinkerhub.org%2F)
![Static Badge](https://img.shields.io/badge/UselessProjects--26-26?link=https%3A%2F%2Ftinkerhub.org%2Fevents%2F1M8ORET9A1%2Fuseless-projects-3.0)



