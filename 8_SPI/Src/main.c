/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
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
#include "spi.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

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

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

#define SPI_NSS_HIGH();  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_2,GPIO_PIN_SET);
#define SPI_NSS_LOW();   HAL_GPIO_WritePin(GPIOA,GPIO_PIN_2,GPIO_PIN_RESET);
#define MY_HSPI          hspi1

/* W25Q64的指令 */
uint8_t w25x_read_id = 0x90;					// 读ID
uint8_t m_addr[3]    = {0,0,0};					// 测试地址0x000000
uint8_t check_addr   = 0x05;					// 检查线路是否繁忙
uint8_t enable_write = 0x06;					// 使能了才能改变芯片数据
uint8_t erase_addr   = 0x20;					// 擦除命令
uint8_t write_addr   = 0x02;					// 写数据命令
uint8_t read_addr    = 0x03;					// 读数据命令

uint8_t temp_ID[5] = {0,0,0,0,0};						// 接收缓存
uint8_t temp_wdata[5] = {0x99,0x88,0x77,0x66,0x55};		// 需要写入的数据
uint8_t temp_rdata[5] = {0,0,0,0,0};					// 读出数据保存的buff
/* 读ID */
void ReadID(void)
{
	SPI_NSS_LOW();	                                        // 使能CS
	HAL_SPI_Transmit(&MY_HSPI, &w25x_read_id, 1, 100);		// 读ID发送指令
	HAL_SPI_Receive(&MY_HSPI, temp_ID, 5, 100);				// 读取ID
	SPI_NSS_HIGH();	                                        // 失能CS
}

/* 检查是否繁忙 */
void CheckBusy(void)
{
	uint8_t status=1;
	uint32_t timeCount=0;
	do
	{
		timeCount++;
		if(timeCount > 0xEFFFFFFF) //等待超时
		{
			return ;
		}
		
		SPI_NSS_LOW();	                                            // 使能CS	
		HAL_SPI_Transmit(&MY_HSPI, &check_addr, 1, 100);			// 发送指令
		HAL_SPI_Receive(&MY_HSPI, &status, 1, 100);				    // 读取
		SPI_NSS_HIGH();                                             // 失能CS
	}while((status&0x01)==0x01);
}


/* 写入数据 */
void WriteData(void)
{
	/* 检查是否繁忙 */
	CheckBusy();
	
	/* 写使能 */
	SPI_NSS_LOW();	                                        // 使能CS
	HAL_SPI_Transmit(&MY_HSPI, &enable_write, 1, 100);		// 发送指令
	SPI_NSS_HIGH(); 	                                    // 失能CS
	
	/* 擦除 */
	SPI_NSS_LOW();	                                        // 使能CS
	HAL_SPI_Transmit(&MY_HSPI, &erase_addr, 1, 100);		// 发送指令
	HAL_SPI_Transmit(&MY_HSPI, m_addr, 3, 100);				// 发送地址
	SPI_NSS_HIGH(); 	                                    // 失能CS
	
	/* 再次检查是否繁忙 */
	CheckBusy();
	
	/* 写使能 */
	SPI_NSS_LOW();	                                        // 使能CS
	HAL_SPI_Transmit(&MY_HSPI, &enable_write, 1, 100);		// 发送指令
	SPI_NSS_HIGH(); 	                                    // 失能CS

	/* 写数据 */
	SPI_NSS_LOW();	                                        // 使能CS
	HAL_SPI_Transmit(&MY_HSPI, &write_addr, 1, 100);		// 发送指令
	HAL_SPI_Transmit(&MY_HSPI, m_addr, 3, 100);				// 地址
	HAL_SPI_Transmit(&MY_HSPI, temp_wdata, 5, 100);			// 写入数据
	SPI_NSS_HIGH(); 	                                    // 失能CS
}

/* 读取数据 */
void ReadData(void)
{
	/* 检查是否繁忙 */
	CheckBusy();	
	
	/* 开始读数据 */
	SPI_NSS_LOW();	                                        // 使能CS
	HAL_SPI_Transmit(&MY_HSPI, &read_addr, 1, 100);			// 读发送指令
	HAL_SPI_Transmit(&MY_HSPI, m_addr, 3, 100);				// 地址
	HAL_SPI_Receive(&MY_HSPI,  temp_rdata, 5, 100);			// 拿到数据
	SPI_NSS_HIGH(); 	                                    // 失能CS
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
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
  MX_SPI1_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
	ReadID();
	WriteData();
	ReadData();

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
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
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
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
