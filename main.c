/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <string.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
typedef struct
{
    uint8_t *buffer;
    uint8_t sizeBuff;
    uint8_t head;
    uint8_t tail;

} ringBuffer_t;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define sizeofBuff 40
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */

uint8_t tempbuff [sizeofBuff];
ringBuffer_t ringBuf1;
uint8_t rxBuf[sizeofBuff];
uint8_t txBuf [sizeofBuff];
uint8_t sizeRead;
uint8_t sizeWrite;
uint8_t sizeReceive;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
/* USER CODE BEGIN PFP */

uint8_t getByteToWriteToRingBuffer (ringBuffer_t *ringbuff);
uint8_t getByteFromRingBufferAvailableToSend (ringBuffer_t *ringbuff);
uint8_t readFromRingBuffer(ringBuffer_t *ringbuff);
void WriteToRingBuffer(ringBuffer_t *ringbuff);

uint8_t getChar(void);
void uart_tx(uint8_t *data, uint8_t size)
{
    for (uint8_t i = 0; i < size; i++)
    {
        USART2 ->DR = *(data +i);
        while (!(USART2 ->SR & (uint32_t )(1 << 6)));//wait TC == 1
        
    }
}
void putChar(uint8_t c)
{
    USART2 ->DR = c;
    while (!(USART2 ->SR & (uint32_t )(1 << 6)));
}
void uart_rx(uint8_t *data, uint8_t size)
{
    sizeReceive = 0;
    for (uint8_t i =0; i < size; i ++)
    {
        *(data + i) = getChar();
        sizeReceive = i;
        if ( USART2 ->SR & (1 << 4)) // detect idle
        { 
            break;            
        }
    }       
}
uint8_t getChar(void)
 {
    uint8_t t;
    while (!(USART2->SR & (1 << 5))){}
    t = USART2 ->DR;
    return t;        
 }     
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

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
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
  
    ringBuf1.buffer = tempbuff;
    
    ringBuf1.sizeBuff = sizeof( tempbuff);
    
    ringBuf1.head = 0; 
    
    ringBuf1.tail = 0;
    
//    USART2 ->CR1 |= 1 << 4; // IDLE INTERRUPT ENABLE
    USART2 ->CR1 |= 1 << 5; // RXNE ENABLE    
    
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    while (getByteFromRingBufferAvailableToSend(&ringBuf1))
    {
        uint8_t read = readFromRingBuffer (&ringBuf1);
        putChar (read);        
    }
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

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
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
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

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
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();

}

/* USER CODE BEGIN 4 */
void RXCallback(void)
{
    
    WriteToRingBuffer(&ringBuf1);

}

uint8_t  getByteToWriteToRingBuffer (ringBuffer_t* ringBuf)
{
    
    if(ringBuf->head > ringBuf->tail)
    {
        sizeRead = ringBuf ->sizeBuff -  (ringBuf ->head - ringBuf->tail);            
    }
    else if (ringBuf->head == ringBuf->tail)
    {
        sizeRead = ringBuf ->sizeBuff;
    }
    else
    {
        sizeRead = ringBuf->tail - ringBuf->head ;
    }
    return sizeRead - 1;
}

uint8_t  getByteFromRingBufferAvailableToSend (ringBuffer_t* ringBuf)
{
    if(ringBuf ->head > ringBuf ->tail)
    {
        sizeWrite = ringBuf ->head - ringBuf ->tail;        
    
    }
    else if (ringBuf ->head == ringBuf ->tail)
    {
        sizeWrite = 0;
    }
    else
    {
        sizeWrite = ringBuf ->sizeBuff - ringBuf ->tail + ringBuf ->head ;
    }
    return sizeWrite;
}
uint8_t readFromRingBuffer(ringBuffer_t *ringbuff)
{
    uint8_t data;
    data = ringbuff ->buffer[ringbuff ->tail];
    ringbuff ->tail++;
    if (ringbuff ->tail == ringbuff ->sizeBuff)
    {
        ringbuff ->tail = 0;
    }
    return data;
}

void WriteToRingBuffer(ringBuffer_t *ringbuff)
{
        if(getByteToWriteToRingBuffer(ringbuff))
        {
            ringbuff->buffer[ringbuff ->head ] = getChar();
            ringbuff->head++;
            if(ringbuff ->head == ringbuff ->sizeBuff)
            {
                ringbuff ->head = 0;
            }
        }  
}
  


/* USER CODE END 4 */

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

