#include <asf.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

void Simple_Clk_Init(void);
void displayC (char disNum); // Transmit UART communication function prototype
void wait(int t); // Wait function prototype
void clear_arr (volatile char *arr);
void dis_all_leds (void);
void dis_all_leds2 (char *word);
char sense_key(void);
volatile char dis_arr[10]; // Char array that keeps the data that is to be received
volatile char keypad_key = 0x00;
volatile int count =0;
volatile float op1 = 0, op2 = 0;
char* volToReg (volatile char *charArr);
gcvtf(float,int,char*);

int main(void)
{
	Simple_Clk_Init();

	//sets the base address for the Port structure to PORT_INSTS or 0x41004400
	Port *ports = PORT_INSTS;
	
	//sets the group offset for the structure PortGroup in this case it is for group[0] or groupA
	// GroupA offset of ' '				// GroupB offset of 0x80
	PortGroup *porA = &(ports->Group[0]);


	// The following statement sets the directory of 4,5,6,7 of port A

	//Set some reg of port A to output
	porA -> DIRSET.reg = PORT_PA04 | PORT_PA05 |PORT_PA06 |PORT_PA07;

	//Set some reg of port A to input
	// The following statement clears the directory of 4,5,6,7 of port A
	porA -> DIRCLR.reg = PORT_PA16 | PORT_PA17 | PORT_PA18| PORT_PA19;
	
	// The following four lines of code sets the pull resistor on registers 16,17,18,19 of port A.

	//Enable input and pull up resistor
	porA->PINCFG[16].reg = PORT_PINCFG_INEN | PORT_PINCFG_PULLEN;
	porA->PINCFG[17].reg = PORT_PINCFG_INEN | PORT_PINCFG_PULLEN;
	porA->PINCFG[18].reg = PORT_PINCFG_INEN | PORT_PINCFG_PULLEN;
	porA->PINCFG[19].reg = PORT_PINCFG_INEN | PORT_PINCFG_PULLEN;
	
	
	clear_arr(dis_arr);
	char operation = 0;
	
	while(1)
	{

		if ((keypad_key = sense_key()) && keypad_key != 0x00)
		{
			if(keypad_key == '#')
			{
				
				op1 = atof(dis_arr);
				operation = '+';
				clear_arr(dis_arr);
				count = 0; 
			}

			if(keypad_key == '*')
			{
				
				
				if(dis_arr[0] == NULL)
				{
					dis_arr[count] = '-';
					++count;
				}
				
				else
				{
					op1 = atof(dis_arr);
					operation = '-';
					clear_arr(dis_arr);
					count = 0;
				}
			}
			
			if(keypad_key == 'C' || keypad_key == 'c')
			{
				
				dis_arr[count] = '.';
					++count;
			}

			if(keypad_key == 'D' || keypad_key == 'd')
			{

				op1 += atoi(dis_arr);
				operation = '*';
				clear_arr(dis_arr);
				count = 0;
			}
			
			if(keypad_key == 'B' || keypad_key == 'b')
			{
				--count;
				dis_arr[count] = 0x00;
			}

			if(keypad_key == 'a' || keypad_key == 'A')
			{
				if(operation == '=')
				{
					clear_arr(dis_arr);
					count = 0;
					keypad_key = 0x00;
				}
				else
					op2 = atof(dis_arr);
				switch (operation)
				{
					case '+': {
								op1+=op2;
								 gcvtf(op1,4,dis_arr);
								//itoa(op1,dis_arr,10);
								break;
							  }
					case '-': {
								op1-=op2;
								 gcvtf(op1,4,dis_arr);
								//itoa(op1,dis_arr,10);
								break;
							  }
					case '*': {
								op1*=op2;
								gcvtf(op1,4,dis_arr);
								//itoa(op1,dis_arr,10);
								break;
							  }
					//case '%': {
								//op1/=op2;
								//itoa(op1,dis_arr,10);
								//break;
							  //}
					default: break;
				}

				op1 = op2 = 0;
				operation = '=';
			
			}
	
			if(keypad_key == '1' || keypad_key == '2' || keypad_key == '3' || keypad_key == '4' || keypad_key == '5' 
			|| keypad_key == '6' || keypad_key == '7' || keypad_key == '8' || keypad_key == '9' || keypad_key == '0')
			 
			 {
				dis_arr[count] = keypad_key;
				++count;
			 }
		}
		
			dis_all_leds();
	
	}
}

char* volToReg (volatile char *charArr)
{
	 static char array[10];

	for (unsigned int k = 0; k < sizeof(charArr); ++k)
	{
		array[k] = charArr[k];
	}
	
	return array;
}
//Simple Clock Initialization//
void Simple_Clk_Init(void)
{
	/* Various bits in the INTFLAG register can be set to one at startup.
	   This will ensure that these bits are cleared */
	
	SYSCTRL->INTFLAG.reg = SYSCTRL_INTFLAG_BOD33RDY | SYSCTRL_INTFLAG_BOD33DET |
			SYSCTRL_INTFLAG_DFLLRDY;
			
	system_flash_set_waitstates(0);  //Clock_flash wait state =0

	SYSCTRL_OSC8M_Type temp = SYSCTRL->OSC8M;      /* for OSC8M initialization  */

	temp.bit.PRESC    = 0;    // no divide, i.e., set clock=8Mhz  (see page 170)
	temp.bit.ONDEMAND = 1;    //  On-demand is true
	temp.bit.RUNSTDBY = 0;    //  Standby is false
	
	SYSCTRL->OSC8M = temp;

	SYSCTRL->OSC8M.reg |= 0x1u << 1;  //SYSCTRL_OSC8M_ENABLE bit = bit-1 (page 170)
	
	PM->CPUSEL.reg = (uint32_t)0;    // CPU and BUS clocks Divide by 1  (see page 110)
	PM->APBASEL.reg = (uint32_t)0;     // APBA clock 0= Divide by 1  (see page 110)
	PM->APBBSEL.reg = (uint32_t)0;     // APBB clock 0= Divide by 1  (see page 110)
	PM->APBCSEL.reg = (uint32_t)0;     // APBB clock 0= Divide by 1  (see page 110)

	PM->APBAMASK.reg |= 01u<<3;   // Enable Generic clock controller clock (page 127)

	/* Software reset Generic clock to ensure it is re-initialized correctly */

	GCLK->CTRL.reg = 0x1u << 0;   // Reset gen. clock (see page 94)
	while (GCLK->CTRL.reg & 0x1u ) {  /* Wait for reset to complete */ }
	
	// Initialization and enable generic clock #0

	*((uint8_t*)&GCLK->GENDIV.reg) = 0;  // Select GCLK0 (page 104, Table 14-10)

	GCLK->GENDIV.reg  = 0x0100;   		 // Divide by 1 for GCLK #0 (page 104)

	GCLK->GENCTRL.reg = 0x030600;  		 // GCLK#0 enable, Source=6(OSC8M), IDC=1 (page 101)
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

void clear_arr (volatile char *arr)
{
	for (unsigned int y = 0 ; y < sizeof(arr); y++)
	{
		arr[y] = 0x00;
	}
}

char sense_key (void)
{
	char key = 0;
	char dis_key[4];
	clear_arr(dis_key);

	//sets the base address for the Port structure to PORT_INSTS or 0x41004400
	Port *ports = PORT_INSTS;
	
	//sets the group offset for the structure PortGroup in this case it is for        group[0] or groupA
	// GroupA offset of ' '				// GroupB offset of 0x80
	PortGroup *porA = &(ports->Group[0]);
	PortGroup *porB = &(ports->Group[1]);
	porA -> OUTSET.reg = PORT_PA05 |PORT_PA06 |PORT_PA07 ;
	porA -> OUTCLR.reg = PORT_PA04; // Activated raw four
	
	if( porA -> IN.reg & PORT_PA16)
	{
		key = 'D';
		
		while (porA -> IN.reg & ( PORT_PA16 | PORT_PA17 | PORT_PA18 | PORT_PA19)){} // Wait till the button is released
		
		return key;
	}
	
	if( porA -> IN.reg & PORT_PA17)
	{
		key = '#';
		//strncpy (dis_key, "####",4);
		while (porA -> IN.reg & ( PORT_PA16 | PORT_PA17 | PORT_PA18 | PORT_PA19)){} // Wait till the button is released
		return key;
	}

	if( porA -> IN.reg & PORT_PA18)
	{
		key = '0';
		//strncpy (dis_key, "0000",4);
		while (porA -> IN.reg & ( PORT_PA16 | PORT_PA17 | PORT_PA18 | PORT_PA19)){}// Wait till the button is released
		return key;
	}

	if( porA -> IN.reg & PORT_PA19)
	{
		key = '*';
		//strncpy (dis_key, "****",4);
		while (porA -> IN.reg & ( PORT_PA16 | PORT_PA17 | PORT_PA18 | PORT_PA19)){} // Wait till the button is released
		return key;
	}
	// The following lines of code output 7,8,9, C if the buttons assigned to 7,8,9,C are pressed

	// The following lines of code enable the second from leftmost output LED by enabling register 5 of port A and calls display function to display the digit on that particular LED

	
	porA -> OUTSET.reg = PORT_PA04 |PORT_PA06 |PORT_PA07 ;
	porA -> OUTCLR.reg = PORT_PA05;
	
	if( porA -> IN.reg & PORT_PA16)
	{
		key = 'C';
		//strncpy (dis_key, "CCCC",4);
		while (porA -> IN.reg & ( PORT_PA16 | PORT_PA17 | PORT_PA18 | PORT_PA19)){}// Wait till the button is released
		return key;
	}
	
	if( porA -> IN.reg & PORT_PA17)
	{
		key = '9';
		//strncpy (dis_key, "9999",4);
		while (porA -> IN.reg & ( PORT_PA16 | PORT_PA17 | PORT_PA18 | PORT_PA19)){}// Wait till the button is released
		return key;
	}

	if( porA -> IN.reg & PORT_PA18)
	{
		key = '8';
		//strncpy (dis_key, "8888",4);
		while (porA -> IN.reg & ( PORT_PA16 | PORT_PA17 | PORT_PA18 | PORT_PA19)){}// Wait till the button is released
		return key;
	}
	
	if( porA -> IN.reg & PORT_PA19)
	{
		key = '7';
		//strncpy (dis_key, "7777",4);
		while (porA -> IN.reg & ( PORT_PA16 | PORT_PA17 | PORT_PA18 | PORT_PA19)){}// Wait till the button is released
		return key;
	}
	// The following lines of code output 4,5,6, b if the buttons assigned to 4,5,6,b are pressed
	
	// The following lines of code enable the second from leftmost output LED by enabling register 5 of port A and calls display function to display the digit on that particular LED

	
	porA -> OUTSET.reg = PORT_PA04 | PORT_PA05 |PORT_PA07 ;
	porA -> OUTCLR.reg = PORT_PA06;
	
	if( porA -> IN.reg & PORT_PA16)
	{
		key = 'B';
		//strncpy (dis_key, "BBBB",4);
		while (porA -> IN.reg & ( PORT_PA16 | PORT_PA17 | PORT_PA18 | PORT_PA19)){}// Wait till the button is released
		return key;
	}
	
	if( porA -> IN.reg & PORT_PA17)
	{
		key = '6';
		//strncpy (dis_key, "6666",4);
		while (porA -> IN.reg & ( PORT_PA16 | PORT_PA17 | PORT_PA18 | PORT_PA19)){}// Wait till the button is released
		return key;
	}

	if( porA -> IN.reg & PORT_PA18)
	{
		key= '5';
		//strncpy (dis_key, "5555",4);
		while (porA -> IN.reg & ( PORT_PA16 | PORT_PA17 | PORT_PA18 | PORT_PA19)){}// Wait till the button is released
		return key;
	}
	
	if( porA -> IN.reg & PORT_PA19)
	{
		key = '4';
		//strncpy (dis_key, "4444",4);
		while (porA -> IN.reg & ( PORT_PA16 | PORT_PA17 | PORT_PA18 | PORT_PA19)){}// Wait till the button is released
		return key;
	}

	// The following lines of code output 1,2,3, A if the buttons assigned to 1,2,3,A are pressed
	
	// The following lines of code enable the second from leftmost output LED by enabling register 5 of port A and calls display function to display the digit on that particular LED

	
	porA -> OUTSET.reg = PORT_PA04 | PORT_PA05 |PORT_PA06;
	porA -> OUTCLR.reg = PORT_PA07;
	
	
	if( porA -> IN.reg & PORT_PA16)
	{
		key = 'A';
		//strncpy (dis_key, "AAAA",4);
		//op1 += atoi (keypad_char);	
		while (porA -> IN.reg & ( PORT_PA16 | PORT_PA17 | PORT_PA18 | PORT_PA19)){}// Wait till the button is released
		return key;
	}
	
	if( porA -> IN.reg & PORT_PA17)
	{
		key = '3';
		//strncpy (dis_key, "3333",4);
		while (porA -> IN.reg & ( PORT_PA16 | PORT_PA17 | PORT_PA18 | PORT_PA19)){}// Wait till the button is released
		return key;
	}

	if( porA -> IN.reg & PORT_PA18)
	{
		key = '2';
		//strncpy (dis_key, "2222",4);
		while (porA -> IN.reg & ( PORT_PA16 | PORT_PA17 | PORT_PA18 | PORT_PA19)){}// Wait till the button is released
		return key;
	}
	
	if( porA -> IN.reg & PORT_PA19)
	{
		key = '1';
		//strncpy (dis_key, "NULL",4);
		while (porA -> IN.reg & ( PORT_PA16 | PORT_PA17 | PORT_PA18 | PORT_PA19)){}// Wait till the button is released
		return key;
	}

	porA -> OUTSET.reg = PORT_PA07 |PORT_PA04 | PORT_PA05 |PORT_PA06;
	//wait (1);

	return key;
}

void dis_all_leds (void)
{
	//sets the base address for the Port structure to PORT_INSTS or 0x41004400
	Port *ports = PORT_INSTS;
	
	//sets the group offset for the structure PortGroup in this case it is for        group[0] or groupA
	// GroupA offset of ' '				// GroupB offset of 0x80
	PortGroup *porA = &(ports->Group[0]);
	int i =0;
	if(dis_arr[i] == '-')
	{ 
		displayC(dis_arr[i]);
		i = 1;
		
	}
	
	porA -> OUTSET.reg = PORT_PA04 | PORT_PA05 |PORT_PA06 |PORT_PA07 ;
	porA -> OUTCLR.reg = PORT_PA07; // Enable register 7
	displayC(dis_arr[i]); // Display the character keypad_char[0] on the rightmost LED on SAMD20
	wait(3);
	
	if(dis_arr[i+1] == '.')
	{
		porA -> OUTSET.reg = PORT_PA04 | PORT_PA05 |PORT_PA06 |PORT_PA07 ;
		porA -> OUTCLR.reg = PORT_PA07; // Enable register 7
		displayC(dis_arr[i]); // Display the character keypad_char[0] on the rightmost LED on SAMD20
		wait(3);
		++i;

	}

	porA -> OUTSET.reg = PORT_PA04 | PORT_PA05 |PORT_PA06 |PORT_PA07 ;
	porA -> OUTCLR.reg = PORT_PA06; // Enable register 6
	displayC (dis_arr[i+1]); // Display the character keypad_char[1] on the second from rightmost LED on SAMD20
	wait(3);
	
	if(dis_arr[i+2] == '.')
	{
		porA -> OUTSET.reg = PORT_PA04 | PORT_PA05 |PORT_PA06 |PORT_PA07 ;
		porA -> OUTCLR.reg = PORT_PA06; // Enable register 6
		displayC (dis_arr[i+2]); // Display the character keypad_char[1] on the second from rightmost LED on SAMD20
		wait(3);
		++i;

	}

	porA -> OUTSET.reg = PORT_PA04 | PORT_PA05 |PORT_PA06 |PORT_PA07 ;
	porA -> OUTCLR.reg = PORT_PA05; // Enable register 5
	displayC (dis_arr[i+2]); // Display the character keypad_char[2] on the third from rightmost LED on SAMD20
	wait(3);
	
	if(dis_arr[i+3] == '.')
	{
		porA -> OUTSET.reg = PORT_PA04 | PORT_PA05 |PORT_PA06 |PORT_PA07 ;
		porA -> OUTCLR.reg = PORT_PA05; // Enable register 5
		displayC (dis_arr[i+3]); // Display the character keypad_char[2] on the third from rightmost LED on SAMD20
		wait(3);
		++i;

	}

	porA -> OUTSET.reg = PORT_PA04 | PORT_PA05 |PORT_PA06 |PORT_PA07 ;
	porA -> OUTCLR.reg = PORT_PA04; // Enable register 4
	displayC(dis_arr[i+3]); // Display the character keypad_char[3] on the leftmost LED on SAMD20
	wait(3);
	
	if(dis_arr[i+4] == '.')
	{
		porA -> OUTSET.reg = PORT_PA04 | PORT_PA05 |PORT_PA06 |PORT_PA07 ;
		porA -> OUTCLR.reg = PORT_PA04; // Enable register 4
		displayC(dis_arr[i+4]); // Display the character keypad_char[3] on the leftmost LED on SAMD20
		wait(3);
		++i;

	}

	porA -> OUTSET.reg = PORT_PA04 | PORT_PA05 |PORT_PA06 |PORT_PA07 ;
	wait(2);
}

void dis_all_leds2 (char *word)

{
	//sets the base address for the Port structure to PORT_INSTS or 0x41004400
	Port *ports = PORT_INSTS;
	
	//sets the group offset for the structure PortGroup in this case it is for        group[0] or groupA
	// GroupA offset of ' '				// GroupB offset of 0x80
	PortGroup *porA = &(ports->Group[0]);
	int i =0;
	
	porA -> OUTSET.reg = PORT_PA04 | PORT_PA05 |PORT_PA06 |PORT_PA07 ;
	porA -> OUTCLR.reg = PORT_PA07; // Enable register 7
	displayC(word[i]); // Display the character keypad_char[0] on the rightmost LED on SAMD20
	wait(3);

	porA -> OUTSET.reg = PORT_PA04 | PORT_PA05 |PORT_PA06 |PORT_PA07 ;
	porA -> OUTCLR.reg = PORT_PA06; // Enable register 6
	displayC (word[i+1]); // Display the character keypad_char[1] on the second from rightmost LED on SAMD20
	wait(3);

	porA -> OUTSET.reg = PORT_PA04 | PORT_PA05 |PORT_PA06 |PORT_PA07 ;
	porA -> OUTCLR.reg = PORT_PA05; // Enable register 5
	displayC (word[i+2]); // Display the character keypad_char[2] on the third from rightmost LED on SAMD20
	wait(3);

	porA -> OUTSET.reg = PORT_PA04 | PORT_PA05 |PORT_PA06 |PORT_PA07 ;
	porA -> OUTCLR.reg = PORT_PA04; // Enable register 4
	displayC(word[i+3]); // Display the character keypad_char[3] on the leftmost LED on SAMD20
	wait(3);

	porA -> OUTSET.reg = PORT_PA04 | PORT_PA05 |PORT_PA06 |PORT_PA07 ;
	wait(3);
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
}