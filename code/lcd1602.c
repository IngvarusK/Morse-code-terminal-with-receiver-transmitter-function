/******************************************************************************
 * This file is a part of the Sysytem Microprocessor Tutorial (C).            *
 ******************************************************************************/

/**
 * @file lcd1602.c
 * @author Koryciak
 * @date Sep 2020
 * @brief File containing definitions for LCD 2x16.
 * @ver 0.3
 */

#include "lcd1602.h"

/******************************************************************************\
* Private definitions
\******************************************************************************/
/* LCD functions */
#define LCD_CLEARDISPLAY 		0x01
#define LCD_SETDDRAMADDR 		0x80
#define LCD_FULLLINE				0x40
/* PCF8574 */
#define PCF8574_ADDRESS     0x27 
#define PCF8574A_ADDRESS    0x3F
/* PCF8574 connections to LCD */
//#define PCF8574_D7 0x80
//#define PCF8574_D6 0x40
//#define PCF8574_D5 0x20
//#define PCF8574_D4 0x10
#define PCF8574_BL 0x08 /* Backlight */
#define PCF8574_EN 0x04 /* Enable bit */
#define PCF8574_RW 0x02 /* Read/Write bit (0 = write) */
#define PCF8574_RS 0x01 /* Register select bit */
/******************************************************************************\
* Private memory declarations
\******************************************************************************/
static uint8_t lcd_backlight = 1;
static uint8_t pcf_address = PCF8574_ADDRESS;
/******************************************************************************\
* Private prototypes
\******************************************************************************/
void PCF8574_Write(uint8_t data);
void LCD1602_Write4(uint8_t data, uint8_t rs);
void LCD1602_Write8(uint8_t data, uint8_t rs);
void LCD1602_CheckAddress(void);
uint8_t itoa(int value, char *ptr);

void LCD1602_Init(void) {																																		

	I2C_Init();											/* via I2C communication */
	
	LCD1602_CheckAddress();					/* if any PCF connected check which one */
																											
	DELAY(42)												/* >15ms */
	
	LCD1602_Write8(0x33,0);					/* 4-bit interface */								
	LCD1602_Write8(0x32,0);					/* HD44780U datasheet Figure 24 */
	LCD1602_Write8(0x2C,0);
	LCD1602_Write8(0x08,0);
	LCD1602_Write8(0x01,0);
	LCD1602_Write8(0x0C,0);					/* cursor off, blink off */
}

void LCD1602_Blink_On(void) {
	LCD1602_Write8(0x0D,0);						/* cursor off, blink on */
}

void LCD1602_Blink_Off(void) {
	LCD1602_Write8(0x0C,0);						/* cursor off, blink off */
}

void LCD1602_SetCursor(uint8_t col, uint8_t row) {
	
	if (row>1) row = 1;   /* prevent from too many rows */
	LCD1602_Write8(LCD_SETDDRAMADDR+col+(LCD_FULLLINE*row),0);
}

void LCD1602_ClearAll(void) {
	
	LCD1602_Write8(LCD_CLEARDISPLAY, 0);
}

void LCD1602_Print(char *str) {

  uint8_t str_len = 0;

  while (str[str_len] != '\0') {              /* until end of string */
    LCD1602_Write8(str[str_len], 1);
    ++str_len;
  }
}

void LCD1602_PrintNum(int number) {

  char str[12];
	uint8_t size;
	
	size = itoa(number, str);
	LCD1602_Print(str);
	if (size < 5) LCD1602_Print("      ");   /* change if display bigger values*/
}

void LCD1602_Backlight(uint8_t state) {
	
	lcd_backlight = state;				 /* update internal variable */
	PCF8574_Write(0x00);
}
/**
 * @brief Write byte to LCD including backlight info.
 *
 * @param Data to send.
 */
void PCF8574_Write(uint8_t data) {
	
	I2C_Write(pcf_address, data | (lcd_backlight?PCF8574_BL:0x00));
}
/**
 * @brief Write 4 bits to LCD.
 *
 * @param A lower nibble of the byte.
 * @param Register select.
 */
void LCD1602_Write4(uint8_t data, uint8_t rs) {
	
	PCF8574_Write(((data << 4)&0xF0) | (rs?PCF8574_RS:0x00) | PCF8574_EN);
	PCF8574_Write(((data << 4)&0xF0) | (rs?PCF8574_RS:0x00));
	DELAY(2)
}
/**
 * @brief Write byte to LCD.
 *
 * @param Data to send.
 * @param Register select
 */
void LCD1602_Write8(uint8_t data, uint8_t rs) {
	
	LCD1602_Write4(((data >> 4)&0x0F), rs);
	LCD1602_Write4(( data      &0x0F), rs);
}
/**
 * @brief Check which version of PCF is connected.
 *
 */
void LCD1602_CheckAddress(void) {
	
	if ((I2C_Write(PCF8574_ADDRESS, 0x00) & I2C_ERR_NOACK)==0) 
		pcf_address = PCF8574_ADDRESS;
	if ((I2C_Write(PCF8574A_ADDRESS, 0x00) & I2C_ERR_NOACK)==0) 
		pcf_address = PCF8574A_ADDRESS;
}
/**
 * @brief Convert integer to string.
 *
 * @param Value to convert.
 * @param String to return.
 * @return Size of string.
 */
uint8_t itoa(int value, char *ptr) {
	
	uint8_t count=0;
	int temp;
	
	if(value==0) {   
		*ptr++='0';
		*ptr='\0';
		return 1;
	}
	if(value<0) {
		value*=(-1);    
		*ptr++='-';
		count++;
	}
	for(temp=value;temp>0;ptr++) {
		temp/=10;
	}
	*ptr='\0';
	for(temp=value;temp>0;temp/=10) {
		*--ptr=temp%10+'0';
		count++;
	}
	return count;
}
