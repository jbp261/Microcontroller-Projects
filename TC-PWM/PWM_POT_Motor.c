////////////////////////////////////////////////////////////////////////////////////// 
////	Motor Control using POT and PWM
//////////////////////////////////////////////////////////////////////////////////////

#include <asf.h>
#include <math.h>
#include <stdlib.h>
void Simple_Clk_Init(void);
void enable_port(void);
void enable_tc_clocks(void);
void enable_tc(void);
void enable_adc_clocks(void);
unsigned int read_adc(void);
void init_adc (void);
#define SIZE1 1110
volatile int sineWave[SIZE1];// Array with Sine Function values
volatile int counter = 0;
#define PI 3.14159265 
Tc *tcptr = TC4 ; 
Adc *adc_port = ADC;
volatile unsigned int cc0 = 0;
volatile unsigned int cc1 = 0;

unsigned int read_adc(void)
{
	int y;
	// start the conversion
	adc_port->SWTRIG.reg = 0x02;
	
	while(!adc_port->INTFLAG.bit.RESRDY) {}		//wait for conversion to be available
	y = adc_port->RESULT.reg;
	return(y); 					
	
}

int main (void)
{
	Simple_Clk_Init();
	enable_adc_clocks();
	init_adc();
	enable_tc_clocks();
	enable_tc();
	enable_port();
	volatile float num = 0;
	while(1)
	{
		
	// AFTER ONE DUTY CYCLE, REITERATE THE TC FUNCTION WITH DIFFERENT CC VALUE
		while(1)//tcptr->COUNT8.STATUS.reg == 0X1 << 3) // Stop bit = 1 means one duty cycle has finished
		{
			num = read_adc();
			if(num <= 30) num = 0;
			tcptr->COUNT8.CC[0].reg = (int) ( (num / 4095) * 255 );
			//cc0 = (int) ( (num / 1094) * 255 );
			tcptr->COUNT8.CC[1].reg =  (int) (255 - ((num / 4095) * 255));
			//cc1 = (int) (255 - ((num / 1094) * 255));
			//enable_tc();
		}
	}
}


/* Set correct PA13 pins as TC pins for PWM operation */
void enable_port(void)
{
	Port *ports = PORT_INSTS;
	PortGroup *porA = &(ports->Group[0]);
	
	//PA22 and WO[0]
	porA->PINCFG[22].bit.PMUXEN = 0x1;		
	porA->PMUX[11].bit.PMUXE = 0x5;			
	
	//PA23 and WO[1]
	porA->PINCFG[23].bit.PMUXEN = 0x1;		
	porA->PMUX[11].bit.PMUXO = 0x5;			
	
	//PA11 for ADC
	porA->PMUX[5].bit.PMUXO = 0x1;		//pg304 data sheet
	porA->PINCFG[11].bit.PMUXEN = 0x1;	// pg304 data sheet
	
	porA -> DIRSET.reg = PORT_PA13;
	porA -> OUTSET.reg = PORT_PA13;                                              
}

/* Perform Clock configuration to source the TC */
void enable_tc_clocks(void)
{
	PM->APBCMASK.reg |= 0x1u << 12;  	// PM_APBCMASK for TC2 is in the 10th bit position
	
	uint32_t temp= 0x15;   		// ID for TC2 is 0x14 (table 14-2)
	temp |= 0<<8;         			//  Selection Generic clock generator 0
	GCLK->CLKCTRL.reg=temp;   		//  Setup in the CLKCTRL register
	GCLK->CLKCTRL.reg |= 0x1u << 14;    	// enable it.
}

//The following function enables all the necessary bits of the registers to make the TC2 run
void enable_tc(void)
{	
	tcptr->COUNT8.CTRLA.reg |= 0x0u << 1;  //disable the TC
	
	//set up of the CTRLA register
	//Pre-scaler is set to PRESC (0x1)
	//Run in standby is off
	//GCLK_TC / 1 is used
	//Wave generation is set to NPWM
	//TC mode is set to 8 bit count
	tcptr->COUNT8.CTRLA.reg = 0x1044 ; 
	
	//The upper limit is set to 255
	tcptr->COUNT8.PER.reg = 0xFF;
	
	//PA22 or turn right
	//tcptr->COUNT8.CC[0].reg = cc0;//pg 453
	
	
	//PA23 or turn left
	//tcptr->COUNT8.CC[1].reg= cc1;
	//One shot to only let one duty cycle run
	//tcptr -> COUNT8.CTRLBSET.reg = 0x1u << 2;
	
	tcptr->COUNT8.CTRLA.reg |= 0x1u << 1; //0x1366; //0x 0001 0011 0110 0110 /*Enable TC*/
	
}

// set up generic clock for ADC
void enable_adc_clocks(void)
{
	PM->APBCMASK.reg |= 0x1u << 16; 			// PM_APBCMASK_______ is in the ___ position
	
	uint32_t temp = 0x17 ; 			// ID for ________ is__________ (see table 14-2)
	temp |= 0<<8; 					// Selection Generic clock generator 0
	GCLK->CLKCTRL.reg = temp; 			// Setup in the CLKCTRL register
	GCLK->CLKCTRL.reg |= 0x1u << 14; 		// enable it.
}

// initialize the on-board ADC system
void init_adc(void)
{
	adc_port->CTRLA.reg = 0x00;		// 1 to enable and 0 to disable		//ADC block is disabled
	
	// you will need to configure 5 registers
		adc_port->REFCTRL.reg = 0x02; // reference voltage is set to 1/2 Vddana
		adc_port->AVGCTRL.reg = 0x00; // No oversampling, 1 sample to collect
		adc_port->SAMPCTRL.reg = 0x00; //one full clock cycle
		adc_port->CTRLB.reg = 0x100;// 0x200 //12-bit resolution //ctrlb.reg
		adc_port->INPUTCTRL.reg = 0x0F001913;   //gain = 1/2 and AIN[19] is set to ADC
	
		adc_port->CTRLA.reg = 0x02;
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
