/*
	* @file main.c 
	* @author Igor Kaminski
	* @date Jan 15 2021 created
	* @date	Jan	22 2021 finished
	* @brief File containing the main function. 
	* @ver 0.1 
	*
	*	PTA	|12	|11	|10	|9	|
	*	-------------------------
	*			|C1	|C2	|C3	|C4	|
	*	_________________________
	*	PTB	|4				|3				|
	*	-------------------------
	*			|LCD SDA	|LCD SCL	|
	*
*/

#include "MKL05Z4.h" /*Device header*/
#include "frdm_bsp.h"
#include "pit.h"
#include "globals.h"
#include "pins.h"
#include "lcd1602.h"
#include "uart0.h"
#include <stdio.h>

#define unit_len 3

volatile int PIT_flag;// used to check PIT interrupt

volatile char temp;		// char from UART receiver
volatile uint8_t rx_EVENT_flag = 0;// is there any new char to read?

// struct use to switch operatations
struct SW_op{
	int pressed;								// is pressed?
	int time_debounce;					// how long wait 
	int sw_ready_to_read;				// is debounced and not pressed?
	int time;										// time pressed
	int delay_sw_ready_to_read;	// is debounced?
	int delay_time;							// time not pressed
};

// function for switch handling
void switch_operate(const volatile uint32_t, unsigned int, struct SW_op*);

int main(){
	
	int ms = 0;			// global time
	
	int status = 1;//				variable used for Morse code read
	int signal = 0;//				0 - Dah, 1 - Dit, 2 - end of letter
	int signal_flag = 0;//	new Dah/Dit/end
	int letter_num = 31;//	num needed for letter read ex. tab[letter_num]
	char letter = '-';//		letter in ANSI code
	
	// transmitter
	int letter_flag = 0;//		is there any new letter to send?
	uint8_t letter_LCD = 0;//	variable used to fill data[]
	char data[16];//					tab of LCD string
	// receiver
	uint8_t letter_LCD_RX = 0;//	variable used to fill data2[]
	char data2[16];//							tab of LCD string
	
	// tab[] -	Importand variable used to read letter, tab[] is "binary search tree" with
	//					start value 31. Program adds or substracts ex. 16/8/4/2/1 meaning marker
	//					was placed on diffrent letter
	//					Example 31(START) -(+16 Dit)-> 47(E) -(-8 Dah)-> 39(A) -(-4 Dah)-> 35(W)	|-> W
	//
	//					Positions:
	// 					0.................................................................15
	//					16................................................................31
	//					32................................................................47
	//					48.............................................................62
	char tab[] = {'0',' ','9','o',' ',' ','8','m',' ','q',' ','g',' ','z','7','t',
							' ','y',' ','k',' ','c',' ','n',' ','x',' ','d',' ','b','6',' ',
							'1','j',' ','w',' ','p',' ','a',' ',' ',' ','r',' ','l',' ','e',
							'2',' ',' ','u',' ','f',' ','i','3','v',' ','s','4','h','5'};
	
	int unit[] = {unit_len, unit_len, unit_len, unit_len, unit_len};//	variables used to averaging times 
	//																																	& dynamic speed of typing change
	float unit_aver = unit_len;//	average time of typing
	float unit_inacc = 2;//				inaccuration of typing from 1 to 3
	struct SW_op sw1 = {0, 1, 0, 0, 0, 0};// struct used to sw1 operations
	struct SW_op sw3 = {0, 1, 0, 0, 0, 0};// struct used to sw3 operations (PIN not connected on presentation)
	
	//char display[10];// used only with feedback about typed Dah/Dit/end
	
	LCD1602_Init();//						LCD init
	LCD1602_Backlight(TRUE);//	Backlight ON
	LCD1602_Blink_On();//				Blinking LCD
	PIT_Init();// 							PIT init
	PINs_init();//							PINs init
	UART0_Init();//							UART0 initialization
	
	while(1){
		
		// SW1 pressed & ready to read
		if(sw1.sw_ready_to_read){
			//	averaging and dynamic speed of typing change
			unit_aver = (float)((float)(unit[0]*1 + unit[1]*2 + unit[2]*3 + unit[3]*3 + unit[4]*5)/15);
			if(sw1.time > (int)(unit_aver*unit_inacc)){
				signal = 0;//	Dah
				for(int i = 0; i < 4; i++) unit[i] = unit[i+1];
				unit[4] = (int)(sw1.time/3);
//				sprintf(display2,"kreska\r\n");
			}
			else{
				signal = 1;//	Dit
				for(int i = 0; i < 4; i++) unit[i] = unit[i+1];
				unit[4] = sw1.time;
//				sprintf(display2,"kropka\r\n");
			}
			signal_flag = 1;
			sw1.sw_ready_to_read = 0;
			sw1.time = 0;
			sw1.delay_time = 0;
//			for(int i=0;display2[i]!=0;i++){
//					while(!(UART0->S1 & UART0_S1_TDRE_MASK));
//					UART0->D = display2[i];
//			}
		}
		if(sw1.delay_sw_ready_to_read && (status > 1)){
			// is time needed to end the code of letter is < time non-activity?
			if(sw1.delay_time > 3*(int)(unit_aver*unit_inacc)){
				signal = 2;
				signal_flag = 1;
//				sprintf(display2,"STOP\r\n");
//				for(int i=0;display2[i]!=0;i++){
//					while(!(UART0->S1 & UART0_S1_TDRE_MASK));
//					UART0->D = display2[i];
//				}
			}
		}
		// Letter decoding using string of Dah/Dit's & STOP(end)
		if(signal_flag){
			switch(status){
				case 1://	start of "binarry tree" - level 1
					if(signal == 0) letter_num -= 16;
					else letter_num += 16;
					sw1.delay_time = 0;
					status <<= 1;
					break;
				case 2://	"binarry tree" - level 2
					if(signal == 0){
						letter_num -= 8;
						status <<= 1;
						sw1.delay_time = 0;
					}
					else if(signal == 1){
						letter_num += 8;
						status <<= 1;
						sw1.delay_time = 0;
					}
					else{
						letter = tab[letter_num];
						letter_flag = 1;
						status = 1;
						letter_num = 31;		
					}
					break;
				case 4://	"binarry tree" - level 3
					if(signal == 0){
						letter_num -= 4;
						status <<= 1;
						sw1.delay_time = 0;
					}
					else if(signal == 1){
						letter_num += 4;
						status <<= 1;
						sw1.delay_time = 0;
					}
					else{
						letter = tab[letter_num];
						letter_flag = 1;
						status = 1;
						letter_num = 31;		
					}
					break;
				case 8://	"binarry tree" - level 4
					if(signal == 0){
						letter_num -= 2;
						status <<= 1;
						sw1.delay_time = 0;
					}
					else if(signal == 1){
						letter_num += 2;
						status <<= 1;
						sw1.delay_time = 0;
					}
					else{
						letter = tab[letter_num];
						letter_flag = 1;
						status = 1;
						letter_num = 31;		
					}
					break;
				case 16://	"binarry tree" - level 5
					if(signal == 0){
						letter_num -= 1;
						status <<= 1;
						sw1.delay_time = 0;
					}
					else if(signal == 1){
						letter_num += 1;
						status <<= 1;
						sw1.delay_time = 0;
					}
					else{
						letter = tab[letter_num];
						letter_flag = 1;
						status = 1;
						letter_num = 31;		
					}
					break;	
				case 32://	"binarry tree" - level 6 (last)
					letter = tab[letter_num];
					letter_flag = 1;
					status = 1;
					letter_num = 31;	
			}
		signal_flag = 0;
		}
		
		//	LCD first row - letter typed by SW1 & sent by trasmiter
		//	LCD second row - letter read by reveiver from diffrent terminal
		//	new letter to transmit and show on LCD
		if(letter_flag){
			// fill the LCD
			if(letter_LCD < 16){
				LCD1602_SetCursor(letter_LCD,0);
				LCD1602_Write8(letter,1);
				data[letter_LCD] = letter;
				letter_LCD++;
			}
			// change entire row
			else{
				LCD1602_SetCursor(0,0);
				for(int i = 0; i < 15; i++) data[i] = data[i+1];//	move left every letter
				//																									this way is faster than method in comments.c
				data[15] = letter;// save the last letter at the end of the data[]
				for(int i = 0; i < 16; i++){
					LCD1602_Write8(data[i],1);//	send to LCD all chars
				}
			}
			// transmit data
			while(!(UART0->S1 & UART0_S1_TDRE_MASK));
			UART0->D = letter;
			letter_flag = 0;
		}
		
		// new char in RX buf
		if(rx_EVENT_flag){
			// fill the LCD
			if(letter_LCD_RX < 16){
				LCD1602_SetCursor(letter_LCD_RX,1);
				LCD1602_Write8(temp,1);
				data2[letter_LCD_RX] = temp;
				letter_LCD_RX++;
			}
			// change entire row
			else{
				LCD1602_SetCursor(0,1);
				for(int i = 0; i < 15; i++) data2[i] = data2[i+1];//	move left every letter
				//																										this way is faster than method in comments.c
				data2[15] = temp;// save the last letter at the end of the data2[]
				for(int i = 0; i < 16; i++){
					LCD1602_Write8(data2[i],1);//	send to LCD all chars
				}
			}
			rx_EVENT_flag = 0;
		}
		
		if(sw3.sw_ready_to_read){//	PIN not connected - used only if other button is connected
			LCD1602_ClearAll();
			letter_LCD = 0;
			sw3.sw_ready_to_read = 0;
			sw3.time = 0;
		}
		
		//	heart of program synchronization
		if(PIT_flag){
			ms++;
			if(sw1.pressed) sw1.time++;
			else if(status > 1) sw1.delay_time++;
			else sw1.delay_time = 0;
			if(sw3.pressed) sw3.time++;
//			if(!(ms%10)){
//				sprintf(display2,"%d %d,%d,%d,%d,%d\r\n", status, unit[0],unit[1],unit[2],unit[3],unit[4]);
//				for(int i=0;display2[i]!=0;i++){
//					while(!(UART0->S1 & UART0_S1_TDRE_MASK));
//					UART0->D = display2[i];
//				}
//			}
			if(ms > 49) {
				ms = 0;
				if(sw1.delay_time > 999999) sw1.delay_time = 999999;//	Overflow protection
			}
			PIT_flag = 0;
		}
		
		switch_operate(PTA->PDIR, (1<<SW1), &sw1);
		switch_operate(PTA->PDIR, (1<<SW3), &sw3);	
		
	}
}

// heart of debouncing
void switch_operate(const volatile uint32_t PDIR_m, unsigned int SW_mask, struct SW_op* sw){
	if( ( PDIR_m & SW_mask ) == 0 ){ /* Test if button pressed */
		sw->pressed = 1;
		sw->delay_sw_ready_to_read = 0;
	}
	else {
		sw->pressed = 0;
		if(sw->time >= sw->time_debounce) sw->sw_ready_to_read = 1;// if segment wasn't bounced
		else sw->time = 0;// if segment was bounced reset the variable and start the cycle again to get non bounced sygnal
		sw->delay_sw_ready_to_read = 1;
	}
}

void PIT_IRQHandler() {
		
	// check to see which channel triggered interrupt 
	if (PIT->CHANNEL[0].TFLG & PIT_TFLG_TIF_MASK) {
		// clear status flag for timer channel 0
		PIT->CHANNEL[0].TFLG &= PIT_TFLG_TIF_MASK;
		// Do work
		
		PIT_flag = 1;// Set the flag & then read in main loop & handle time services
		
	} else if (PIT->CHANNEL[1].TFLG & PIT_TFLG_TIF_MASK) {
		// clear status flag for timer channel 1
		PIT->CHANNEL[1].TFLG &= PIT_TFLG_TIF_MASK;
	}
	
}

void UART0_IRQHandler(){
	if(UART0->S1 & UART0_S1_RDRF_MASK){
		temp = UART0->D;//		read the char & reset RDRF flag
		rx_EVENT_flag = 1;//	new char to read
	}
}
