# Motor Control Project

### Abstract

The project involves the development of a motor speed/position control program. This program involves EIC, TC interrupts, PORT, PWM, ADC, and DAC.

### Equipments
- SAM D20 Microcontroller
	- ADC, DAC, PWM, EIC Peripherals 
- The Extension Board
	- POT
	- Four Multiplexed Seven Segment LEDs
- H-bridge Motor
- Oscilloscope and Function Generator

### Detailed Description

- The main() consists of initialization of all related peripherals and an infinite dummy loop. There are two timer generated interrupts. One for the 7-segment display, keypad interface, and the state machine. This timer interrupt has the lowest priority, and it is set to about 60Hz. The other timer interrupt is for implementing the speed and position control and a low-pass digital filter. This timer interrupt has a medium priority level and should be set to 200Hz. Encoder quadrature signal is decoded by EIC with the highest priority of all the interrupts. 
- The following figure shows the high level code block diagrm.

![High Level Code Block Diagram](https://raw.githubusercontent.com/jbp261/Microcontroller-Projects/master/Images/HighLevelCode.png "High Level Code Block Diagram")

### File Description

- **Motor_Control.c**: Implementation ofmotor control program.

### Results

- The following figure is the state diagram of the motor control implemented in the project successfully. 

![The State machine](https://raw.githubusercontent.com/jbp261/Microcontroller-Projects/master/Images/stateMachine.png "The State machine")


- The following table shows the specifications of the keypad keys and their respective function in the state machine.

|Key|Function|
| ------------- | ------------------------------ |
| 1   →  Enter the desired speed | To enter the speed control mode |
| 2  →  Enter the desired position (including ‘-’) | To enter the position control mode and decide the position of the motor |
| 0 → Decel to 0 RPM | To get to ideal mode with 0 RPM |
|A| Negative Sign|
|D|Enter|
|C|Clear the Entry|


