[![Python Badge](https://img.shields.io/badge/python-2-blue.svg)](https://www.python.org/downloads/)


# Head-up Display Auto-alignment System

## Description
There is a need for aviation head worn systems to be accurately aligned to the aircraft when used in flight environments. Rockwell Collins would like to create a demonstration system for an auto-aligning head worn display using cutting edge micro-electronics and real-time algorithms.

## Project Sponsor
Rockwell Collins


## Authors
  * Jiongcheng Luo
  * Drew Hamm
  * Krisna Irawan

## Usage
The usage/demonstration of this project requires specific hardware components and software environment, all following components are necessary.

 ### Hardware Components
 * [MPU-9250 Breakout](https://www.sparkfun.com/products/13762)
 * [Metro Mini 328](https://www.adafruit.com/product/2590)
 * [I2C Protocol Guide](http://www.byteparadigm.com/applications/introduction-to-i2c-and-spi-protocols/)
 

 ### Software Environment
Sourse code for [Metro Mini 328](https://www.adafruit.com/product/2590) & [MPU-9250](https://www.sparkfun.com/products/13762) (https://www.adafruit.com/product/2590) can be uploaded via [Arduino IDE](https://www.arduino.cc/en/Main/Software) (source code can be found [here](https://github.com/gijoncheng/CapstoneProject-OSU-65/tree/master/src/Auto_Alignment))
 
Graphical UI (for demonstration purpose) was written in [Python](https://www.python.org/downloads/release/python-279/), you will need to install [VPython](https://github.com/pyserial/pyserial) (a 3D graphics module for Python) and [pySerial](https://github.com/pyserial/pyserial) (A Python module encapsulates the access for the serial port) before compiling.

### How to Run
* Wired the microcontrollers with IMUs together via I2C protocal connection
* Instaill Python 2.7.9, VPython and pySerial
* Compile [gui.py](https://github.com/gijoncheng/CapstoneProject-OSU-65/tree/master/src/3D_sim_gui) in command ``python gui.py`` or use Python IDLE 

 
 
 

## Project Documentations
<h4><a href="https://github.com/gijoncheng/CapstoneProject-OSU-65/blob/master/progress_report/main.pdf">Progress Report</a></h4>
<h4><a href="https://github.com/gijoncheng/CapstoneProject-OSU-65/blob/master/design_document/main.pdf">Design Document</a></h4>
<h4><a href="https://github.com/gijoncheng/CapstoneProject-OSU-65/blob/master/tech_review/main.pdf">Technology Review</a></h4>
<h4><a href="https://github.com/gijoncheng/CapstoneProject-OSU-65/blob/master/requirement_document/main.pdf">Requirement Document</a></h4>
<h4><a href="https://github.com/gijoncheng/CapstoneProject-OSU-65/blob/master/problem_statement/problem-statement.pdf">Problem Statement</a></h4>
