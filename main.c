/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "string.h"

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);

/* LCD Function prototypes */
void LCD_init(void);
void LCD(uint8_t data, uint8_t cmd);
void LCD_clear(void);
void LCD_cursor(uint8_t line, uint8_t lcd_type);
void LCD_print(char *data, uint8_t lcd_type);

/* Private user code ---------------------------------------------------------*/
/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* MCU Configuration--------------------------------------------------------*/
  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();
	
  /* Configure the system clock */
  SystemClock_Config();

  /* Initialize all configured peripherals */
  MX_GPIO_Init();

	/* Initialize LCD screen */
	LCD_init();
	
	/* Infinite Loop */
  while (1)
  {
		// Move cursor to line 1
		LCD_cursor(1, 0);
		LCD_print("PARK AT LOCATION:", 0);
		HAL_Delay(250);
	
		// Move cursor to line 2
		LCD_cursor(2, 0);
		LCD_print("SECTOR A ", 0);
		HAL_Delay(750);
		
		// clear screen
		LCD_clear();
  }

	/* End of Main */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3
                          |GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7
                          |GPIO_PIN_8|GPIO_PIN_9, GPIO_PIN_RESET);

  /*Configure GPIO pins : PA0 PA1 PA2 PA3
                           PA4 PA5 PA6 PA7
                           PA8 PA9 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3
                          |GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7
                          |GPIO_PIN_8|GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

}


/******************************
	 * LCD Function Definitions *
*******************************/
void LCD_init(void){
	LCD(0x38, 0); 				// set Lcd to 2 lines 5*7 matrix 
	LCD(0x0C, 0);					// Display on, cursor off
	LCD(0x06, 0);					// Increment cursor in right direction
	LCD(0x01, 0);					// Clear Display Screen
	LCD(0x80, 0);					// Force Cursor to the beginning of LCD
}


void LCD(uint8_t data, uint8_t cmd){
	/*
	set Rs = cmd
	if cmd is low then it is command
	if cmd is high then it is data
	*/
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, cmd);
	
	/* Mask lower bits and assign it to respective GPIO pins */
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0,  data & 0x01 );
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, (data>>1) & 0x01 );
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, (data>>2) & 0x01 );
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, (data>>3) & 0x01 );
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, (data>>4) & 0x01 );
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, (data>>5) & 0x01 );
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, (data>>6) & 0x01 );
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, (data>>7) & 0x01 );
	
	/*After providing data, pulse is needed as invoking signal for LCD*/
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_SET);
	HAL_Delay(5);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_RESET);
	
	/*Command executed*/
}


void LCD_clear(void){
	/* Send Instruction to clear the LCD screen*/
	LCD(0x01, 0);
}

void LCD_cursor(uint8_t line, uint8_t lcd_type){
	/* for 16*2 LCD, Type 0 */
	if (lcd_type == 0){
		if (line==1)
			LCD(0x80, 0);
		else if (line==2)
			LCD(0xC0, 0);
	} 
	
	/* For 20*4 LCD,Type 1 */ 
	else {
		if (line == 1)
			LCD(0x80, 0);
		else if (line == 2)
			LCD(0x94, 0);
		else if (line == 3)
			LCD(0xC0, 0);
		else if (line == 4)
			LCD(0xD4, 0);
	}
}

void LCD_print(char *data, uint8_t lcd_type){
	uint8_t i;						// iterator

	for(i=0; i<strlen(data); i++)
		LCD(data[i], 1);
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
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
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

