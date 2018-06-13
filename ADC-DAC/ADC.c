////////////////////////////////////////////////////////////////////////////////////// 
////		Analog To Digital Converter 
//////////////////////////////////////////////////////////////////////////////////////

#include <asf.h>
#include <stdlib.h>

void Simple_Clk_Init(void);
void enable_adc_clocks(void);
void init_adc(void);
void dis_all_leds (void);
PortGroup *porta = (PortGroup *)PORT;
Adc *adc_port = ADC;// a pointer to the ADC block
void wait(int t); // Wait function prototype
void clear_arr (volatile char *arr);
void displayC (char disNum); 
volatile char dis_arr[4];
volatile float x = 0;
gcvtf(float,int,char*);
 
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
	Port *ports = PORT_INSTS;
	PortGroup *porA = &(ports->Group[0]);
	porA -> DIRSET.reg = PORT_PA04 | PORT_PA05 |PORT_PA06 |PORT_PA07;
	porA -> DIRSET.reg = PORT_PA13;
	porA -> OUTSET.reg = PORT_PA13;
	clear_arr(dis_arr);
	
	while(1)
	{
			x = (float) (read_adc());			
			clear_arr(dis_arr);
			gcvtf(x,4,dis_arr);//itoa(x,dis_arr,4);
			dis_all_leds();
			wait(3);
	}
}

// set up generic clock for ADC
void enable_adc_clocks(void)
{
	PM->APBCMASK.reg |= 0x1u << 16; 			
	
	uint32_t temp = 0x17 ; 			
	temp |= 0<<8; 					
	GCLK->CLKCTRL.reg = temp; 			
	GCLK->CLKCTRL.reg |= 0x1u << 14; 		
}

// initialize the on-board ADC system 
void init_adc(void)
{
	adc_port->CTRLA.reg = 0x00;		// 1 to enable and 0 to disable
	
	
		adc_port->REFCTRL.reg = 0x02; // reference voltage is set to 1/2 Vddana
		adc_port->AVGCTRL.reg = 0x00; // No oversampling, 1 sample to collect
		adc_port->SAMPCTRL.reg = 0x00; //one full clock cycle
		adc_port->CTRLB.reg = 0x200; //12-bit resolution //ctrlb.reg
		adc_port->INPUTCTRL.reg = 0x0F001813;   //gain = 1/2 and AIN[19] is set to ADC
	
	// config PA11 to be owned by ADC Peripheral
	
		porta->PMUX[5].bit.PMUXO = 0x1;		//pg304 data sheet
		porta->PINCFG[11].bit.PMUXEN = 0x1;	//pg304 data sheet
	
	adc_port->CTRLA.reg = 0x02;				//Enable ADC	
}
	
//Simple Clock Initialization
void Simple_Clk_Init(void)
{
	/* Various bits in the INTFLAG register can be set to one at startup.
	   This will ensure that these bits are cleared */
	
	SYSCTRL->INTFLAG.reg = SYSCTRL_INTFLAG_BOD33RDY | SYSCTRL_INTFLAG_BOD33DET |
			SYSCTRL_INTFLAG_DFLLRDY;
			
	system_flash_set_waitstates(0);  	//Clock_flash wait state =0

	SYSCTRL_OSC8M_Type temp = SYSCTRL->OSC8M;      /* for OSC8M initialization  */

	temp.bit.PRESC    = 0;    		// no divide, i.e., set clock=8Mhz  (see page 170)
	temp.bit.ONDEMAND = 1;    		// On-demand is true
	temp.bit.RUNSTDBY = 0;    		// Standby is false
	
	SYSCTRL->OSC8M = temp;

	SYSCTRL->OSC8M.reg |= 0x1u << 1;  	// SYSCTRL_OSC8M_ENABLE bit = bit-1 (page 170)
	
	PM->CPUSEL.reg = (uint32_t)0;    	// CPU and BUS clocks Divide by 1  (see page 110)
	PM->APBASEL.reg = (uint32_t)0;     	// APBA clock 0= Divide by 1  (see page 110)
	PM->APBBSEL.reg = (uint32_t)0;     	// APBB clock 0= Divide by 1  (see page 110)
	PM->APBCSEL.reg = (uint32_t)0;     	// APBB clock 0= Divide by 1  (see page 110)

	PM->APBAMASK.reg |= 01u<<3;   		// Enable Generic clock controller clock (page 127)

	/* Software reset Generic clock to ensure it is re-initialized correctly */

	GCLK->CTRL.reg = 0x1u << 0;   		// Reset gen. clock (see page 94)
	while (GCLK->CTRL.reg & 0x1u ) {  /* Wait for reset to complete */ }
	
	// Initialization and enable generic clock #0

	*((uint8_t*)&GCLK->GENDIV.reg) = 0;  	// Select GCLK0 (page 104, Table 14-10)

	GCLK->GENDIV.reg  = 0x0100;   		// Divide by 1 for GCLK #0 (page 104)

	GCLK->GENCTRL.reg = 0x030600;  		// GCLK#0 enable, Source=6(OSC8M), IDC=1 (page 101)
}

void clear_arr (volatile char *arr)
{
	for (unsigned int y = 0 ; y < 4; y++)
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
	
	//sets the group offset for the structure PortGroup in this case it is for        group[0] or groupA
	// GroupA offset of ' '				// GroupB offset of 0x80
	PortGroup *porA = &(ports->Group[0]);
	int i =0;
	
	
	porA -> OUTSET.reg = PORT_PA04 | PORT_PA05 |PORT_PA06 |PORT_PA07 ;
	porA -> OUTCLR.reg = PORT_PA07; // Enable register 7
	displayC(dis_arr[i]); // Display the character keypad_char[0] on the rightmost LED on SAMD20
	wait(2);
	

	porA -> OUTSET.reg = PORT_PA04 | PORT_PA05 |PORT_PA06 |PORT_PA07 ;
	porA -> OUTCLR.reg = PORT_PA06; // Enable register 6
	displayC (dis_arr[i+1]); // Display the character keypad_char[1] on the second from rightmost LED on SAMD20
	wait(2);


	porA -> OUTSET.reg = PORT_PA04 | PORT_PA05 |PORT_PA06 |PORT_PA07 ;
	porA -> OUTCLR.reg = PORT_PA05; // Enable register 5
	displayC (dis_arr[i+2]); // Display the character keypad_char[2] on the third from rightmost LED on SAMD20
	wait(2);
	porA -> OUTSET.reg = PORT_PA04 | PORT_PA05 |PORT_PA06 |PORT_PA07 ;
	porA -> OUTCLR.reg = PORT_PA04; // Enable register 4
	displayC(dis_arr[i+3]); // Display the character keypad_char[3] on the leftmost LED on SAMD20
	wait(2);
	
	porA -> OUTSET.reg = PORT_PA04;
	//porA -> OUTSET.reg = PORT_PA04 | PORT_PA05 |PORT_PA06 |PORT_PA07 ;
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
	
	if ( disNum == '-') //LED Light!
	{
		porB -> OUTSET.reg = PORT_PB00|PORT_PB01|PORT_PB02|PORT_PB03|PORT_PB04|PORT_PB05|PORT_PB06|PORT_PB07;
		porB -> OUTCLR.reg = PORT_PB09;
		wait(2);
	}
	else if ( disNum == '.')
	{
		porB -> OUTSET.reg = PORT_PB00|PORT_PB01|PORT_PB02|PORT_PB03|PORT_PB04|PORT_PB05|PORT_PB06;
		porB -> OUTCLR.reg = PORT_PB07;
		wait(2);
	}
	else if ( disNum == '0')
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

	porB -> OUTSET.reg = 0xFF;
	wait(1);
}