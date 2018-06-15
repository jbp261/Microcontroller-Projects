# General Purpose Input Output (GPIO) Project

### Abstract

The GPIO project aims to build a software de-bouncing circuit and a calculator using the multiplexed seven-segment LEDs and four by four keypad located on the extension board. 

### Equipments
- SAM D20 Microcontroller
	-  I/O Peripherals 
- The Extension Board
	- 4 by 4 Keypad
	- Four Multiplexed Seven Segment LEDs

### Detailed Description

Debouncing 
- If a button is tapped very quickly, it is not registered as a valid key-press. Also if a button is held down, make sure that your program does not repeatedly accept this key-press multiple times.

Calculator
- A calculator developed using the keypad and 7-segment display as the input/output devices.  The calculator can add and subtract two integer numbers (with sign). The digit key “#” represents the 'add' (+), “*” represents subtraction (-), "A" as the equal (=) and, and "B" as backspace. The cases like consecutive add and subtract operation, as well as over/underflow (more than 9999 or less than -9999), are not covered. The calculator is built by implementing leading zero blanking, multiple, divide, decimal point or floating point. The following figure shows the schematic of the keypad and 7-segment display circuit and their connections to the SAM D20 board.

![Pinpad and LED Schematic](https://raw.githubusercontent.com/jbp261/Microcontroller-Projects/master/Images/Pinpad_LEDs_Sch.png "Pinpad and LED Schematic")

### File Description

- **Calculator_UART.c **: This file contains the operational code for implementing a simple calculator using the SAM D20 microcontroller. 
- **LED_Toggle_1.c** : This file contains the functions to display a specific number or a letter on the quad seven segment display
- **Software_de-bouncing.c**: This file contains the implementation of the de-bouncing circuit state diagram in c. 

### Results
The de-bouncing circuit was implemented successfully by following the state machine diagram shown below. 

![De-bouncing state diagram](https://raw.githubusercontent.com/jbp261/Microcontroller-Projects/master/Images/De-bouncing.png "De-bouncing state diagram")

The calculator was also implemented successfully as per the specification provided in the detailed description section. 
