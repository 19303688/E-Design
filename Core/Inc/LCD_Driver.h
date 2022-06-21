/*
 * LCD_Driver.h
 *
 *  Created on: Apr 18, 2022
 *      Author: Calvin
 */

#ifndef INC_LCD_DRIVER_H_
#define INC_LCD_DRIVER_H_
#include "main.h"
/*#define LCD_DATA_PORT_D TRISB
#define LCD_RS_D TRISC4
#define LCD_EN_D TRISB11


#define EN PB11
#define D4 PB13
#define D5 PB14
#define D6 PB15
#define D7 PB1
*/
void writeBit(uint8_t value);
void send_Message();
void send_Command(uint8_t L1, uint8_t L2);
void lcd_Init();
void LCD_Write_Char(char Data);
void LCD_Write_String(char *str);
void clearDisplay();
void LCD_Set_Cursor(unsigned char r, unsigned char c);
void LCD_Write_Command(char Data);
#endif /* INC_LCD_DRIVER_H_ */

