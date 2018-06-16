# Timer Counter/ Pulse Width Modulation (TC-PWM) Projects

### Abstract

The project contains the implementation of the function of a DAC by using a PWM output and a filter and control the speed of a motor using PWM and the ADC code from ADC lab to read the position of the POT.

### Equipments
- SAM D20 Microcontroller
	- DAC/ADC, PWM, TC Peripherals 
- The Extension Board
	- POT
	- Four Multiplexed Seven Segment LEDs
- H-bridge Motor

### Additional Environment
- Matlab for filter design

### Detailed Description

**PWM**
- In PWM frequency generation mode, for the 8-bit counter, is determined by the Period register; while the Compare/Capture Value registers (CCx) are used to determine the duty cycle of the waveform. This allows for a variable duty cycle and a variable frequency for the 8-bit counter. In a typical application, the content of the CC register is changed to vary the output effective voltage while the frequency of the PWM, once it is set, stays constant. 
- Using the fact described above, the pot output is used to control the duty cycle. The output of the PWM is then fed to the H-bridge motor. If the pot is turned right, the duty cycle increases and the motor runs fast and vice versa.
- The PWM output pass through RC filter formed by the 0.1uF and 10k resistor.
- The following figure shows the motor driver circuit for H-bridge motor. 


![H-bridge motor schematic](https://raw.githubusercontent.com/jbp261/Microcontroller-Projects/master/Images/H-bridgeMototSch.png "H-bridge motor schematic")


**TC**
- A sine wave of about 500Hz (+/- 50Hz) was generated using timer counter. 

### File Description

- **PWM_POT_Motor.c**: Implementation of the ADC project specified in the description section
- **PWM_SineWave.c**:  Implementation of the DAC project specified in the description section

### Results
- The DAC project was implemented successfully as per the requirements are shown in the description section above. 

- The output from the DAC port through an analog low pass filter is shown below through an oscilloscope. 

![Oscilloscope result DAC](https://raw.githubusercontent.com/jbp261/Microcontroller-Projects/master/Images/DAC-Result.png "Oscilloscope result DAC")



