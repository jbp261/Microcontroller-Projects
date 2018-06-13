///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// This project shows how to perform Port Control. 
// It demonstrates how to set a pin as an input as well as output pin. 
// We toggle the LED on the SAMD20 whenever the button SW0 on the SAMD20 is pressed.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//header files for all drivers
#include <asf.h>

//void functions
void wait(int t);
void Simple_Clk_Init(void);

//global variables
volatile int count = 0;

int main (void)
{
	//set micro-controller clock to 8Mhz
	Simple_Clk_Init();

	//sets the base address for the Port structure to PORT_INSTS or 0x41004400
	//Port *ports = PORT_INSTS;
	
	//sets the group offset for the structure PortGroup in this case it is for group[0] or groupA
	// GroupA offset of 0x00				// GroupB offset of 0x80
	//PortGroup *porA = &(ports->Group[0]);
	
	// Set the direction of LED0 pin to be output
	// PORT_PAxx is defined as (1u << x) so in this case PORT_PA14 = (1u << 14)
	//int *pntToDirSet = 0x4100440;
	* ((unsigned int *) 0x41000440) = 1<<14;
	//dirset - output
	
	//pntToDirSet = 1<<14;
	
	//int *outSet = 0x41004418;
	//int *outClr = 0x41004414;

	// Set the direction of SW0 pin to be input
	//porA->DIRCLR.reg = PORT_PA15;
	//porA->OUTSET.reg = PORT_PA15;

	//Perform other configurations for SW0 - Do input enable and pull-up
	//porA->PINCFG[15].reg = PORT_PINCFG_INEN | PORT_PINCFG_PULLEN;

	while (1){
		//check if button was pressed
		//if(( porA->IN.reg & PORT_PA15))
		//{
			// if button was pressed toggle LED
			//pnt->OUTSET.reg = PORT_PA14;
			//*(pntToDirSet + 0x18) = 1<<14;
			* ((unsigned int *) 0x41004418) = 1<<14;
			
			//pa14 is output
			
			wait(100);
			//pnt->OUTCLR.reg = PORT_PA14;
			//*(pntToDirSet + 0x14) = 1<<14;
			* ((unsigned int *) 0x41004414) = 1<<14;
			//do nothing
			
			//wait
			wait(100);
			
		//}
	}
}

//time delay function
void wait(int t)
{
	count = 0;
    while (count < t*1000)
	{
		count++;
	}
}

//Simple Clock Initialization - Do Not Modify -//
void Simple_Clk_Init(void)
{
	/* Various bits in the INTFLAG register can be set to one at startup.
	   This will ensure that these bits are cleared */
	
	SYSCTRL->INTFLAG.reg = SYSCTRL_INTFLAG_BOD33RDY | SYSCTRL_INTFLAG_BOD33DET |
			SYSCTRL_INTFLAG_DFLLRDY;
			
	system_flash_set_waitstates(0);  		//Clock_flash wait state =0

	SYSCTRL_OSC8M_Type temp = SYSCTRL->OSC8M;      	/* for OSC8M initialization  */

	temp.bit.PRESC    = 0;    			// no divide, i.e., set clock=8Mhz  (see page 170)
	temp.bit.ONDEMAND = 1;    			// On-demand is true
	temp.bit.RUNSTDBY = 0;    			// Standby is false
	
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
