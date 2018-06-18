# Filters and Motor Speed Control Projects

### Abstract

There are two parts of this project. First is to implement a digital filter using external interrupt control (EIC): a 2nd order notch filter. Second is to Implement a motor speed measurement function. The first part uses a timer triggered interrupt and the second part involves both timer and the external interrupt.

### Equipments
- SAM D20 Microcontroller
	- ADC, DAC, PWM, EIC Peripherals 
- The Extension Board
	- POT
	- Four Multiplexed Seven Segment LEDs
- H-bridge Motor
- Oscilloscope and Function Generator

### Additional Environment
- Matlab for digital filter design

### Detailed Description

**The Notch Filter**
- The implementation of an IIR digital 2nd order notch filter that rejects a 57Hz (~377 rad/sec) frequency component with a bandwidth of 5Hz and a sampling frequency of 500Hz. The filter was designed in Matlab. 
- The sampling was done using the external interrupt control. 

**Motor Speed Control**
- The quadrature signals (Phase A and B) from the motor encoder were connected to PA28 and PB14 respectively as shown below. The pin PA28 was set up as an external interrupt input pin.  The EIC handler routine was implemented using the quadrature decoder logic. 

![Motor Encoder](https://raw.githubusercontent.com/jbp261/Microcontroller-Projects/master/Images/Motor%20Control.png "Motor Encoder")

- This fixed sampling frequency was obtained by using a timer triggered interrupt. If the ‘raw’ calculated speed was directly displayed on the 7-segment display, the value would have fluctuated and difficult to read. To smooth out the speed display, the calculated speed was filtered by a 1Hz low pass FIR filter before it was sent to the 7-segment display routine.
- The quadrature signal decoding logic and the digital filter algorithm was implemented in the same timer interrupt handler routine. The frequency of this routine was set to 200Hz.

### File Description

- **Filter.c**: Implementation of band reject filter.
- **Motor_Speed_Measurement.c**:  Implementation of the motor speed control as described in the description section. 

### Results

**The Notch Filter**
- In the following graphs, the yellow signal is from the function generator, and the green signal is the filtered output from the microcontroller without analog low pass filter.

![53HZ](https://raw.githubusercontent.com/jbp261/Microcontroller-Projects/master/Images/53Hz.jpg "53HZ")
> Signal at 53 Hz, not attenuated by the notch filter.

![57 Hz](https://raw.githubusercontent.com/jbp261/Microcontroller-Projects/master/Images/57Hz.jpg "57 Hz")
> Signal at 57 Hz, attenuated by the notch filter.

![62 Hz](https://raw.githubusercontent.com/jbp261/Microcontroller-Projects/master/Images/62Hz.jpg "62 Hz")
> Signal at 62 Hz, not attenuated by the notch filter.

![Aliasing](https://raw.githubusercontent.com/jbp261/Microcontroller-Projects/master/Images/Aliasing.jpg "Aliasing")
> Instance of aliasing, signal freq. 300Hz, sampling freq. 100Hz

**Motor Speed Control**
- The motor speed control was implemented successfully as described in the description section. 

![Filter Calc](https://raw.githubusercontent.com/jbp261/Microcontroller-Projects/master/Images/FilterCalc.jpg "Filter Calc")
> The time taken by the microcontroller for filter calculations: 400 &mu;Sec
