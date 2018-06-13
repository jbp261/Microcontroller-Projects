////////////////////////////////////////////////////////////////////////////////////// 
////		Digital To Analog Converter 
//////////////////////////////////////////////////////////////////////////////////////

#include <asf.h>
#include <math.h>
#include <stdlib.h>
void Simple_Clk_Init(void);
void configure_dac(void);
void configure_dac_clock(void);
void createSineWave(); 
Dac *dac_port = DAC;//initialize the DAC pointer
volatile int sineWave[888];// Array with Sine Function values
#define PI 3.14159265 
#define FREQ 2000


int main(void)
{
	Simple_Clk_Init();// System Clock
	configure_dac_clock();// DAC Clock Setup
	configure_dac();//DAC Registers and Port Setup
	createSineWave();//Fills up the sineWave variable with sine function values
	int i = 0;
	double data = 0;
	int data_i = 0;
	while (1) {

	while (dac_port->STATUS.reg & DAC_STATUS_SYNCBUSY) {}// Wait until DAC is not busy
		
			data = (sineWave[i]); // Read a value of Sine Function
			dac_port->DATA.reg = (int) data; // Add the Sine function value of 10-bit to the DATA reg.
			i++;
			if (i == 888) i = 0; // Reset the counter
	}
}


//Definition of CreateSinewave()
//The following function creates a sine function values and populates the sineWave array
//All the Sine function values are scaled within 0 and 1023
void createSineWave()
{
	
	for(int i = 0; i < 888; i++)
	{
		double x = 2*PI*i/148;
		
		sineWave[i] = (int) (0.50 * (1024 + 1023 * sin(x))); 
	}
	
}
void configure_dac_clock(void)
{
	PM->APBCMASK.reg |= 0x1u << 18; 			// PM_APBCMASK for DAC is in the 18-bit position
	
	uint32_t temp = 0x1A; 			// ID for DAC is 0x1A (see table 14-2)
	temp |= 0<<8;					// Selection Generic clock generator 0
	GCLK->CLKCTRL.reg = temp; 			// Setup in the CLKCTRL register
	GCLK->CLKCTRL.reg |= 0x1u << 14; 		// enable it.
}

void configure_dac(void)
{
	//set pin as output for the dac
	Port *ports = PORT_INSTS;
	PortGroup *por = &(ports->Group[0]);
	
	por->PINCFG[1].bit.PMUXEN = 0x1;		// set to correct pin configuration
	por->PMUX[2].bit.PMUXE = 0x1;			// set to correct peripheral


	while (/*DAC pointer*/dac_port->STATUS.reg & DAC_STATUS_SYNCBUSY) {
		/* Wait until the synchronization is complete */
	}

	dac_port->CTRLB.reg = 0x40;/* Set reference voltage to 3.3V with CTRLB */


	while (/*DAC pointer*/dac_port->STATUS.reg & DAC_STATUS_SYNCBUSY) {
		/* Wait until the synchronization is complete */
	}

	dac_port->CTRLA.reg = 0x02; /* Enable the module with CTRLA */

	dac_port->CTRLB.reg = 0x41;/* Enable selected output with CTRLB*/

}

//Simple Clock Initialization
void Simple_Clk_Init(void)
{
	/* Various bits in the INTFLAG register can be set to one at startup.
	   This will ensure that these bits are cleared */
	
	SYSCTRL->INTFLAG.reg = SYSCTRL_INTFLAG_BOD33RDY | SYSCTRL_INTFLAG_BOD33DET |
			SYSCTRL_INTFLAG_DFLLRDY;
			
	system_flash_set_waitstates(0);  		//Clock_flash wait state = 0

	SYSCTRL_OSC8M_Type temp = SYSCTRL->OSC8M;      	/* for OSC8M initialization  */

	temp.bit.PRESC    = 0;    			// no divide, i.e., set clock=8Mhz  (see page 170)
	temp.bit.ONDEMAND = 1;    			//  On-demand is true
	temp.bit.RUNSTDBY = 0;    			//  Standby is false
	
	SYSCTRL->OSC8M = temp;

	SYSCTRL->OSC8M.reg |= 0x1u << 1;  		// SYSCTRL_OSC8M_ENABLE bit = bit-1 (page 170)
	
	PM->CPUSEL.reg = (uint32_t)0;    		// CPU and BUS clocks Divide by 1  (see page 110)
	PM->APBASEL.reg = (uint32_t)0;     		// APBA clock 0= Divide by 1  (see page 110)
	PM->APBBSEL.reg = (uint32_t)0;     		// APBB clock 0= Divide by 1  (see page 110)
	PM->APBCSEL.reg = (uint32_t)0;     		// APBB clock 0= Divide by 1  (see page 110)

	PM->APBAMASK.reg |= 01u<<3;   			// Enable Generic clock controller clock (page 127)

	/* Software reset Generic clock to ensure it is re-initialized correctly */

	GCLK->CTRL.reg = 0x1u << 0;   			// Reset gen. clock (see page 94)
	while (GCLK->CTRL.reg & 0x1u ) {  /* Wait for reset to complete */ }
	
	// Initialization and enable generic clock #0

	*((uint8_t*)&GCLK->GENDIV.reg) = 0;  		// Select GCLK0 (page 104, Table 14-10)

	GCLK->GENDIV.reg  = 0x0100;   		 	// Divide by 1 for GCLK #0 (page 104)

	GCLK->GENCTRL.reg = 0x030600;  		 	// GCLK#0 enable, Source=6(OSC8M), IDC=1 (page 101)
}
