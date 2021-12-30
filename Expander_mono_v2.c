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
const unsigned char thestring[] = "Une note ?";

unsigned char rx_buffer[Size_MIDI+1];
uint16_t vel;
volatile enum {false, true} handshake;
volatile enum {low, high} oct;

// 32 values of sinus waveform
const uint16_t waveform1[] = {512, 612, 708, 796,
                             874, 938, 985, 1014,
                             1023, 1014, 985, 938,
                             874, 796, 708, 612,
                             512, 412, 316, 227,
                              150,  86,  39,   10,
                                 0,   10,  39,  86,
                              150, 227, 316, 412};
const uint16_t waveform2[] = {562, 612, 660, 708, 753, 796, 837,
                             874, 908, 938, 963, 985, 1002, 1014,
                             1021, 1014, 1002, 985, 963, 938, 908,
                             874, 837, 796, 753, 708, 660, 612, 562,
                             512, 462, 412, 363, 316, 270, 227, 187,
                             150, 116, 86, 60, 39, 22, 10, 2,
                             0, 2, 10, 22, 39, 60, 86, 116,
                             150, 187, 227, 270, 316, 363, 412, 461, 512};
/*const uint16_t waveform[] = {512, 708, 985, 1023,
                             985, 874, 708, 512,
                             316, 150, 39, 0,
                             39, 150, 316};*/
#define SamplesPerCycle1 (sizeof(waveform1)/2)
#define SamplesPerCycle2 (sizeof(waveform2)/2)
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
	static uint32_t windex = 0;

	if(oct == low){
		LPC_DAC0->CR = (waveform2[windex++]*vel)>>3;
		if (windex >= SamplesPerCycle2) {
			windex = 0;
		}
	} else {
		LPC_DAC0->CR = (waveform1[windex++]*vel)>>4;
		if (windex >= SamplesPerCycle1) {
			windex = 0;
		}
	}
}


//
// Main routine
//
int main(void) {
	uint32_t temp, Freq, Div;
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
	Freq = 1;
	Div = (15000000)/(SamplesPerCycle2 * Freq);
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

	oct = low;
	while(1) {
		handshake = false;                                   	// Clear handshake flag, will be set by ISR at end of user input
		while (handshake == false);                          	// Wait here for handshake from ISR

		lcd_position(1,  8);
		lcd_puts((char*) rx_buffer);            				// Display the data received

		temp_char = rx_buffer[1]%12+1;							// Getting key's number
		if(rx_buffer[2] != 0)last = rx_buffer[1];				// Memorizing the last pressed key
		if(rx_buffer[1] == last){
			vel = rx_buffer[2];									// Changing the amplitude
		} else {
			temp_char = 20;										// If the last pressed key  is not released, we do nothing
		}

		note[2] = ' ';
		octave = (rx_buffer[1]-rx_buffer[1]%12)/12;

		if(octave < 6)oct = low;								// Oversampling low frequencies
		else oct = high;

		// First octave reference multiplied by the key's octave
		switch (temp_char) {
			case 20: break;
			case 1:  Freq = (uint32_t)  32.7<<octave; note[0] = 'C'; note[1] = (unsigned char) '0'+octave; break;
			case 2:  Freq = (uint32_t) 34.64<<octave; note[0] = 'C'; note[1] = '#'; note[2] = (unsigned char) '0'+octave; break;
			case 3:  Freq = (uint32_t)  36.7<<octave; note[0] = 'D'; note[1] = (unsigned char) '0'+octave; break;
			case 4:  Freq = (uint32_t)  38.9<<octave; note[0] = 'D'; note[1] = '#'; note[2] = (unsigned char) '0'+octave; break;
			case 5:  Freq = (uint32_t)  41.2<<octave; note[0] = 'E'; note[1] = (unsigned char) '0'+octave; break;
			case 6:  Freq = (uint32_t) 43.65<<octave; note[0] = 'F'; note[1] = (unsigned char) '0'+octave; break;
			case 7:  Freq = (uint32_t) 46.25<<octave; note[0] = 'F'; note[1] = '#'; note[2] = (unsigned char) '0'+octave; break;
			case 8:  Freq = (uint32_t)    49<<octave; note[0] = 'G'; note[1] = (unsigned char) '0'+octave; break;
			case 9:  Freq = (uint32_t)  51.9<<octave; note[0] = 'G'; note[1] = '#'; note[2] = (unsigned char) '0'+octave; break;
			case 10: Freq = (uint32_t)    55<<octave; note[0] = 'A'; note[1] = (unsigned char) '0'+octave; break;
			case 11: Freq = (uint32_t) 58.27<<octave; note[0] = 'A'; note[1] = '#'; note[2] = (unsigned char) '0'+octave; break;
			case 12: Freq = (uint32_t) 61.73<<octave; note[0] = 'B'; note[1] = (unsigned char) '0'+octave; break;
			default:
			case '0': LPC_DAC0->CTRL = 0;
					  LPC_DAC0->CR = 512;
		} // end of switch

		// Calculate the new DAC counter reload value, and load it
		if(oct == low)LPC_DAC0->CNTVAL = (15000000)/(SamplesPerCycle2 * Freq) - 1;
		else LPC_DAC0->CNTVAL = (15000000)/(SamplesPerCycle1 * Freq) - 1;

		lcd_position(0,  8);
		lcd_puts((char*) note);
	}

} // end of main
