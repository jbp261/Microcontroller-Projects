////////////////////////////////////////////////////////////////////////////////////// 
////	PWM SineWave Generation
//////////////////////////////////////////////////////////////////////////////////////

#include <asf.h>
#include <math.h>
#include <stdlib.h>
void Simple_Clk_Init(void);
void enable_port(void);
void enable_tc_clocks(void);
void enable_tc(void);
void createSineWave();
#define SIZE1 1110
volatile int sineWave[SIZE1];// Array with Sine Function values
volatile int counter = 0;
#define PI 3.14159265 
Tc *tcptr = TC2 ;  

int main (void)
{
	Simple_Clk_Init();
	createSineWave(); // generate the sine wave data
	enable_tc();
	
	while(1)
	{
		
	// AFTER ONE DUTY CYCLE, REITERATE THE TC FUNCTION WITH DIFFERENT CC VALUE
		while(tcptr->COUNT8.STATUS.reg == 0X1 << 3) // Stop bit = 1 means one duty cycle has finished
		{
			enable_tc();
		}
	}
}

//The following sineWave function generates data for 500 Hz frequency
void createSineWave()
{	
	for(int i = 0; i < SIZE1; i++)
	{
		double x = 2*PI*i/(37);      
		
		sineWave[i] = (int) ( (127 + 127 * sin(x))); 
	}
}
/* Set correct PA13 pins as TC pins for PWM operation */
void enable_port(void)
{
	Port *ports = PORT_INSTS;
	PortGroup *porA = &(ports->Group[0]);
	
	porA->PINCFG[13].bit.PMUXEN = 0x1;		configuration
	porA->PMUX[6].bit.PMUXO = 0x4;			
}

/* Perform Clock configuration to source the TC */
void enable_tc_clocks(void)
{
	PM->APBCMASK.reg |= 0x1u << 10;  	// PM_APBCMASK for TC2 is in the 10th bit position
	
	uint32_t temp= 0x14;   		// ID for TC2 is 0x14 (table 14-2)
	temp |= 0<<8;         			//  Selection Generic clock generator 0
	GCLK->CLKCTRL.reg=temp;   		//  Setup of the CLKCTRL register
	GCLK->CLKCTRL.reg |= 0x1u << 14;    	// enable it.
}

//The following function enables all the necessary bits of the registers to make the TC2 run
void enable_tc(void)
{
	enable_port();
	enable_tc_clocks();
	
	tcptr->COUNT8.CTRLA.reg |= 0x0u << 1;  //disable the TC
	
	//set up of the CTRLA register
	//Prescaler is set to PRESC (0x1)
	//Run in standby is off
	//GCLK_TC / 1 is used
	//Wave generation is set to NPWM
	//TC mode is set to 8 bit count
	tcptr->COUNT8.CTRLA.reg = 0x1044 ; 
	
	//The upper limit is set to 255
	tcptr->COUNT8.PER.reg = 0xFF;
	
	//The following line of code changes the CC[1] values
	tcptr->COUNT8.CC[1].reg = sineWave[counter++];//pg 453
	if(counter == SIZE1) counter = 0;
	
	//One shot to only let one duty cycle run
	tcptr -> COUNT8.CTRLBSET.reg = 0x1u << 2;
	
	tcptr->COUNT8.CTRLA.reg |= 0x1u << 1; //0x1366; //0x 0001 0011 0110 0110 /*Enable TC*/
	
}

//Simple Clock Initialization
void Simple_Clk_Init(void)
{
	/* Various bits in the INTFLAG register can be set to one at startup.
	   This will ensure that these bits are cleared */
	
	SYSCTRL->INTFLAG.reg = SYSCTRL_INTFLAG_BOD33RDY | SYSCTRL_INTFLAG_BOD33DET |
			SYSCTRL_INTFLAG_DFLLRDY;
			
	system_flash_set_waitstates(0);  		//Clock_flash wait state =0

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
