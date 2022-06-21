/*
 * LCD_Driver.c
 *
 *  Created on: Apr 18, 2022
 *      Author: Calvin
 */

#include "Flow_Helpers.h"
#include "main.h"
#include <string.h>
#include <stdio.h>
/*
extern int measureModeState;
void SetState(uint8_t uartMessage[]){

	//Determine Type of command
	if (uartMessage[2] == '$'){ //Set Measure Mode Command, : @,$,AI,!\n

		if (uartMessage[4] == 'D' && uartMessage[5] == 'V'){
			measureModeState = 0; //Set to DC Measure State
		}
		if (uartMessage[4] == 'A' && uartMessage[5] == 'V'){
			measureModeState = 1; //Set to AC Measure State
		}
		if (uartMessage[4] == 'D' && uartMessage[5] == 'I'){
			measureModeState = 2; //Set to DC Measure State
		}
		if (uartMessage[4] == 'A' && uartMessage[5] == 'I'){
			measureModeState = 3; //Set to DC Measure State
		}
		if (uartMessage[4] == 'T' && uartMessage[5] == 'C'){
			measureModeState = 4; //Set to DC Measure State
		}
	}
}
*/
