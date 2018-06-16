# Analog to Digital Conversion - Digital to Analog Conversion (ADC - DAC) Projects

### Abstract

- For ADC project, a voltage meter with the use of the SAMD20‟s ADC and the 7-segments display was built. This voltage meter was used in measuring the voltage from the on-board potentiometer (POT). 
- For DAC project, a 2KHz ±5% tone was generated that could be heard from the speaker. The DAC project required the use of the SAMD20‟s DAC and the onboard amplifier/speaker.

### Equipments
- SAM D20 Microcontroller
	- DAC/ADC Peripherals 
- The Extension Board
	- POT
	- Speaker
	- Four Multiplexed Seven Segment LEDs

### Detailed Description

**DAC**
- The analog voltage at PORT_PA11 and display its value on the 7-segment display. 
	- 12-bit Resolution
	- Seven Segment display in Volts (display up to at least two decimal places)
	- The Voltage Range read from the pot should be from 0V to 3.3V

**ADC**
- Using the ADC, display the waveform on an Oscilloscope and ‘tune’ your program so that the frequency of the sine wave is within 5% of 2KHz.
- Also, pass the sinusoid to the onboard speaker.

![ADC and DAC schematic (Extension Board)](https://raw.githubusercontent.com/jbp261/Microcontroller-Projects/master/Images/ADC-DAC%20Sch.png "ADC and DAC schematic (Extension Board)")
> ADC-DAC schematic for the extension board.

### File Description

- **ADC.c**: Implementation of the ADC project specified in the description section
- **DAC.c**:  Implementation of the DAC project specified in the description section

### Results
- The DAC project was implemented successfully as per the requirements are shown in the description section above. 

- The output from the DAC port through an analog low pass filter is shown below through an oscilloscope. 

![Oscilloscope result DAC](https://raw.githubusercontent.com/jbp261/Microcontroller-Projects/master/Images/DAC-Result.png "Oscilloscope result DAC")
