////////////////////////////////////////////////////////////////////////////////////// 
////Motor Speed Control using POT along with the display of the motor speed using LPF
//////////////////////////////////////////////////////////////////////////////////////

#include <asf.h>
#include <math.h>
#include <stdlib.h>
void Simple_Clk_Init(void);
void enable_port(void);
void enable_tc_clocks(void);
void enable_tc_clocks_2(void);
void enable_tc(void);
void enable_adc_clocks(void);
unsigned int read_adc(void);
void init_adc (void);
//void EIC_Handler(void);
void dis_all_leds(void);
void config_EIC(void);
Eic *EICptr = EIC;	/* Global declaration of External Interrupt Controller Pointer */ 
void wait(int t); // Wait function prototype
void clear_arr (volatile char *arr);
void displayC (char disNum); 
volatile char dis_arr[4];
volatile float x = 0;
volatile float c_x = 0;
volatile int prev = 0;
gcvtf(float,int,char*);
Dac *dac_port = DAC;//initialize the DAC pointer
Tc *tcptr = TC5 ; 
Tc *tcptr4 = TC4 ; 
Adc *adc_port = ADC;
volatile float yy=0, yy1=0, yy2=0, uu=0, uu1=0, uu2=0, uu3=0;
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
	enable_tc_clocks();
	enable_tc_clocks_2();
	enable_port();
	init_adc();
	enable_tc();
	
	config_EIC();


	clear_arr(dis_arr);
	
	volatile float num = 0;
	
	while(1)
	{
		num = read_adc();
		if(num <= 50) num = 0;
		tcptr4->COUNT8.CC[0].reg = (int) ( (num / 4095) * 255 ); // Scaled ADC output value
		tcptr4->COUNT8.CC[1].reg =  (int) (255 - ((num / 4095) * 255)); // 255 - Scaled ADC output value		
		clear_arr(dis_arr);
		gcvtf(uu,4,dis_arr);//itoa(xx,dis_arr,4);
		dis_all_leds();

	}
}


void TC5_Handler (void) {
	float data = 0;

	
	float n = abs(c_x);
	
	uu = (int)  ((n * 200 * 60 ) / 400); 
	
	//The following equation filters the input value uu
	yy = (int) (((0.9691 * yy1) +  (0.03094 * uu1))); 

	yy1 = yy;
	uu1 = uu;
	
	c_x = 0;// Resetting the counter
	
	tcptr->COUNT8.INTFLAG.reg = 0x01;

}


/* Set correct PA13 pins as TC pins for PWM operation */
void enable_port(void)
{
	NVIC-> ISER[0] = 1 << 18; // For interrupt for TC5
	NVIC->ISER[0] =  1 << 04; // For interrupt for EIC
	Port *ports = PORT_INSTS;
	PortGroup *porA = &(ports->Group[0]);
	PortGroup *porB = &(ports->Group[1]);
	
	//PA11 for ADC
	porA->PMUX[6].bit.PMUXO = 0x1;
	porA->PINCFG[13].bit.PMUXEN = 0x1;
	
	//PA28 for rising edge  EXTINT[8]
	porA->PMUX[14].bit.PMUXE = 0x0;		//pg304 data sheet
	porA->PINCFG[28].bit.PMUXEN = 0x1;	//pg304 data sheet
	
	//PB14 for rising edge   EXTINT[14]
	porB->PMUX[7].bit.PMUXE = 0x0;		//pg304 data sheet
	porB->PINCFG[14].bit.PMUXEN = 0x1;	//pg304 data sheet
	
	//PA02 DAC
	porA->PINCFG[2].bit.PMUXEN = 0x1;		
	porA->PMUX[1].bit.PMUXE = 0x1;			
	//PA22 and WO[0]
	porA->PINCFG[22].bit.PMUXEN = 0x1;		
	porA->PMUX[11].bit.PMUXE = 0x5;			
	
	//PA23 and WO[1]
	porA->PINCFG[23].bit.PMUXEN = 0x1;		
	porA->PMUX[11].bit.PMUXO = 0x5;			
	
	porA -> DIRSET.reg = PORT_PA04 | PORT_PA05 |PORT_PA06 |PORT_PA07;
	
	porA -> DIRSET.reg = PORT_PA13;
	porA -> OUTSET.reg = PORT_PA13;
}

// DAC  PA02                         


void config_EIC (void)
{
	PM->APBAMASK.reg |= 0x1u << 6; 			// PM_APBAMASK for EIC is in the 06-bit position
	uint32_t temp = 0x03 ;   		// ID for EIC is 0x03 ( table 14-2)
	temp |= 0<<8;         				//  Selection Generic clock generator 0
	GCLK->CLKCTRL.reg=temp;   			//  Setup in the CLKCTRL register
	GCLK->CLKCTRL.reg |= 0x1u << 14;    // enable it.
	
	//EIC setup
	EICptr->CTRL.reg = 0x0;
	
	EICptr->INTENSET.reg = 0x1u << 8; 
	EICptr ->CONFIG[1].bit.SENSE0 = 0x1; 	//the triggering of EXTINT[8] is set to rising edge
	EICptr->CTRL.reg = 0x2;
}


void EIC_Handler(void)
{
	
	Port *ports = PORT_INSTS;
	PortGroup *portAs = &(ports->Group[0]);
	PortGroup *portBs = &(ports->Group[1]);
	
	if(EICptr -> INTFLAG.reg & (0x1 << 8))
	{	
		
		if(portBs->IN.reg & PORT_PB14)
		{
			c_x++;
		}
		if(!(portBs->IN.reg & PORT_PB14))
		{
			c_x--;
		}
	
	}
	
	EICptr->INTFLAG.reg =  0x1 << 8;	/* Clear external interrupt flags for EXTINT[8] and EXTINT[14] */
	


}

void configure_dac_clock(void)
{
	PM->APBCMASK.reg |= 0x1u << 18; 			// PM_APBCMASK for DAC is in the 18-bit position
	
	uint32_t temp = 0x1A; 			// ID for DAC is 0x1A  (table 14-2)
	temp |= 0<<8;					// Selection Generic clock generator 0
	GCLK->CLKCTRL.reg = temp; 			// Setup in the CLKCTRL register
	GCLK->CLKCTRL.reg |= 0x1u << 14; 		// enable it.
}

void configure_dac(void)
{
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


//TC

/* Perform Clock configuration to source the TC */
void enable_tc_clocks(void)
{
	PM->APBCMASK.reg |= 0x1u << 13;  	// PM_APBCMASK for TC5 is in the 10th bit position
	uint32_t temp= 0x15;   		// ID for TC2 is 0x14 (table 14-2)
	temp |= 0<<8;         			//  Selection Generic clock generator 0
	GCLK->CLKCTRL.reg=temp;   		//  Setup in the CLKCTRL register
	GCLK->CLKCTRL.reg |= 0x1u << 14;    	// enable it.
	
}

void enable_tc_clocks_2(void)
{
	PM->APBCMASK.reg |= 0x1u << 12;  	// PM_APBCMASK for TC4 is in the 10th bit position
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
	//Pre-scaler is set to PRESC (0x7)
	//Run in standby is off
	//GCLK_TC / 1024 is used
	//Wave generation is set to NPWM
	//TC mode is set to 8 bit count
	tcptr->COUNT8.CTRLA.reg = 0x1744 ; 
	
	
	//The upper limit is set to 39
	tcptr->COUNT8.PER.reg = 0x27;
	
	tcptr->COUNT8.INTENSET.bit.OVF = 0x1; // Overflow Interrupt is Set Here

	
	tcptr->COUNT8.CTRLA.reg |= 0x1u << 1; /*Enable TC*/
	
	tcptr4->COUNT8.CTRLA.reg |= 0x0u << 1;  //disable the TC
	
	//set up of the CTRLA register
	//Pre-scaler is set to PRESC (0x1)
	//Run in standby is off
	//GCLK_TC / 1 is used
	//Wave generation is set to NPWM
	//TC mode is set to 8 bit count
	tcptr4->COUNT8.CTRLA.reg = 0x1044 ; 
	
	//The upper limit is set to 255
	tcptr4->COUNT8.PER.reg = 0xFF;
	
	tcptr4->COUNT8.CTRLA.reg |= 0x1u << 1; // /*Enable TC*/
	
	
}


//ADC   PA03

// set up generic clock for ADC
void enable_adc_clocks(void)
{
	PM->APBCMASK.reg |= 0x1u << 16; 			// PM_APBCMASK_ADC is in the 16 position
	
	uint32_t temp = 0x17 ; 			// ID for ADC is 17 (table 14-2)
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

void clear_arr (volatile char *arr)
{
	for (unsigned int y = 0 ; y < 3; y++)
	{
		arr[y] = 0x00;
	}
}


//time delay function
void wait(int t)
{
	volatile int count_1 = 0;
	while (count_1 < t*500)
	{
		count_1++;
	}
}

void dis_all_leds (void)
{
	//sets the base address for the Port structure to PORT_INSTS or 0x41004400
	Port *ports = PORT_INSTS;
	PortGroup *porB = &(ports->Group[1]);
	//sets the group offset for the structure PortGroup in this case it is for        group[0] or groupA
	// GroupA offset of ' '				// GroupB offset of 0x80
	PortGroup *porA = &(ports->Group[0]);
	int i =0;
	wait(1);
	
	//porA -> OUTSET.reg = PORT_PA04 | PORT_PA05 |PORT_PA06 |PORT_PA07 ;
	porA -> OUTCLR.reg = PORT_PA07; // Enable register 7
	displayC(dis_arr[i]); // Display the character keypad_char[0] on the rightmost LED on SAMD20
	wait(1);
	
	

	porA -> OUTSET.reg = PORT_PA04 | PORT_PA05 |PORT_PA06 |PORT_PA07 ;
	porA -> OUTCLR.reg = PORT_PA06; // Enable register 6
	displayC (dis_arr[i+1]); // Display the character keypad_char[1] on the second from rightmost LED on SAMD20
	wait(1);
	
	porA -> OUTSET.reg = PORT_PA04 | PORT_PA05 |PORT_PA06 |PORT_PA07 ;
	porA -> OUTCLR.reg = PORT_PA05; // Enable register 5
	displayC (dis_arr[i+2]); // Display the character keypad_char[2] on the third from rightmost LED on SAMD20
	wait(1);
	


	porA -> OUTSET.reg = PORT_PA04 | PORT_PA05 |PORT_PA06 |PORT_PA07 ;
	porA -> OUTCLR.reg = PORT_PA04; // Enable register 4
	displayC(dis_arr[i+3]); // Display the character keypad_char[3] on the leftmost LED on SAMD20
	wait(1);
	
	porA -> OUTSET.reg = PORT_PA04 | PORT_PA05 |PORT_PA06 |PORT_PA07 ;
	wait(1);
	porB -> OUTSET.reg = 0xFF;
	wait(1);
}


void displayC (volatile char disNum)
{
	Port *ports = PORT_INSTS;
	PortGroup *porB = &(ports->Group[1]);
	porB ->DIRSET.reg = 0xFF; // Set all port B register as output
	
	porB ->DIRSET.reg = PORT_PB09 | PORT_PB07;
	porB -> OUTSET.reg = 0xFF;
	porB -> OUTSET.reg = PORT_PB09 | PORT_PB07;
	

	 if ( disNum == '0')
	{
		porB -> OUTSET.reg = PORT_PB06|PORT_PB07;
		porB -> OUTCLR.reg = PORT_PB00|PORT_PB01|PORT_PB02|PORT_PB03|PORT_PB04|PORT_PB05;
		wait(2);
	}
	else if( disNum == '1')
	{
		porB -> OUTSET.reg = PORT_PB00|PORT_PB03|PORT_PB04|PORT_PB05|PORT_PB06|PORT_PB07;
		porB -> OUTCLR.reg = PORT_PB01|PORT_PB02;
		wait(2);
	}
	else if( disNum == '2')
	{
		porB -> OUTSET.reg = PORT_PB02|PORT_PB05| PORT_PB07;
		porB -> OUTCLR.reg = PORT_PB00|PORT_PB01|PORT_PB03|PORT_PB04|PORT_PB06;
		wait(2);
	}
	else if( disNum == '3')
	{
		porB -> OUTSET.reg = PORT_PB04|PORT_PB05|PORT_PB07;
		porB -> OUTCLR.reg = PORT_PB00|PORT_PB01|PORT_PB02|PORT_PB03|PORT_PB06;
		wait(2);
	}
	else if( disNum == '4')
	{
		porB -> OUTSET.reg = PORT_PB00|PORT_PB03|PORT_PB04|PORT_PB07;
		porB -> OUTCLR.reg = PORT_PB01|PORT_PB02|PORT_PB05|PORT_PB06;
		wait(2);
	}
	else if( disNum == '5')
	{
		porB -> OUTSET.reg = PORT_PB01|PORT_PB04|PORT_PB07;
		porB -> OUTCLR.reg = PORT_PB00|PORT_PB02|PORT_PB03|PORT_PB05|PORT_PB06;
		wait(2);
	}
	else if (disNum == '6')
	{
		porB -> OUTSET.reg = PORT_PB01|PORT_PB07;
		porB -> OUTCLR.reg = PORT_PB00|PORT_PB02|PORT_PB03|PORT_PB04|PORT_PB05|PORT_PB06;
		wait(2);
	}
	else if (disNum == '7')
	{
		porB -> OUTSET.reg = PORT_PB06|PORT_PB03|PORT_PB04|PORT_PB05|PORT_PB07;
		porB -> OUTCLR.reg = PORT_PB00|PORT_PB01|PORT_PB02;
		wait(2);
	}
	else if (disNum == '8')
	{
		
		porB -> OUTSET.reg = PORT_PB07;
		porB -> OUTCLR.reg = PORT_PB00|PORT_PB01|PORT_PB02|PORT_PB03|PORT_PB04|PORT_PB05|PORT_PB06;
		wait(2);
	}
	else if (disNum == '9')
	{
		porB -> OUTSET.reg = PORT_PB04|PORT_PB07;
		porB -> OUTCLR.reg = PORT_PB00|PORT_PB01|PORT_PB02|PORT_PB03|PORT_PB05|PORT_PB06;
		wait(2);
	}
	else if (disNum == 'a' || disNum == 'A')
	{
		porB -> OUTSET.reg = PORT_PB03|PORT_PB07;
		porB -> OUTCLR.reg = PORT_PB00|PORT_PB01|PORT_PB02|PORT_PB04|PORT_PB05|PORT_PB06;
		wait(2);
	}
	else if (disNum == 'b' || disNum == 'B')
	{
		porB -> OUTSET.reg = PORT_PB00|PORT_PB01|PORT_PB07;
		porB -> OUTCLR.reg = PORT_PB03|PORT_PB02|PORT_PB04|PORT_PB05|PORT_PB06;
		wait(2);
	}
	else if (disNum == 'c' || disNum == 'C')
	{
		porB -> OUTSET.reg = PORT_PB01|PORT_PB02|PORT_PB06|PORT_PB07;
		porB -> OUTCLR.reg = PORT_PB03|PORT_PB00|PORT_PB04|PORT_PB05;
		wait(2);
	}
	else if (disNum == 'd' || disNum == 'D')
	{
		porB -> OUTSET.reg = PORT_PB05 | PORT_PB00|PORT_PB07;
		porB -> OUTCLR.reg = PORT_PB03|PORT_PB01|PORT_PB02|PORT_PB04|PORT_PB06;
		wait(2);
	}
	else
	{
		porB -> OUTSET.reg = PORT_PB08|PORT_PB00|PORT_PB01|PORT_PB02|PORT_PB03|PORT_PB04|PORT_PB05|PORT_PB07;
		porB -> OUTCLR.reg = PORT_PB06;
		wait(2);
	}

	
}