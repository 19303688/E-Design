/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2022 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under BSD 3-Clause license,
 * the "License"; You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                        opensource.org/licenses/BSD-3-Clause
 *
 ******************************************************************************
 */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <float.h>
#include "LCD_Driver.h"
#include "ADC.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
DAC_HandleTypeDef hdac1;
DMA_HandleTypeDef hdma_dac1_ch1;

TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_TIM2_Init(void);
static void MX_TIM3_Init(void);
static void MX_DAC1_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

// *********************************************** Button Variables ************************************************************************
uint32_t lastTime = 0;

uint8_t leftButton = 0;
uint8_t rightButton = 0;
uint8_t topButton = 0;
uint8_t bottomButton = 0;
uint8_t middleButton = 0;
uint8_t buttonChange = 0;

// *******************************************End of Button Variables **************************************************************************

// ***************************************** Recieve Variables ************************************************************************
uint8_t msg[1];

// ******************************************* End of Recieve **************************************************************************
// ***************************************** Display State ************************************************************************
uint8_t displayMode = 0; //0 = Normal Operation, 1 = Write to LCD
uint8_t firstTime = 0;
// ******************************************* End of display State**************************************************************************

// *******************************************Const Messages ******************************************************************************

char outputOnMesssage[9] = "OUTPUT ON";
char outputOffMesssage[10] = "OUTPUT OFF";
char DCoutput[6] = " .   V";
char sineOutput[26] = "O: .   V,A: .   V,F:    Hz";
char pulseOutput[33] = "O: .   V,A: .   V,F:    Hz,D:   %";
// **************************************** End of Const Messages **************************************************************************

// *******************************************Menu Operation******************************************************************************

uint8_t outputMessage[12];
uint8_t requestState[10];

//0  Depth
uint8_t m_menuOnOff = 0; //0 = menuState, 1 = measure State

//1 Depth
uint8_t m_menuState = 0; //0 = Measure Mode, 1 = Signal Generation
char m_menuStateOptions[2][17] = { "Measure Mode", "Signal Generation" };
uint8_t menuState = 0; // 0 = menu mode; 1 = measure mode;

//1 Depth, 2 Index
uint8_t m_signalGenerationState = 0; //0 = Type, 1 = Parameter, 2 = Output
char m_signalGenerationOptions[3][9] = { "Type", "Parameter", "Output" };

//2 Depthh, 1 Index
uint8_t m_measureModeState = 0; //0 = DCV, 1 = DCC, 2 = ACV, 3 = ACC
char m_measureModeOptions[4][3] = { "DCV", "DCC", "ACV", "ACC" };
uint8_t measureModeState = 0; //0 = DV (DC Voltage), 1 = DI (DC Current), 2 = AC (AC Voltage) , 3 = AI (AC Current)

//3 Depth, 1 Index
uint8_t m_typeState = 0; //0 = DC, 1 = Sine, 2 = Pulse
char m_typeOptions[3][5] = { "DC", "Sine", "Pulse" };
uint8_t signalTypeState = 0; //0 = d (DC), 1 = s (Sinusiodal), 2 = p (Pulse)

//3 Depth, 2 Index
uint8_t m_parameterState = 0; //0 = Amplitude, 1 = Offset, 2 = Frequency, 3 = Duty Cycle
char m_parameterOptions[4][10] = { "Amplitude", "Offset", "Frequency",
		"Duty Cycle" };
uint8_t signalParameterState = 0; //0 = a (Amplitude), 1 = o (Offset), 2 = , 2 = f (Frequency), 3 = d (Duty Cycle)

//3 Depth, 3 Index
uint8_t m_outputState = 0; //0 = On, 1 = Off
char m_outputOptions[4][10] = { "On", "Off" };
uint8_t outputOnOff = 0; //0 = Off, 1 = On //Note that the order is different for this mode

void setMenuValue() {

}
uint8_t moveLeftRight() {
	uint8_t leftRight = 0;

	if (leftButton == 1) {
		leftRight = 1;
		leftButton = 0;
	}
	if (rightButton == 1) {
		leftRight = -1;
		rightButton = 0;
	}
	if (middleButton == 1) {
		setMenuValue();
		middleButton = 0;
	}
	return leftRight;
}
uint8_t moveUpDown() {
	uint8_t upDown = 0;

	if (topButton == 1) {
		upDown = 1;
		topButton = 0;
	}
	if (bottomButton == 1) {
		upDown = 0;
		bottomButton = 0;
	}
	return upDown;
}

uint8_t topScroll = 0;
void displayMeasurements(){
	LCD_Set_Cursor(1, 1);
	switch(measureModeState){

	case 0:
		LCD_Write_String("1.111V"); //"DCV", "DCC", "ACV", "ACC"
		topScroll = 0;
		break;
	case 1:
		LCD_Write_String("O:1.234V,A:5.678V,F:9876Hz"); //AC voltage
		topScroll = 1;
		break;
	case 2:
		LCD_Write_String("123.4mA");
		topScroll = 0;
		break;
	case 3:
		LCD_Write_String("O:123.4mA,A:567.8mA,F:9876Hz"); //AC Current
		topScroll = 1;
		break;

	default:
		break;
	}
}

void updateScreen() {

	clearDisplay();
	if (menuState == 0) { //0 is menu mode
		LCD_Set_Cursor(1, 1);
		LCD_Write_String("Menu Mode");
	} else if (menuState == 1) {
		displayMeasurements();
		LCD_Set_Cursor(2, 1);
		if (outputOnOff == 1) {
			switch (signalTypeState) {
			case 0:
				LCD_Write_String(DCoutput);//Need
				break;
			case 1:
				LCD_Write_String(sineOutput);
				break;
			case 2:
				LCD_Write_String(pulseOutput);
				break;
			default:
				LCD_Write_String("error");

			}
		} else if (outputOnOff == 0) {
			LCD_Write_String(outputOffMesssage);
		}
	}
}

// **************************************** End of Menu Operation **************************************************************************

//For UART Comms
extern int mid;
volatile int midflag;

unsigned char rxBuffer[20];
int uartCounter = 0;
int rxFlag = 0;
uint8_t interpMessage;

uint8_t writeCharFlag = 0; //0 = Do not right, 1 = write char
uint8_t setOutputType = 0; // 3 = nothing, 0 = DC, 1 = AC
uint8_t setOutputAmp = 0;
uint8_t setOutputFreq = 0;
uint8_t outputChange = 0; //1 means run output change function, 0 means do nothing
uint8_t requestStateFlag = 0;
uint8_t flagtemp = 0;
char signalType;

float value = 1;
uint32_t var;
uint32_t dac_freq = 1000; // the freq we want
uint32_t sine_val[100];
uint32_t dac_value[100];
uint32_t sine_value[100];
uint8_t dcCounter = 0; //To determine intial DC value
uint8_t acCounter = 0; //To determine intial AC value
uint8_t otherState = 0;
uint8_t kapFlag = 0;
#define PI 3.1415926
//Fsine wave = TimerFreq/Numbersamples;

void initial_sineval() { //Default, 1kHz, 1V amp, 1.2V Offset
	for (int i = 0; i < 100; i++) {
		sine_val[i] = ((float) (sin(i * 2 * PI / 100)) * ((float) 480 / 3300)
				* (4096)) + ((float) 1165 / (float) 3300) * 4096;
		dac_value[i] = sine_val[i];
	}

}
void initial_dcVal() { //Default 1.2V
	for (int i = 0; i < 100; i++) {
		dac_value[i] = ((float) 1160 / 3300) * 4096;
	}
}
void initial_pulseVal() { //Default 1.2V, 25% duty cycle
	initial_dcVal();
	for (int i = 0; i < 28; i++) {
		dac_value[i] = ((float) 2065 / 3300) * 4096;
	}
}

void SetMeasureState() { //uint8_t uartMessage[]

	//Determine Type of command
	//	if (uartMessage[2] == '$') { //Set Measure Mode Command, : @,$,AI,!\n

	if (rxBuffer[4] == 'D' && rxBuffer[5] == 'V') {
		measureModeState = 0; //Set to DC Measure State
	}
	if (rxBuffer[4] == 'A' && rxBuffer[5] == 'V') {
		measureModeState = 1; //Set to AC Measure State
	}
	if (rxBuffer[4] == 'D' && rxBuffer[5] == 'I') {
		measureModeState = 2; //Set to DC Measure State
	}
	if (rxBuffer[4] == 'A' && rxBuffer[5] == 'I') {
		measureModeState = 3; //Set to DC Measure State
	}
	if (rxBuffer[4] == 'T' && rxBuffer[5] == 'C') {
		measureModeState = 4; //Set to DC Measure State
	}
	//	}
}

uint32_t offset = 1200;
uint32_t amplitude = 1000; //peak to peak
uint32_t freq = 1000;
uint32_t dutyCycle = 25;
void setOutputOff() {
	outputOnOff = 0;
	HAL_DAC_Stop_DMA(&hdac1, DAC_CHANNEL_1);
	//clearDisplay();
	updateScreen();
	//LCD_Set_Cursor(2, 1);
	//LCD_Write_String("Output Off");
}
void setOutputOn() {
	outputOnOff = 1;

	if (signalTypeState == 0) {
		initial_dcVal();
	}

	if (signalTypeState == 1) {
		initial_sineval();
	}
	if (signalTypeState == 2) {
		initial_pulseVal();
	}

	HAL_DAC_Start_DMA(&hdac1, DAC_CHANNEL_1, dac_value, 100, DAC_ALIGN_12B_R);
	//clearDisplay();
	//LCD_Set_Cursor(2, 1);
}
void RequestState() { //This function creates a TX Response via reading the state of the system
	uint8_t response[11];

	response[0] = '@';
	response[1] = ',';
	//Measure Mode Check
	if (measureModeState == 0) {
		response[2] = 'D';
		response[3] = 'V';
	} else if (measureModeState == 1) {
		response[2] = 'A';
		response[3] = 'V';
	} else if (measureModeState == 2) {
		response[2] = 'D';
		response[3] = 'I';
	} else if (measureModeState == 3) {
		response[2] = 'A';
		response[3] = 'I';
	} else if (measureModeState == 4) {
		response[2] = 'T';
		response[3] = 'C';
	}

	response[4] = ',';
	//Signal Parameter Checks
	response[5] = rxBuffer[4];

	response[6] = ',';

	if (outputOnOff == 0) {
		response[7] = '0';
		setOutputOff();
	}
	if (outputOnOff == 1) {
		response[7] = '1';
		setOutputOn();
	}
	response[8] = ',';
	response[9] = '!';
	response[10] = '\n';

	HAL_UART_Transmit(&huart2, (uint8_t*) response, 11, 50);

}
void changeAmplitude() {
	uint32_t temp = 0;
	temp = (uint32_t) (rxBuffer[6] - 48) * 1000; //First entry
	temp = temp + (uint32_t) (rxBuffer[7] - 48) * 100;
	temp = temp + (uint32_t) (rxBuffer[8] - 48) * 10;
	temp = temp + (uint32_t) (rxBuffer[9] - 48);

	amplitude = temp;

	if (signalTypeState == 1) { //AC amplitude
		for (int i = 0; i < 100; i++) {

				dac_value[i] = ((float) (sin(i * 2 * PI / 100))
						* ((float) ((amplitude/2)-15) / 3300) * (4096))
						+ ((float) offset / (float) 3300) * 4096;
			//}

		}
	}
	if (signalTypeState == 2){
		for (int i = 0; i < 100; i++) {
			dac_value[i] = ((float) 1160 / 3300) * 4096;
		}
		for (int i = 0; i < (dutyCycle); i++) {
			dac_value[i] = ((float) (offset+amplitude) / 3300) * 4096;
		}
	}
}

void changeOffset() {

	uint32_t temp = 0;
	temp = (uint32_t) (rxBuffer[6] - 48) * 1000; //First entry
	temp = temp + (uint32_t) (rxBuffer[7] - 48) * 100;
	temp = temp + (uint32_t) (rxBuffer[8] - 48) * 10;
	temp = temp + (uint32_t) (rxBuffer[9] - 48);
	offset = temp;

	if (signalTypeState == 0) { //DC
		for (int i = 0; i < 100; i++) {
			dac_value[i] = (offset * 1.18 + i * 0.005);
		}

	}
	if (signalTypeState == 1) { //AC
		for (int i = 0; i < 100; i++) {

			dac_value[i] = ((float) (sin(i * 2 * PI / 100))
					* ((float) 470 / 3300) * (4096))
					+ ((float) offset / (float) 3300) * 4096;
		}
	}
	if (signalTypeState == 2){
		for (int i = 0; i < 100; i++) {
			dac_value[i] = ((float) offset / 3300) * 4096;
		}
		for (int i = 0; i < (dutyCycle); i++) {
			dac_value[i] = ((float) (offset+amplitude) / 3300) * 4096;
		}
	}
}
void Freq_Get_Prescalar() {
	uint32_t autoreload_val = 1;
	autoreload_val = (720000 / dac_freq) - 1;

	__HAL_TIM_SET_AUTORELOAD(&htim2, autoreload_val);

	TIM2->CR1 = 0; //disable the timer
	TIM2->EGR = TIM_EGR_UG; //reintialize all  registers
	TIM2->CR1 |= TIM_CR1_CEN; //start timer again

}
void changeFreq() {
	int temp = 0;
	//change amplitude of wave
	temp = (uint32_t) (rxBuffer[6] - 48) * 1000; //First entry
	temp = temp + (uint32_t) (rxBuffer[7] - 48) * 100;
	temp = temp + (uint32_t) (rxBuffer[8] - 48) * 10;
	temp = temp + (uint32_t) (rxBuffer[9] - 48);
	freq = temp;

	dac_freq = temp;

	Freq_Get_Prescalar();

}
void changeDutyCycle(){
	uint32_t temp = 0;
	temp = (uint32_t) (rxBuffer[6] - 48) * 1000; //First entry
	temp = temp + (uint32_t) (rxBuffer[7] - 48) * 100;
	temp = temp + (uint32_t) (rxBuffer[8] - 48) * 10;
	temp = temp + (uint32_t) (rxBuffer[9] - 48);
	dutyCycle = temp;
	for (int i = 0; i < 100; i++) {
		dac_value[i] = ((float) offset / 3300) * 4096;
	}
	for (int i = 0; i < (dutyCycle); i++) {
		dac_value[i] = ((float) (offset+amplitude) / 3300) * 4096;
	}

}
void setSignalType() {
	if (rxBuffer[6] == 'd') {
		//Set DAC in DC config
		signalTypeState = 0;
	}
	if (rxBuffer[6] == 's') {
		//Set DAC in sine config
		signalTypeState = 1;
	}
	if (rxBuffer[6] == 'p') {
		signalTypeState = 2;
		//Set DAC in pulse config
	}
	if (outputOnOff == 1){
		setOutputOn();
	}
}

void SetOutputType() {

	//@,*,m,t,!\n
	// a, o, f, d, c, t (Type, Amplitude, Offset, Frequency, Duty cycle, Temperature
	//what we work with
	////0 = t (Type), 1 = a (Amplitude), 2 = o (Offset), 3 = f (Frequency), 4 = d (Duty Cycle), 5 = c (Temperature)
	//signalParameterState
	if (rxBuffer[4] == 't') { // Set Type
		setSignalType();
		//signalTypeState
		//signalParameterState = 0;
		//HAL_UART_Transmit(&huart2, (uint8_t*) "OutPut Type is type", 19, 50);
	}
	if (rxBuffer[4] == 'a') { // Set Amplitude
		signalParameterState = 1;
		changeAmplitude();

		//HAL_UART_Transmit(&huart2, (uint8_t*) "Measure Mode is ampl", 20, 50);
	}
	if (rxBuffer[4] == 'o') { // Set Offset
		signalParameterState = 2;
		changeOffset();
		//HAL_UART_Transmit(&huart2, (uint8_t*) "Measure Mode is Of", 18, 50);
	}
	if (rxBuffer[4] == 'f') {  // Set Frequency
		signalParameterState = 3;
		changeFreq();

		//HAL_UART_Transmit(&huart2, (uint8_t*) "Measure Mode is Fr", 18, 50);
	}
	if (rxBuffer[4] == 'd') { // Set Duty Cycle
		//HAL_UART_Transmit(&huart2, (uint8_t*) "Measure Mode is DutyC", 21, 50);
		signalParameterState = 4;
		changeDutyCycle();
	}
	if (rxBuffer[4] == 'c') { // Set Temperature
		//HAL_UART_Transmit(&huart2, (uint8_t*) "Measure Mode is Temp", 20, 50);
		signalParameterState = 5;
	}
}

uint8_t uartRecieve = 0;
uint8_t measureStateFlag = 0;
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {

	if (msg[0] == '@') {
		for (int x = 0; x < 20; x++) {
			rxBuffer[x] = 0; //Clear buffer on new input command
		}
		rxFlag = 1;
	}
	if (rxFlag == 1) {
		rxBuffer[uartCounter] = msg[0];
		uartCounter++;
	}
	if (msg[0] == '\n') {
		uartRecieve = 1;
		rxFlag = 0;
		uartCounter = 0;

		if (rxBuffer[2] == '$') { //Set State
			measureStateFlag = 1;
		}
		if (rxBuffer[2] == '#') { //LCD Command/Write Char
			writeCharFlag = 1; //@,#,1,H,!\n -> Display H on LCD
			displayMode = 1;
		}
		if (rxBuffer[2] == '^') {
			kapFlag = 1;
			displayMode = 0;

		}
		if (rxBuffer[2] == '*') //REQUEST STATUS
		{
			displayMode = 0;
			if (rxBuffer[6] == '0') {
				outputOnOff = 0;
			}
			if (rxBuffer[6] == '1') {
				outputOnOff = 1;
				if (rxBuffer[4] == 'd') {
					signalTypeState = 0;
				}
				if (rxBuffer[4] == 's') {
					signalTypeState = 1;
				}
				if (rxBuffer[4] == 'p') {
					signalTypeState = 2;
				}
			}
			requestStateFlag = 1;
			//RequestState(); // Request Format: @,*,s,0,!\n Response Format: @,DV,p,1,!\n

			if (rxBuffer[4] == 'm') {
				if (measureModeState == 0) //DC State
				{
					//DC_Measure();
				}
				if (measureModeState == 1) //AC State, Request format @,*,m,f,!\n
				{
					//AC_Measure(rxBuffer);
				}
			}
			uartRecieve = 1;
		}
	}

	HAL_UART_Receive_IT(&huart2, (uint8_t*) msg, 1);
}

void debugLEDs() {
	if (menuState == 0) {
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, 1);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, 0);
	} else if (menuState == 1) {
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, 0);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, 1);
	}
	if (measureModeState == 2 || measureModeState == 3){ //Display if the system is in current measurement mode
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, 1);
	} else{
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, 0);
	}
	if (outputOnOff == 0) { //1 for output on.
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, 0);
	} else if (outputOnOff == 1) {
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, 1);
	}
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
	/* Prevent unused argument(s) compilation warning */
	UNUSED(GPIO_Pin);
	displayMode = 0;
	firstTime = 0;

	if (GPIO_Pin == GPIO_PIN_1 && HAL_GetTick() - lastTime > 100) { //Left Button
		lastTime = HAL_GetTick();
		leftButton = 1;
	}
	if (GPIO_Pin == GPIO_PIN_10 && HAL_GetTick() - lastTime > 100) { //Right Button
		lastTime = HAL_GetTick(); //D4
		rightButton = 1;
	}
	if (GPIO_Pin == GPIO_PIN_3 && HAL_GetTick() - lastTime > 50) { //Middle Button
		lastTime = HAL_GetTick(); //D4
		menuState = 1 - menuState;
		middleButton = 1;
	}
	if (GPIO_Pin == GPIO_PIN_0 && HAL_GetTick() - lastTime > 100) { //Top Button
		lastTime = HAL_GetTick(); //D3
		topButton = 1;
	}
	if (GPIO_Pin == GPIO_PIN_2 && HAL_GetTick() - lastTime > 100) { //Bottom Button
		lastTime = HAL_GetTick(); //D4
		bottomButton = 1;
	}

	buttonChange = 1;
}

/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void) {
	/* USER CODE BEGIN 1 */

	/* USER CODE END 1 */

	/* MCU Configuration--------------------------------------------------------*/

	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();

	/* USER CODE BEGIN Init */

	/* USER CODE END Init */

	/* Configure the system clock */
	SystemClock_Config();

	/* USER CODE BEGIN SysInit */

	/* USER CODE END SysInit */

	/* Initialize all configured peripherals */
	MX_GPIO_Init();
	MX_DMA_Init();
	MX_USART2_UART_Init();
	MX_TIM2_Init();
	MX_TIM3_Init();
	MX_DAC1_Init();
	/* USER CODE BEGIN 2 */

	HAL_UART_Transmit(&huart2, (uint8_t*) "@,19303688,!\n", 13, 50);
	HAL_UART_Receive_IT(&huart2, (uint8_t*) msg, 1);

	lcd_Init();

	HAL_TIM_Base_Start(&htim2);
	Freq_Get_Prescalar();

	//char test[] = "test";

	updateScreen();
	lastTime = HAL_GetTick();
	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1) {

		if (uartRecieve == 1){
			updateScreen();
			uartRecieve = 0;
		}

		if (measureStateFlag == 1 ){
			//SetMeasureState(rxBuffer); //Input Format: : @,$,AV,!\n. No Response
			SetMeasureState();
			updateScreen();
			measureStateFlag = 0;
		}

		debugLEDs();

		//if(topScroll == 1){

		//}

		if (kapFlag == 1) {
			SetOutputType();
			kapFlag = 0;
		}
		if (requestStateFlag == 1) {
			RequestState();
			requestStateFlag = 0;
		}

		if (buttonChange == 1) {
			updateScreen();
			uartRecieve = 0;
			buttonChange = 0;
		}
	}
	//HAL_DAC_SetValue(&hdac1, DAC_CHANNEL_1, DAC_ALIGN_12B_R, var);

	/* USER CODE END WHILE */

	/* USER CODE BEGIN 3 */

	/* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void) {
	RCC_OscInitTypeDef RCC_OscInitStruct = { 0 };
	RCC_ClkInitTypeDef RCC_ClkInitStruct = { 0 };
	RCC_PeriphCLKInitTypeDef PeriphClkInit = { 0 };

	/** Initializes the CPU, AHB and APB busses clocks
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
	RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
	RCC_OscInitStruct.PLL.PREDIV = RCC_PREDIV_DIV1;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
		Error_Handler();
	}
	/** Initializes the CPU, AHB and APB busses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
			| RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK) {
		Error_Handler();
	}
	PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART2
			| RCC_PERIPHCLK_TIM2 | RCC_PERIPHCLK_TIM34;
	PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
	PeriphClkInit.Tim2ClockSelection = RCC_TIM2CLK_HCLK;
	PeriphClkInit.Tim34ClockSelection = RCC_TIM34CLK_HCLK;
	if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK) {
		Error_Handler();
	}
}

/**
 * @brief DAC1 Initialization Function
 * @param None
 * @retval None
 */
static void MX_DAC1_Init(void) {

	/* USER CODE BEGIN DAC1_Init 0 */

	/* USER CODE END DAC1_Init 0 */

	DAC_ChannelConfTypeDef sConfig = { 0 };

	/* USER CODE BEGIN DAC1_Init 1 */

	/* USER CODE END DAC1_Init 1 */
	/** DAC Initialization
	 */
	hdac1.Instance = DAC1;
	if (HAL_DAC_Init(&hdac1) != HAL_OK) {
		Error_Handler();
	}
	/** DAC channel OUT1 config
	 */
	sConfig.DAC_Trigger = DAC_TRIGGER_T2_TRGO;
	sConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;
	if (HAL_DAC_ConfigChannel(&hdac1, &sConfig, DAC_CHANNEL_1) != HAL_OK) {
		Error_Handler();
	}
	/* USER CODE BEGIN DAC1_Init 2 */

	/* USER CODE END DAC1_Init 2 */

}

/**
 * @brief TIM2 Initialization Function
 * @param None
 * @retval None
 */
static void MX_TIM2_Init(void) {

	/* USER CODE BEGIN TIM2_Init 0 */

	/* USER CODE END TIM2_Init 0 */

	TIM_ClockConfigTypeDef sClockSourceConfig = { 0 };
	TIM_MasterConfigTypeDef sMasterConfig = { 0 };

	/* USER CODE BEGIN TIM2_Init 1 */

	/* USER CODE END TIM2_Init 1 */
	htim2.Instance = TIM2;
	htim2.Init.Prescaler = 1 - 1;
	htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim2.Init.Period = 100 - 1;
	htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
	if (HAL_TIM_Base_Init(&htim2) != HAL_OK) {
		Error_Handler();
	}
	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK) {
		Error_Handler();
	}
	sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig)
			!= HAL_OK) {
		Error_Handler();
	}
	/* USER CODE BEGIN TIM2_Init 2 */

	/* USER CODE END TIM2_Init 2 */

}

/**
 * @brief TIM3 Initialization Function
 * @param None
 * @retval None
 */
static void MX_TIM3_Init(void) {

	/* USER CODE BEGIN TIM3_Init 0 */

	/* USER CODE END TIM3_Init 0 */

	TIM_ClockConfigTypeDef sClockSourceConfig = { 0 };
	TIM_MasterConfigTypeDef sMasterConfig = { 0 };

	/* USER CODE BEGIN TIM3_Init 1 */

	/* USER CODE END TIM3_Init 1 */
	htim3.Instance = TIM3;
	htim3.Init.Prescaler = 8000;
	htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim3.Init.Period = 10;
	htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	if (HAL_TIM_Base_Init(&htim3) != HAL_OK) {
		Error_Handler();
	}
	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK) {
		Error_Handler();
	}
	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig)
			!= HAL_OK) {
		Error_Handler();
	}
	/* USER CODE BEGIN TIM3_Init 2 */

	/* USER CODE END TIM3_Init 2 */

}

/**
 * @brief USART2 Initialization Function
 * @param None
 * @retval None
 */
static void MX_USART2_UART_Init(void) {

	/* USER CODE BEGIN USART2_Init 0 */

	/* USER CODE END USART2_Init 0 */

	/* USER CODE BEGIN USART2_Init 1 */

	/* USER CODE END USART2_Init 1 */
	huart2.Instance = USART2;
	huart2.Init.BaudRate = 115200;
	huart2.Init.WordLength = UART_WORDLENGTH_8B;
	huart2.Init.StopBits = UART_STOPBITS_1;
	huart2.Init.Parity = UART_PARITY_NONE;
	huart2.Init.Mode = UART_MODE_TX_RX;
	huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart2.Init.OverSampling = UART_OVERSAMPLING_16;
	huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
	huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
	if (HAL_UART_Init(&huart2) != HAL_OK) {
		Error_Handler();
	}
	/* USER CODE BEGIN USART2_Init 2 */

	/* USER CODE END USART2_Init 2 */

}

/**
 * Enable DMA controller clock
 */
static void MX_DMA_Init(void) {

	/* DMA controller clock enable */
	__HAL_RCC_DMA1_CLK_ENABLE();

	/* DMA interrupt init */
	/* DMA1_Channel3_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(DMA1_Channel3_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(DMA1_Channel3_IRQn);

}

/**
 * @brief GPIO Initialization Function
 * @param None
 * @retval None
 */
static void MX_GPIO_Init(void) {
	GPIO_InitTypeDef GPIO_InitStruct = { 0 };

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOF_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOA,
			LD2_Pin | D2_Pin | D3_Pin | D5_Pin | DB4_Pin | DB5_Pin, GPIO_PIN_RESET);

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOC, Rs_Pin | DB6_Pin | D4_Pin | DB7_Pin,
			GPIO_PIN_RESET);

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOB, RNW_Pin | E_Pin, GPIO_PIN_RESET);

	/*Configure GPIO pin : B1_Pin */
	GPIO_InitStruct.Pin = B1_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

	/*Configure GPIO pins : left_Button_Pin down_Button_Pin middle_Button_Pin */
	GPIO_InitStruct.Pin = left_Button_Pin | down_Button_Pin | middle_Button_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

	/*Configure GPIO pins : LD2_Pin D2_Pin D3_Pin D5_Pin */
	GPIO_InitStruct.Pin = LD2_Pin | D2_Pin | D3_Pin | D5_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	/*Configure GPIO pins : Rs_Pin D4_Pin */
	GPIO_InitStruct.Pin = Rs_Pin | D4_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

	/*Configure GPIO pins : top_Button_Pin right_Button_Pin */
	GPIO_InitStruct.Pin = top_Button_Pin | right_Button_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	/*Configure GPIO pins : RNW_Pin E_Pin */
	GPIO_InitStruct.Pin = RNW_Pin | E_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	/*Configure GPIO pins : DB6_Pin DB7_Pin */
	GPIO_InitStruct.Pin = DB6_Pin | DB7_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

	/*Configure GPIO pins : DB4_Pin DB5_Pin */
	GPIO_InitStruct.Pin = DB4_Pin | DB5_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	/*Configure GPIO pins : PB6 PB7 */
	GPIO_InitStruct.Pin = GPIO_PIN_6 | GPIO_PIN_7;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF4_I2C1;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	/* EXTI interrupt init*/
	HAL_NVIC_SetPriority(EXTI0_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(EXTI0_IRQn);

	HAL_NVIC_SetPriority(EXTI1_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(EXTI1_IRQn);

	HAL_NVIC_SetPriority(EXTI2_TSC_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(EXTI2_TSC_IRQn);

	HAL_NVIC_SetPriority(EXTI3_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(EXTI3_IRQn);

	HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void) {
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */

	/* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line)
{ 
	/* USER CODE BEGIN 6 */
	/* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
	/* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
