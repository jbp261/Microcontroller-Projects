////////////////////////////////////////////////////////////////////////////////////// 
////	Motor Speed & Direction Control through KeyPad Entry 
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
void enable_tc_clocks_3(void);
unsigned int read_adc(void);
void init_adc (void);
//void EIC_Handler(void);
void dis_all_leds(void);
void config_EIC(void);
void wait(int t); // Wait function prototype
void clear_arr (volatile char *arr);
void displayC (volatile char disNum);
Eic *EICptr = EIC;	/* Global declaration of External Interrupt Controller Pointer */ 
volatile char dis_arr[4];
volatile float x = 0;
volatile float c_x = 0;
volatile int prev = 0;
gcvtf(float,int,char*);
char sense_key(void);
Dac *dac_port = DAC;//initialize the DAC pointer
Tc *tcptr = TC5 ; 
Tc *tcptr4 = TC4 ; 
Tc *tcptr3 = TC3 ;
Adc *adc_port = ADC;
volatile char keypad_key = 0x00;
volatile float yy=0, yy1=0, yy2=0, uu=0, uu1=0, uu2=0, uu3=0;
volatile int counter1 = 0;
volatile unsigned int cc0 = 0, state1 = 0;
volatile unsigned int cc1 = 0;
volatile float num = 0;
volatile float Kp = 0.015 ,Ki = 0.9 ; 
volatile float Kpp = 0.009 ,Kii = 0.54 ;
volatile float int_err = 0;
volatile char temp1 = 0x00, temp2 = 0x00;
volatile char operation = 0;
volatile int state = 0, count = 0;
volatile float maintain = 0;
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
	enable_tc_clocks();
	enable_tc_clocks_2();
	enable_tc_clocks_3();
	enable_port();
	enable_tc();
	config_EIC();
	clear_arr(dis_arr);
	
	while(1) {
		
	}

}

// Low Priority Handler
void TC3_Handler (void) 
{
	
	switch(state)
	{
		case 0: {
			temp1 = sense_key();
			if(temp1 == 0x00)
			break;
			counter1++;
			state =1;
			break;
		}
		
		case 1: {
			temp2 = sense_key();
			if(temp1 != temp2)
			{
				state =0;
				break;
			}
			counter1++;
			if(counter1 > 5)
			{
				state = 2;
				break;
			}
			else
			break;
		}
		
		case 2: {
			keypad_key = temp1;
			counter1 = 0;
			state = 3;
			break;
		}
		
		case 3: {
			temp1 = sense_key();
			if(temp1 != 0x00)
			break;
			counter1++;
			if(counter1 > 5)
			{
				counter1 = 0;
				state = 0;
				break;
			}
			else
			break;
		}
		
		default: break;
		
	}
	
	
		if(keypad_key == '1' ) 
		{
			num  = 0;
			state1 = 2;
			keypad_key = 4;
		}
		
		if(keypad_key == '0' )
		{
			num = 0;
			state1 = 4;
			keypad_key = 4;
		}
	
	

	//_________________________
	

	switch(state1)
	{
		case 1: {/*Ideal*/
				num = 0;
				tcptr4->COUNT8.CC[0].reg = num ; // Scaled ADC output value
				tcptr4->COUNT8.CC[1].reg = num; // 255 - Scaled ADC output value
			break;
		}
		
		case 2: {/*Accel*/
			
			num = (Kp* (1505-uu) ) + ( Ki * int_err);
			
			int_err += ((1505 - uu) * 0.0167); 
			
			num = (int) ((num / 4837) * 255);
			
			tcptr4->COUNT8.CC[0].reg = num ; // Scaled ADC output value
			tcptr4->COUNT8.CC[1].reg = 0; // 255 - Scaled ADC output value
			
			if ( uu > 1200 && uu < 1500 ){
				state = 3;
				num = 0;
			}
			break;
			}
		
		case 3: {/*Sped_ctrl*/
			
			num = (Kp* (1505-uu) ) + ( Ki * int_err);
			
			
			int_err += ((1505 - uu) * 0.0167);
			
			num = (int) ((num / 4837) * 255);
			
			tcptr4->COUNT8.CC[0].reg = num ; // Scaled ADC output value
			tcptr4->COUNT8.CC[1].reg = 0; // 255 - Scaled ADC output value
			
			break;}
		
		case 4: {/*Decel*/
				
				
				num = (Kpp* (-uu) ) + ( Kii * int_err);
				
				
				int_err += ((- uu) * 0.0167);
				
				num = (int) ((num / 4837) * 255);
				
				tcptr4->COUNT8.CC[0].reg = num ; // Scaled ADC output value
				tcptr4->COUNT8.CC[1].reg = 0; // 255 - Scaled ADC output value
			
			break;}
		
		default: break;
		
		
		}// switch 
	//_______________________
		clear_arr(dis_arr);
		gcvtf(yy,4,dis_arr);
		dis_all_leds();
}

// Medium Priority Handler
void TC5_Handler (void) {
	float data = 0;
	
	float n =  (int) abs(c_x);
	
	uu = (int)  ((n * 200 * 60 ) / 400); 
	
	//The following equation filters the input value uu
	yy = (int) (((0.9691 * yy1) +  (0.03094 * uu1))); 

	yy1 = yy;
	uu1 = uu;
	
	c_x = 0;// Resetting the counter
	tcptr->COUNT8.INTFLAG.reg = 0x01;
	
}

// High Priority Handler
void EIC_Handler(void)
{
	
	Port *ports = PORT_INSTS;
	PortGroup *portBs = &(ports->Group[1]);
	
	if(portBs->IN.reg & PORT_PB14)
	{
		c_x++;
	}
	
	else
	{
		c_x--;
	}
	
	
	EICptr->INTFLAG.reg =  0x1 << 8;	/* Clear external interrupt flags for EXTINT[8] and EXTINT[14] */
	
}

/* Set correct PA13 pins as TC pins for PWM operation */
void enable_port(void)
{
	NVIC-> ISER[0] = 1 << 16; // For interrupt for TC3
	NVIC-> ISER[0] |= 1 << 18; // For interrupt for TC5
	NVIC->ISER[0] |=  1 << 04; // For interrupt for EIC
	NVIC->IP[1] = 0x00000000;   // Highest priority for EIC 
	NVIC->IP[4] = 0x00400080;   //
	
	Port *ports = PORT_INSTS;
	PortGroup *porA = &(ports->Group[0]);
	PortGroup *porB = &(ports->Group[1]);
	
	//Set some reg of port A to input
	// The following statement clears the directory of 4,5,6,7 of port A
	porA -> DIRCLR.reg = PORT_PA16 | PORT_PA17 | PORT_PA18| PORT_PA19;
	
	// The following four lines of code sets the pull resistor on registers 16,17,18,19 of port A.

	//Enable input and pull up resistor
	porA->PINCFG[16].reg = PORT_PINCFG_INEN | PORT_PINCFG_PULLEN;
	porA->PINCFG[17].reg = PORT_PINCFG_INEN | PORT_PINCFG_PULLEN;
	porA->PINCFG[18].reg = PORT_PINCFG_INEN | PORT_PINCFG_PULLEN;
	porA->PINCFG[19].reg = PORT_PINCFG_INEN | PORT_PINCFG_PULLEN;
	
	//PA11 for ADC
	porA->PMUX[6].bit.PMUXO = 0x1;
	porA->PINCFG[13].bit.PMUXEN = 0x1;
	
	//PA28 for rising edge  EXTINT[8]
	porA->PMUX[14].bit.PMUXE = 0x0;		//refer to pg304 data sheet
	porA->PINCFG[28].bit.PMUXEN = 0x1;	//refer to pg304 data sheet
	
	//PB14 for rising edge   EXTINT[14]
	porB->PMUX[7].bit.PMUXE = 0x0;		//refer to pg304 data sheet
	porB->PINCFG[14].bit.PMUXEN = 0x1;	//refer to pg304 data sheet
	
	//PA02 DAC
	porA->PINCFG[2].bit.PMUXEN = 0x1;		// set to correct pin configuration
	porA->PMUX[1].bit.PMUXE = 0x1;			// set to correct peripheral
	
	//PA22 and WO[0]
	porA->PINCFG[22].bit.PMUXEN = 0x1;		// set to correct pin configuration
	porA->PMUX[11].bit.PMUXE = 0x5;			// set to correct peripheral
	
	//PA23 and WO[1]
	porA->PINCFG[23].bit.PMUXEN = 0x1;		// set to correct pin configuration
	porA->PMUX[11].bit.PMUXO = 0x5;			// set to correct peripheral
	
	porA -> DIRSET.reg = PORT_PA04 | PORT_PA05 |PORT_PA06 |PORT_PA07;
	
	porA -> DIRSET.reg = PORT_PA13;
	porA -> OUTSET.reg = PORT_PA13;
}

// DAC  PA02                         

void config_EIC (void)
{
	PM->APBAMASK.reg |= 0x1u << 6; 			// PM_APBAMASK for EIC is in the 06-bit position
	uint32_t temp = 0x03 ;   		// ID for EIC is 0x03 (see table 14-2)
	temp |= 0<<8;         				//  Selection Generic clock generator 0
	GCLK->CLKCTRL.reg=temp;   			//  Setup in the CLKCTRL register
	GCLK->CLKCTRL.reg |= 0x1u << 14;    // enable it.
	
	//EIC setup
	EICptr->CTRL.reg = 0x0;
	
	EICptr->INTENSET.reg = 0x1u << 8; 
	EICptr ->CONFIG[1].bit.SENSE0 = 0x1; 	//the triggering of EXTINT[8] is set to rising edge
	EICptr->CTRL.reg = 0x2;
}


void configure_dac_clock(void)
{
	PM->APBCMASK.reg |= 0x1u << 18; 			// PM_APBCMASK for DAC is in the 18-bit position
	
	uint32_t temp = 0x1A; 			// ID for DAC is 0x1A (see table 14-2)
	temp |= 0<<8;					// Selection Generic clock generator 0
	GCLK->CLKCTRL.reg = temp; 			// Setup in the CLKCTRL register
	GCLK->CLKCTRL.reg |= 0x1u << 14; 		// enable it.
}

/* Perform Clock configuration to source the TC */
void enable_tc_clocks(void)
{
	PM->APBCMASK.reg |= 0x1u << 13;  	// PM_APBCMASK for TC5 is in the 10th bit position
	uint32_t temp= 0x15;   		// ID for TC2 is 0x15 (see table 14-2)
	temp |= 0<<8;         			//  Selection Generic clock generator 0
	GCLK->CLKCTRL.reg=temp;   		//  Setup in the CLKCTRL register
	GCLK->CLKCTRL.reg |= 0x1u << 14;    	// enable it.
	
}

void enable_tc_clocks_2(void)
{
	PM->APBCMASK.reg |= 0x1u << 12;  	// PM_APBCMASK for TC4 is in the 10th bit position
	uint32_t temp= 0x15;   		// ID for TC2 is 0x15 (see table 14-2)
	temp |= 0<<8;         			//  Selection Generic clock generator 0
	GCLK->CLKCTRL.reg=temp;   		//  Setup in the CLKCTRL register
	GCLK->CLKCTRL.reg |= 0x1u << 14;    	// enable it.
	
}

void enable_tc_clocks_3(void)
{
	PM->APBCMASK.reg |= 0x1u << 11;  	// PM_APBCMASK for TC3 is in the 10th bit position
	uint32_t temp= 0x14;   		// ID for TC2 is 0x14 (see table 14-2)
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
//--------------------------------------------------------------------------------------
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
	
	tcptr4->COUNT8.CTRLA.reg |= 0x1u << 1; /*Enable TC*/
	
//-----------------------------------------------------------------------------
	
	tcptr3->COUNT8.CTRLA.reg |= 0x0u << 1;  //disable the TC
	
	//set up of the CTRLA register
	//Pre-scaler is set to PRESC (0x7)
	//Run in standby is off
	//GCLK_TC / 1024 is used
	//Wave generation is set to NPWM
	//TC mode is set to 8 bit count
	tcptr3->COUNT8.CTRLA.reg = 0x1744 ;
	
	
	//The upper limit is set to 130
	tcptr3->COUNT8.PER.reg = 0x82;
	
	tcptr3->COUNT8.INTENSET.bit.OVF = 0x1; // Overflow Interrupt is Set Here

	
	tcptr3->COUNT8.CTRLA.reg |= 0x1u << 1; /*Enable TC*/
	
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
	while (count_1 < t * 500)
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
	
	porB -> OUTSET.reg = 0xFF;
	porA -> OUTSET.reg = PORT_PA04 | PORT_PA05 |PORT_PA06 |PORT_PA07 ;
	porA -> OUTCLR.reg = PORT_PA07; // Enable register 7
	
	displayC(dis_arr[i]); // Display the character keypad_char[0] on the rightmost LED on SAMD20
	wait(1);
	
	
	porB -> OUTSET.reg = 0xFF;
	porA -> OUTSET.reg = PORT_PA04 | PORT_PA05 |PORT_PA06 |PORT_PA07 ;
	porA -> OUTCLR.reg = PORT_PA06; // Enable register 6

	displayC (dis_arr[i+1]); // Display the character keypad_char[1] on the second from rightmost LED on SAMD20
	wait(1);
	
	porB -> OUTSET.reg = 0xFF;
	porA -> OUTSET.reg = PORT_PA04 | PORT_PA05 |PORT_PA06 |PORT_PA07 ;
	porA -> OUTCLR.reg = PORT_PA05; // Enable register 5
	
	displayC (dis_arr[i+2]); // Display the character keypad_char[2] on the third from rightmost LED on SAMD20
	wait(1);
	

	porB -> OUTSET.reg = 0xFF;
	porA -> OUTSET.reg = PORT_PA04 | PORT_PA05 |PORT_PA06 |PORT_PA07 ;
	porA -> OUTCLR.reg = PORT_PA04; // Enable register 4
	
	displayC(dis_arr[i+3]); // Display the character keypad_char[3] on the leftmost LED on SAMD20
	wait(1);
	
	porA -> OUTSET.reg = PORT_PA04 | PORT_PA05 |PORT_PA06 |PORT_PA07 ;
	wait(1);
	porB -> OUTSET.reg = 0xFF;
	//wait(1);
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

char sense_key (void)
{
	char key = 0x00;

	//sets the base address for the Port structure to PORT_INSTS or 0x41004400
	Port *ports = PORT_INSTS;
	
	//sets the group offset for the structure PortGroup in this case it is for group[0] or groupA
	PortGroup *porA = &(ports->Group[0]);
	
	porA -> OUTSET.reg = PORT_PA05 |PORT_PA06 |PORT_PA07 ;
	porA -> OUTCLR.reg = PORT_PA04; // Activated raw four
	
	if( porA -> IN.reg & PORT_PA16)
	{
		key = 'D';
		return key;
	}
	
	if( porA -> IN.reg & PORT_PA17)
	{
		key = '#';
		return key;
	}

	if( porA -> IN.reg & PORT_PA18)
	{
		key = '0';
		return key;
	}

	if( porA -> IN.reg & PORT_PA19)
	{
		key = '*';
		return key;
	}
	// The following lines of code output 7,8,9, C if the buttons assigned to 7,8,9,C are pressed

	// The following lines of code enable the second from leftmost output LED by enabling register 5 of port A and calls display function to display the digit on that particular LED

	
	porA -> OUTSET.reg = PORT_PA04 |PORT_PA06 |PORT_PA07 ;
	porA -> OUTCLR.reg = PORT_PA05;
	
	if( porA -> IN.reg & PORT_PA16)
	{
		key = 'C';
		return key;
	}
	
	if( porA -> IN.reg & PORT_PA17)
	{
		key = '9';
		return key;
	}

	if( porA -> IN.reg & PORT_PA18)
	{
		key = '8';
		return key;
	}
	
	if( porA -> IN.reg & PORT_PA19)
	{
		key = '7';
		return key;
	}
	// The following lines of code output 4,5,6, b if the buttons assigned to 4,5,6,b are pressed
	
	// The following lines of code enable the second from leftmost output LED by enabling register 5 of port A and calls display function to display the digit on that particular LED

	
	porA -> OUTSET.reg = PORT_PA04 | PORT_PA05 |PORT_PA07 ;
	porA -> OUTCLR.reg = PORT_PA06;
	
	if( porA -> IN.reg & PORT_PA16)
	{
		key = 'B';
		return key;
	}
	
	if( porA -> IN.reg & PORT_PA17)
	{
		key = '6';
		return key;
	}

	if( porA -> IN.reg & PORT_PA18)
	{
		key= '5';
		return key;
	}
	
	if( porA -> IN.reg & PORT_PA19)
	{
		key = '4';
		return key;
	}

	// The following lines of code output 1,2,3, A if the buttons assigned to 1,2,3,A are pressed
	
	// The following lines of code enable the second from leftmost output LED by enabling register 5 of port A and calls display function to display the digit on that particular LED

	
	porA -> OUTSET.reg = PORT_PA04 | PORT_PA05 |PORT_PA06;
	
	porA -> OUTCLR.reg = PORT_PA07;
	
	
	if( porA -> IN.reg & PORT_PA16)
	{
		key = 'A';
		return key;
	}
	
	if( porA -> IN.reg & PORT_PA17)
	{
		key = '3';
		return key;
	}

	if( porA -> IN.reg & PORT_PA18)
	{
		key = '2';
		return key;
	}
	
	if( porA -> IN.reg & PORT_PA19)
	{
		key = '1';
		return key;
	}

	porA -> OUTSET.reg = PORT_PA07;

	return key;
}