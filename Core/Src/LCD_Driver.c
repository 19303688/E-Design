/*
 * LCD_Driver.c
 *
 *  Created on: Apr 18, 2022
 *      Author: Calvin
 */

#include "LCD_Driver.h"
#include "main.h"
#include <string.h>
#include <stdio.h>



void LCD_DATA(unsigned char Data){

	if(Data & 1){
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_SET);
		//D4 = 1;
	}
	else{
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_RESET);
		//D4 = 0;
	}
	if(Data & 2){
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_SET);
		//D5 = 1;
	}
		else{
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_RESET);
			//D5 = 0;
		}
	if(Data & 4){
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_SET);
		//D6 = 1;
	}
		else{
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_RESET);
			//D6 = 0;
		}
	if(Data & 8){
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_SET);
		//D7 = 1;
		}
		else{
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_RESET);
			//D7 = 0;
		}
}

void LCD_CMD(unsigned char CMD){
	//Select Command Register
	//RS = 0;
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4, GPIO_PIN_RESET);
	// Move the command data to the LCD
	LCD_DATA(CMD);
	//Send the enable clock signal
	//EN = 1;
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_11, GPIO_PIN_SET);
	HAL_Delay(50);
	//EN = 0;
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_11, GPIO_PIN_RESET);
}



void writeBit(uint8_t value){
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, ( (value >> 0) & 0x1) ); //D4
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, ( (value >> 1) & 0x1) ); //D5
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6,  ( (value >> 2) & 0x1) ); //D6
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8,  ( (value >> 3) & 0x1) ); //D7

}
void send_Message(){

	HAL_Delay(1);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_11,  GPIO_PIN_SET);
	HAL_Delay(1);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_11,  GPIO_PIN_RESET);
	HAL_Delay(2);
}
void send_Command(uint8_t L1, uint8_t L2){
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4,  GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2,  GPIO_PIN_RESET);

	writeBit(L1);
	send_Message();
	writeBit(L2);
	send_Message();
}

void lcd_Init(){

	writeBit(0x00);

	HAL_Delay(15);

	//Set to 8 bit mode
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4,  0);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2,  0);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_11,  0);

	LCD_CMD(0b0011); //1 Set 8 bit mode
	HAL_Delay(5);

	LCD_CMD(0b0011); //2 Set 8 bit mode
	HAL_Delay(1);

	LCD_CMD(0b0011); //3 Set 8 bit mode
	HAL_Delay(1);

	LCD_CMD(0b0010); //4 Set 4 bits long
	HAL_Delay(1);

	//Fucntion Set 4 bits long
	LCD_CMD(0b0010); //5 Sets font
	LCD_CMD(0b1000);

	//Function Set display
	LCD_CMD(0b0000); //6 display on/off
	LCD_CMD(0b1111);

	// Display Off
	LCD_CMD(0b0000); //7 Clear Display
	LCD_CMD(0b0001);

	//Display clear
	LCD_CMD(0b0000); //8 Entry mode
	LCD_CMD(0b010);


}
void LCD_Write_Char(char Data)
{
	char Low4,High4;
	Low4  = Data & 0x0F;
	High4 = Data & 0xF0;

	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4, 1); //RS

	writeBit(High4>>4);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_11, 1); //ENABLE
	HAL_Delay(1);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_11, 0); // Enable
	HAL_Delay(1);

	writeBit(Low4);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_11, 1); //ENABLE
	HAL_Delay(1);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_11, 0);
	HAL_Delay(1);
}
void LCD_Write_Command(char Data)
{
	char Low4,High4;
	Low4  = Data & 0x0F;
	High4 = Data & 0xF0;

	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4, 0); //RS

	writeBit(High4>>4);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_11, 1); //ENABLE
	HAL_Delay(1);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_11, 0); // Enable
	HAL_Delay(1);

	writeBit(Low4);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_11, 1); //ENABLE
	HAL_Delay(1);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_11, 0);
	HAL_Delay(1);
}

void LCD_Write_String(char *str){
	int i = 0;
	for (i = 0; str[i] != '\0';i++){
		LCD_Write_Char(str[i]);
	}
}

void clearDisplay(){
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_11,  GPIO_PIN_RESET);

	send_Command(0x0, 0x1);
}
void scrollRight(){
	//ToDo Implement the Scroll
	//LCD_Write_Command();

}
void LCD_Set_Cursor(unsigned char r, unsigned char c){
	unsigned char Temp, Low4, High4;
	if (r ==1 ){
		Temp = 0x80 + c -1 ; //0x80 is used to move the cursor
		High4 = Temp >>4;
		Low4 = Temp & 0x0F;
		LCD_CMD(High4);
		LCD_CMD(Low4);
		//writeBit(High4);
		//send_Message();
		//writeBit(Low4);
		//send_Message();
	}
	if (r == 2){
		Temp = 0xC0 + c - 1;
		High4 = Temp >>4;
		Low4 = Temp & 0x0F;
		//writeBit(High4);
		//send_Message();
		//writeBit(Low4);
		//send_Message();

		LCD_CMD(High4);
		LCD_CMD(Low4);
	}
	HAL_Delay(1);
}

/*

void LCD_DATA(unsigned char Data){

	if(Data & 1){
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_SET);
		//D4 = 1;
	}
	else{
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_RESET);
		//D4 = 0;
	}
	if(Data & 2){
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET);
		//D5 = 1;
	}
		else{
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET);
			//D5 = 0;
		}
	if(Data & 4){
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, GPIO_PIN_SET);
		//D6 = 1;
	}
		else{
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, GPIO_PIN_RESET);
			//D6 = 0;
		}
	if(Data & 8){
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);
		//D7 = 1;
		}
		else{
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
			//D7 = 0;
		}
}

void LCD_CMD(unsigned char CMD){
	//Select Command Register
	//RS = 0;
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4, GPIO_PIN_RESET);
	// Move the command data to the LCD
	LCD_DATA(CMD);
	//Send the enable clock signal
	//EN = 1;
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);
	HAL_Delay(50);
	//EN = 0;
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
}

void LCD_Init(){
	//IO Pin Configurations
	//LCD_DATA_PORT_D = 0x00;
	//LCD_RS_D = 0;
	//LCD_EN_D = 0;
	//The Init Procedure
	LCD_DATA(0x00);
	HAL_Delay(30);

	LCD_CMD(0x03);
	HAL_Delay(5);

	LCD_CMD(0x03);
	HAL_Delay(1);

	LCD_CMD(0x03);
	LCD_CMD(0x02);
	LCD_CMD(0x02);
	LCD_CMD(0x08);
	LCD_CMD(0x00);
	LCD_CMD(0x0C);
	LCD_CMD(0x00);
	LCD_CMD(0x06);

}

void LCD_Write_Char(char Data){
	char Low4,High4;
	Low4 = Data & 0x0F;
	High4 = Data & 0xF0;

	//RS = 1;
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4, GPIO_PIN_SET);

	LCD_DATA(High4>>4);

	//EN = 1;
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);

	Hal_Delay(1);

	//EN = 0;
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);

	Hal_Delay(1);
	LCD_DATA(Low4);

	//EN = 1;
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);

	Hal_Delay(1);

	//EN = 0;
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);

	Hal_delay(1);
}
void LCD_Write_String(char *str){
	int i ;
	for (i = 0; str[i] != '\0';i++){
		LCD_Write_Char(str[i]);
	}
}
void LCD_Clear(){
	LCD_CMD(0);
	LCD_CMD(1);
}

void LCD_Set_Cursor(unsigned char r, unsigned char c){
	unsigned char Temp, Low4, High4;
	if (r ==1 ){
		Temp = 0x80 + c -1 ; //0x80 is used to move the cursor
		High4 = Temp >>4;
		Low4 = Temp & 0x0F;
		LCD_CMD(High4);
		LCD_CMD(Low4);
	}
	if (r == 2){
		Temp = 0xC0 + c - 1;
		High4 = Temp >>4;
		Low4 = Temp & 0x0F;
		LCD_CMD(High4);
		LCD_CMD(Low4);
	}
}
*/
/*
void lcd_Send_Char(uint8_t cmd){

	//Rs = 1
	HAL_GPIO_WritePin(GPIOC, GPIO_Pin_4, GPIO_PIN_SET);
	//R/W = 0 for write
	HAL_GPIO_WritePin(GPIOB, GPIO_Pin_2, GPIO_PIN_RESET);

	writeBit(cmd >> 4);
	writeBit(cmd & 0x0F);
}
void writeBit(uint8_t value){

	HAL_GPIO_WritePin(GPIOB, GPIO_Pin_13, ( (value >> 0) & 0x1) );
	HAL_GPIO_WritePin(GPIOB, GPIO_Pin_14, ( (value >> 1) & 0x1) );
	HAL_GPIO_WritePin(GPIOB, GPIO_Pin_15, ( (value >> 2) & 0x1) );
	HAL_GPIO_WritePin(GPIOB, GPIO_Pin_1,  ( (value >> 3) & 0x1) );

}

void send_Messge(){

	HAL_GPIO_WritePin(GPIOB, GPIO_Pin_11,  GPIO_PIN_SET);
	HAL_Delay(1);
	HAL_GPIO_WritePin(GPIOB, GPIO_Pin_11,  GPIO_PIN_RESET);
	HAL_Delay(1);
}

void lcd_Init(){
	HAL_Delay(20);

	//Set to 4 bit mode
	HAL_GPIO_WritePin(GPIOC, GPIO_Pin_4,  GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOB, GPIO_Pin_2,  GPIO_PIN_RESET);
	writeBit(0x03);
	sendMessage();
	HAL_Delay(5);

	//Set to 4 bit mode
	HAL_GPIO_WritePin(GPIOC, GPIO_Pin_4,  GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOB, GPIO_Pin_2,  GPIO_PIN_RESET);
	writeBit(0x03);
	SendMessage();

	//Set to 4 bit mode
	HAL_GPIO_WritePin(GPIOC, GPIO_Pin_4,  GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOB, GPIO_Pin_2,  GPIO_PIN_RESET);
	writeBit(0x03);
	sendMessage();

	//Set display to On and cursor to blinking
	HAL_GPIO_WritePin(GPIOC, GPIO_Pin_4,  GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOB, GPIO_Pin_2,  GPIO_PIN_RESET);
	writeBit(0x0);
	sendMessage();
	HAL_GPIO_WritePin(GPIOC, GPIO_Pin_4,  GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOB, GPIO_Pin_2,  GPIO_PIN_RESET);
	writeBit(0x0);
	sendMessage();

}
*/
