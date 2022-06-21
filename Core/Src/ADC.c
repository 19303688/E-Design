/*
 * ADC.c
 *
 *  Created on: 21 Apr 2022
 *      Author: Calvin
 */


#include "LCD_Driver.h"
#include "main.h"
#include "ADC.h"
#include <string.h>
#include <stdio.h>

/*
float ADCPoll(){

	HAL_ADC_Start(&hadc1);
	HAL_ADC_PollForConversion(&hadc1, 10);

	uint16_t vref = HAL_ADC_GetValue(&hadc1);
	float voltage = ((float)vref / (4095))*3.3;
	HAL_ADC_Stop(&hadc1);
	return voltage;
}

float DC_Calculator(){
	HAL_TIM_Base_Start_IT(&htim2);
	uint16_t voltage[1000];
	uint16_t cumaltiveSumVolt = 0;
	int i = 0;
	for (i = 0; i <= 1000; i++){
	  voltage[i] = ADCPoll();
	  cumaltiveSumVolt = cumaltiveSumVolt + voltage[i];
	}

	float DC_Voltage = (cumaltiveSumVolt/1000);
	return DC_Voltage;
}

void DC_Measure(){
	uint8_t response[13]; //Request Format: @,*,m,f,!\n Response Format: @,m,a,0123,!\n

	response[0] = '@';
	response[1] = ',';
	response[2] = 'm';
	response[3] = ',';
	response[4] = 'a';
	response[5] = ',';

	response[6] = '1';
	response[7] = '2';
	response[8] = '0';
	response[9] = '0';

	response[10] = ',';
	response[11] = '!';
	response[12] = '\n';

	HAL_UART_Transmit(&huart2, (uint8_t*) response, 13, 50);
}

void AC_Measure(uint8_t uartMessage[]){
	uint8_t response[13]; //Request Format: @,*,m,a,!\n Response Format: @,m,a,0123,!\n

	response[0] = '@';
	response[1] = ',';
	response[2] = 'm';
	response[3] = ',';
	//This needs to be changed to measurements with the DAC
	if (uartMessage[6] == 'a'){
		response[4] = 'a';
		response[6] = '1';
		response[7] = '2';
		response[8] = '0';
		response[9] = '0';
	}
	if (uartMessage[6] == 'o'){
		response[4] = 'o';
		response[6] = '0';
		response[7] = '5';
		response[8] = '0';
		response[9] = '0';
	}
	if (uartMessage[6] == 'f'){
		response[4] = 'f';
		response[6] = '5';
		response[7] = '2';
		response[8] = '5';
		response[9] = '0';
	}
	if (uartMessage[6] == 'd'){
		response[4] = 'd';
		response[6] = '0';
		response[7] = '0';
		response[8] = '5';
		response[9] = '0';
	}
	if (uartMessage[6] == 'c'){
		response[4] = 'c';
		response[6] = '0';
		response[7] = '0';
		response[8] = '2';
		response[9] = '5';
	}

	response[5] = ',';
	response[10] = ',';
	response[11] = '!';
	response[12] = '\n';

	HAL_UART_Transmit(&huart2, (uint8_t*) response, 13, 50);
}
*/
