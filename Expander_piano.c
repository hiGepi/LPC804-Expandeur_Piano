/*
===============================================================================
 Name        : Expander.c
 Author      : $Saša Radosavljevic
 Version     : 1
 Copyright   : $(copyright)
 Description : MIDI Expander project for Digital Piano Keyboard
===============================================================================
*/

#include <cr_section_macros.h>
#include <stdio.h>
#include "LPC8xx.h"
#include "fro.h"
#include "rom_api.h"
#include "syscon.h"
#include "swm.h"
#include "lib_ENS_II1_lcd.h"
#include "mrt.h"
#include "chip_setup.h"
#include "uart.h"
#include "iocon.h"
#include "DAC.h"
#include "gpio.h"
#include "utilities.h"
#include <string.h>

#define Size_MIDI 3

unsigned char rx_buffer[Size_MIDI+1];
unsigned char play;
volatile enum {false, true} handshake;

uint32_t vel;

// Piano sample (Yamaha CFX sampling on a digital piano)
uint16_t SampleSize[12] = {62,58,55,52,49,46,44,41,39,36,35,32};
uint16_t Sample[12][62] = {
 {511,519,556,607,671,737,802,859,906,944,970,977,961,918,848,757,656,558,479,427,395,374,348,311,265,220,188,180,203,256,332,419,506,582,641,677,688,675,636,576,503,430,369,327,304,299,301,303,297,282,263,250,249,264,293,332,373,409,435,450,462,474}
,{511,566,676,751,793,819,848,891,942,977,966,901,809,733,696,694,709,720,713,680,625,559,499,460,453,476,521,569,597,583,528,453,381,323,282,262,265,294,343,391,422,432,433,424,401,374,374,406,444,455,430,376,307,241,203,213,276,384}
,{511,538,633,728,819,899,956,977,959,907,834,757,686,624,564,501,436,382,347,334,341,370,420,474,513,531,542,559,585,614,642,661,671,674,673,666,644,602,545,484,424,363,309,277,269,269,264,248,221,191,177,196,250,327,413}
,{511,531,553,587,633,668,671,654,634,610,587,582,606,653,704,746,772,775,757,738,726,717,710,704,689,667,638,592,532,478,435,388,336,292,272,265,240,181,111,58,41,70,149,258,358,426,457,462,461,470,487,499}
,{511,594,680,752,816,883,944,979,972,937,902,880,873,870,854,819,771,722,672,621,569,518,465,414,376,354,344,346,355,352,335,317,305,299,298,298,288,261,223,184,160,160,177,197,215,240,285,356,447}
,{511,515,559,614,669,711,733,742,754,783,828,872,897,894,867,829,786,744,707,682,665,650,626,588,534,477,431,402,381,345,281,197,113,56,45,69,103,124,134,149,184,240,306,370,425,466}
,{511,588,667,752,837,912,967,977,933,849,758,690,653,629,580,485,385,336,364,442,513,551,568,583,589,560,493,419,370,353,353,340,295,243,221,241,286,330,349,344,343,362,406,467}
,{511,708,839,931,978,975,907,795,691,632,601,554,496,468,482,529,568,540,444,364,381,469,545,574,564,541,528,498,418,319,265,275,300,300,279,244,216,226,272,348,460}
,{543,598,663,729,783,818,830,824,807,779,740,689,623,540,440,330,221,127,63,44,77,153,248,336,401,440,461,467,473,496,537,582,608,604,578,549,534,538,562}
,{511,570,610,650,707,792,852,853,805,732,657,595,552,532,532,544,558,583,636,707,765,774,712,594,453,323,226,155,89,44,48,119,232,343,431,493}
,{511,606,625,605,607,663,752,818,851,847,801,735,644,545,465,406,391,414,433,429,424,428,445,491,541,542,475,333,163,59,44,96,197,329,458}
,{511,551,619,695,752,771,744,700,652,628,633,635,606,542,490,446,406,378,339,278,186,87,44,66,130,214,307,390,446,476,487,486}};


//
// Function name: UART1_IRQHandler
// Description:	  UART1 interrupt service routine.
//                This ISR reads one received char from the UART1 RXDAT register,
//                appends it to the rx_buffer array until 3 Bytes are stored, then
// 				  puts enable the handshake to process the data
// Parameters:    None
// Returns:       void
//

void UART1_IRQHandler() {
	static uint8_t rx_char_counter = 0;
	unsigned char temp;

	temp = LPC_USART1->RXDAT;
	rx_buffer[rx_char_counter] = temp;        	// Append the current character to the rx_buffer
	rx_char_counter++;                      	// Increment array index counter.

	if (rx_char_counter >= Size_MIDI){
		rx_buffer[rx_char_counter] = 0x00;
		handshake = true;                       // Set handshake for main()
		rx_char_counter = 0;
	}
	return;
}

// DAC interrupt service routine
void DAC0_IRQHandler(void) {
	static uint16_t windex = 0;

	LPC_DAC0->CR = (Sample[play][windex++]*(vel))>>3;
	if(windex >= SampleSize[play]){
		windex=0;
	}
}


//
// Main routine
//
int main(void) {
	uint32_t temp, Div;
	unsigned char temp_char, last;
	unsigned char note[4];
	unsigned char octave;

	// Main Clock initialization at 15 MHz
	LPC_PWRD_API->set_fro_frequency(30000);

	init_lcd();

	// Enable clocks to relevant peripherals
	LPC_SYSCON->SYSAHBCLKCTRL0 |= (UART1 | SWM | IOCON | DAC0);

	// Connect UART1 TXD, RXD signals to port pins
	ConfigSWM(U1_TXD, TARGET_TX);
	ConfigSWM(U1_RXD, P0_20);

	// Configure UART1 for 31250 baud, 8 data bits, no parity, 1 stop bit.

	// Configure FRG0
	LPC_SYSCON->FRG0MULT = 0;
	LPC_SYSCON->FRG0DIV = 255;

	// Select frg0clk as the source for fclk0 (to UART0)
	LPC_SYSCON->UART1CLKSEL = 0;

	// Give USART1 a reset
	LPC_SYSCON->PRESETCTRL0 &= (UART1_RST_N);
	LPC_SYSCON->PRESETCTRL0 |= ~(UART1_RST_N);

	// Configure the USART1 baud rate generator
	LPC_USART1->BRG = 29;

	// Configure the USART1 CFG register:
	// 8 data bits, no parity, one stop bit, no flow control, asynchronous mode
	LPC_USART1->CFG = DATA_LENG_8|PARITY_NONE|STOP_BIT_1;

	// Configure the USART1 CTL register (nothing to be done here)
	// No continuous break, no address detect, no Tx disable, no CC, no CLRCC
	LPC_USART1->CTL = 0;

	// Clear any pending flags, just in case
	LPC_USART1->STAT = 0xFFFF;

	// Enable USART1
	LPC_USART1->CFG |= UART_EN;

	// Enable the USART1 RX Ready Interrupt
	LPC_USART1->INTENSET = RXRDY;
	NVIC_EnableIRQ(UART1_IRQn);

	// Configure 10-bits DAC

	// Enable DACOUT on its pin
	LPC_SWM->PINENABLE0 &= ~(DACOUT0);

	// Configure the DACOUT pin. Inactive mode (no pullups/pulldowns), DAC function enabled
	temp = (LPC_IOCON->DACOUT_PIN) & (IOCON_MODE_MASK) & (IOCON_DACEN_MASK);
	temp |= (0<<IOCON_MODE)|(1<<IOCON_DAC_ENABLE);
	LPC_IOCON->DACOUT_PIN = temp;

	// Configure the 10-bit DAC counter with an initial Freq.
	// SamplesPerCycle * Freq = samples/sec
	Div = (15000000-1);
	LPC_DAC0->CNTVAL = Div - 1;

	// Power to the DAC!
	LPC_SYSCON->PDRUNCFG &= ~(DAC0_PD);

	// Write to the CTRL register to start the action. Double buffering enabled, Count enabled.
	LPC_DAC0->CTRL = (1<<DAC_DBLBUF_ENA) | (1<<DAC_CNT_ENA);

	// Enable the DAC interrupt
	NVIC_EnableIRQ(DAC0_IRQn);

	//IHM
	lcd_position(0,  0);
	lcd_puts("Note : ");
	lcd_position(1,  0);
	lcd_puts("MIDI : ");

	//Sampling rate 16kHz
	LPC_DAC0->CNTVAL = 15000000/16000 - 1;
	while(1) {
		handshake = false;                                   	// Clear handshake flag, will be set by ISR at end of user input
		while (handshake == false);                          	// Wait here for handshake from ISR

		lcd_position(1,  8);
		lcd_puts((char*) rx_buffer);            				// Display the data received

		temp_char = rx_buffer[1]%12+1;							// Getting key's number
		if(rx_buffer[2] != 0)last = rx_buffer[1];				// Memorizing the last pressed key
		if(rx_buffer[1] == last){
			vel = rx_buffer[2];					// Changing the amplitude
		} else {
			temp_char = 20;										// If the last pressed key  is not released, we do nothing
		}

		note[2] = ' ';
		octave = (rx_buffer[1]-rx_buffer[1]%12)/12;

		if(octave != 4){
			vel = 0;
			temp_char = 20;
		}
		// First octave reference multiplied by the key's octave
		switch (temp_char) {
			case 20: break;
			case 1:  play =  0; note[0] = 'C'; note[1] = (unsigned char) '0'+octave; break;
			case 2:  play =  1; note[0] = 'C'; note[1] = '#'; note[2] = (unsigned char) '0'+octave; break;
			case 3:  play =  2; note[0] = 'D'; note[1] = (unsigned char) '0'+octave; break;
			case 4:  play =  3; note[0] = 'D'; note[1] = '#'; note[2] = (unsigned char) '0'+octave; break;
			case 5:  play =  4; note[0] = 'E'; note[1] = (unsigned char) '0'+octave; break;
			case 6:  play =  5; note[0] = 'F'; note[1] = (unsigned char) '0'+octave; break;
			case 7:  play =  6; note[0] = 'F'; note[1] = '#'; note[2] = (unsigned char) '0'+octave; break;
			case 8:  play =  7; note[0] = 'G'; note[1] = (unsigned char) '0'+octave; break;
			case 9:  play =  8; note[0] = 'G'; note[1] = '#'; note[2] = (unsigned char) '0'+octave; break;
			case 10: play =  9; note[0] = 'A'; note[1] = (unsigned char) '0'+octave; break;
			case 11: play = 10; note[0] = 'A'; note[1] = '#'; note[2] = (unsigned char) '0'+octave; break;
			case 12: play = 11; note[0] = 'B'; note[1] = (unsigned char) '0'+octave; break;
			default:
			case '0': LPC_DAC0->CTRL = 0;
					  LPC_DAC0->CR = 512;
		} // end of switch
		lcd_position(0,  8);
		lcd_puts((char*) note);
	}

} // end of main
