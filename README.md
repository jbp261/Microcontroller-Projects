# Microcontroller-Projects

### Project Member
- Jay Patel

### Project Description

The goal of this project is to explore all the functionality of the SAM D20 microcontroller and understand the operation of the underlying ARM Cortex M0+ chip. The projects aimed at building applications like a de-bouncing circuit, a calculator, etc. Understanding and analyzing the different functionalities of the microcontroller to engineer a product with minimal complexity was the primary goal. 

The following topics were coverd in depth.
Embedded system design challenge and metrics. Processor and IC technologies. Software and hardware architectures for embedded system design. Design flow and tools. The design of standard peripherals, microcontrollers, single-purpose and general-purpose processors. Advanced concepts of interfacing and communication protocols in embedded systems

### Equipments
- SAM D20 J18 32-bit ARM® Cortex®-M0+ Microcontroller 

![SAMD 20 J18 MCU](https://raw.githubusercontent.com/jbp261/Microcontroller-Projects/master/Images/ATSAMD20-XPRO.jpg "SAMD 20 J18 MCU")

- Extension Board

![Extension Board](https://raw.githubusercontent.com/jbp261/Microcontroller-Projects/master/Images/Extention_Board.png "Extension Board")


- H-Bridge Motor

![Motor Fan](https://raw.githubusercontent.com/jbp261/Microcontroller-Projects/master/Images/Load_Motor_Fan.png "Motor Fan")

### Environment
- Atmel Studio 7 IDE
![IDE](https://raw.githubusercontent.com/jbp261/Microcontroller-Projects/master/Images/AtmelStudio7.jpeg "IDE")

- Matlab by Mathworks

### Installation 

Atmel Studio Installation Guide [**HERE**](https://www.youtube.com/watch?v=q8ruiOJhNbk "**HERE**")

The following steps guide to create a new project in Atmel Studio

1. Open Atmel Studio
2. Click on File
	a. Go to New &rightarrow; Project
3. Click on GCC C ASF Board Project C/C++
	a. Name the file
	b. Select the location of the file to save by clicking on the Browse button
	Click 'ok'
4. Board Selection winow will appear
	a. Click on the Device Family to select SAMD
	b. Find ATSAMD20J18 in the list
	c. Once that is clicked on, two options will appear at the bottom of the screen
	- Select the User Board template - ATSAMD20J18.
	d. Click 'ok'
5. Atmel Studio will then create and load in the new project
6. In the Solution Explorer window, find the src file and expand it.
	a. Open the  '**main.c**' file
	b. This is the file where all the code goes in
	- All the files shown in this projects are thee main.c files.
### Folder Description 
- **GPIO**: General Purpose Input Output related projects
- **ADC/DAC**: Analog to Digital Conversion and Digital to Analog Conversion related projects
- **TC-PWM**: Timer Control and Pulse Width Modulation related projects
- **Filters-MotorSpeedMeasurement**: Implementation of digital filters and motor speed measurement related projects
- **Motor Control**: DC motor control related projects

### Referances

- [SAM D20 Xplained Pro Datasheet](http://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-42102-SAMD20-Xplained-Pro_User-Guide.pdf "SAM D20 Xplained Pro Datasheet")

- [ARM Cortex-M Textbook](https://www.arm.com/resources/education/textbooks/efficient-embedded-systems-design-and-programming "Arm Cortex-M Textbook")

- [Embedded C Programming and the Atmel AVR](https://www.cengage.com/c/embedded-c-programming-and-the-atmel-avr-2e-barnett/9781418039592 "Embedded C Programming and the Atmel AVR")

### Acknowledgment
- Special thanks to [Dr. Youngsoo Kim](http://www.sjsu.edu/people/youngsoo.kim/ "Dr. Youngsoo Kim") at San Jose State University for his significant efforts in explaining all the difficult concepts of embedded systems.

- Many thanks to Alan Chen and Marcus Garcia for their help in debugging various problems I faced executing the projects. 
