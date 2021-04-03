#include "pins.h"
#include "frdm_bsp.h"

void PINs_init(){
	//--------------------------------LED--------------------------------
	SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK; /* Enable clock for GPIO B */
	
	PORTB->PCR[RED_LED_POS] |= PORT_PCR_MUX(1); /* Set Pin 8 MUX as GPIO */
	PORTB->PCR[GREEN_LED_POS] |= PORT_PCR_MUX(1); /* Set Pin 9 MUX as GPIO */
	PORTB->PCR[BLUE_LED_POS] |= PORT_PCR_MUX(1); /* Set Pin 10 MUX as GPIO */
	
	PTB->PDDR |= (1<<RED_LED_POS) | (1<<GREEN_LED_POS) | (1<<BLUE_LED_POS); /* Set RED LED pin as output */
	PTB->PSOR |= (1<<RED_LED_POS) | (1<<GREEN_LED_POS) | (1<<BLUE_LED_POS); /* Turn off RED LED */
	
	//--------------------------------SWs--------------------------------
	SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK; /* Enable clock for GPIO A */
	
	PORTA->PCR[SW1] |= PORT_PCR_MUX(1) | PORT_PCR_PE_MASK | PORT_PCR_PS_MASK; /* Set Pin 1 MUX as GPIO & Enable pull-up resitor on Pin 1 */
	PORTA->PCR[SW2] |= PORT_PCR_MUX(1) | PORT_PCR_PE_MASK | PORT_PCR_PS_MASK; /* Set Pin 1 MUX as GPIO & Enable pull-up resitor on Pin 1 */
	PORTA->PCR[SW3] |= PORT_PCR_MUX(1) | PORT_PCR_PE_MASK | PORT_PCR_PS_MASK; /* Set Pin 1 MUX as GPIO & Enable pull-up resitor on Pin 1 */
}
