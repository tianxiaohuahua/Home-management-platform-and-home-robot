/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
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
#include "adc.h"
#include "dma.h"
#include "i2c.h"
#include "spi.h"
#include "usart.h"
#include "gpio.h"
#include "lcd.h"
#include "stm32_pca9685.h"
#include "tim.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#ifdef __GNUC__
/* With GCC, small printf (option LD Linker->Libraries->Small printf
   set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */
 
/* Private functions -------------------------------------------------*/
/* USER CODE END Includes */
PUTCHAR_PROTOTYPE
{
    HAL_UART_Transmit(&huart1 , (uint8_t *)&ch, 1, 0xFFFF);
    return ch;
}
/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
uint8_t aRxBuffer;   //�����жϻ���
uint8_t Uart1_RxBuff[256];  //���ջ���
uint8_t Uart1_Rx_Cnt = 0;  //���ջ������

uint8_t Reve_Data[256];
uint8_t Reve_Order;

uint16_t ADC1_Value;

//uint8_t cAlmStr[] = "�������(����256)\r\n";
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

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
	uint16_t a_value = 35,i;
	int value_PWM;
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
  MX_USART1_UART_Init();
  MX_ADC1_Init();
  MX_I2C1_Init();
  MX_SPI1_Init();
	MX_TIM2_Init();
	PCA_MG9XX_Init(60,90);//��ʼ���������
  /* USER CODE BEGIN 2 */
	HAL_UART_Receive_IT(&huart1, (uint8_t *)&aRxBuffer, 1); //���������жϣ��������մ��ڷ��͹�������Ϣ
	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1); //������ʱ��PWMͨ�� ��ʱ��2 ͨ��1
	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3); //������ʱ��PWMͨ�� ��ʱ��2 ͨ��3
	__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 0); //����PWM��ռ�ձȣ������ʼ��ռ�ձ�Ϊ0
	__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, 0); //����PWM��ռ�ձȣ������ʼ��ռ�ձ�Ϊ0
	
  /* USER CODE END 2 */
	/*��Ļ��Ϊ��·ԭ��Ӵ���������ʼ��������Ļ*/
	HAL_Delay(500);
	printf("��ʼ��led \r\n");
	Lcd_Init();			//��ʼ��OLED  
	printf("���ñ��� \r\n");
	LCD_Clear(BLACK);
	printf("������ɫ \r\n");
	BACK_COLOR=BLACK;

	printf("��ʼ��led \r\n");
	Lcd_Init();			//��ʼ��OLED  
	printf("���ñ��� \r\n");
	LCD_Clear(BLACK);
	printf("������ɫ \r\n");
	BACK_COLOR=BLACK;
	
	/*���ݽ̳���������ʾ�������֣�*/
//	LCD_ShowChinese32x32(10, 0, 0, 32, YELLOW);   //��
//	LCD_ShowChinese32x32(45, 0, 1, 32, YELLOW);   //��
//	LCD_ShowChinese32x32(80, 0, 2, 32, YELLOW);   //԰
//	LCD_ShowChinese32x32(115, 0, 3, 32, YELLOW);  //��
//	LCD_ShowChinese32x32(150, 0, 4, 32, YELLOW);  //��
//	printf("д��ͼƬ \r\n");
//	LCD_ShowPicture(0,50,100,100);
//	HAL_Delay(200);
//	LCD_ShowPicture(80,50,100,100);
//	HAL_Delay(200);
//	LCD_ShowPicture(80,100,100,100);
//	HAL_Delay(200);
//	LCD_ShowPicture(0,100,100,100);
//	HAL_Delay(200);
//	LCD_ShowPicture(0,50,100,100);
	LCD_ShowString(0, 0, "Welcome!", YELLOW); //��������ʾ����Ļ�ϣ���ʾ����Զ�����
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
			/*
				�������ã�
				�������ݣ�
				��������
			*/
		
			/*����ADC��������������ص�ѹ*/
			HAL_ADC_Start(&hadc1);
			HAL_ADC_PollForConversion(&hadc1, 10);
			if(HAL_IS_BIT_SET(HAL_ADC_GetState(&hadc1), HAL_ADC_STATE_REG_EOC))
			{
				ADC1_Value = HAL_ADC_GetValue(&hadc1);
			}
			/*��ʼ���봮�ڿ��Ʋ���
				���ڿ��Կ��������������ģ�飬���ڷ��͹��������ݷ�Ϊ�����֣���һ���ֽ��������һ���ֽ��Ժ��ǿ�������
				��һ���ֽ������ж���Ҫ����һ��ģ����в�����
				������ֽڸ��ݲ�ͬ��ģ����䲻ͬ�Ĳ���
				 0 �� ����LED������
				 1 �� ������ʾ��Ļ��ʾ�ض����ݣ�
				 2 �� ����PCA9685ģ�飬������16·PWM�������
				 3 �� ���Ƶ��̵�����˶������Կ����������������ת��ת�٣�
				
				ע�⣺ 
					Reve_Order ��ֱֵ�Ӵ����˴��ڽ��յ��ĵ�һ���ֽڵ�����
					Reve_Data  �ǽ��յ��Ĵӵڶ����ֽڿ�ʼ�����ݣ�һ����255���ֽ�
			*/
			if(Reve_Order =='0') //�������ڿ���LED����
				{
					/*���ݲ���һ��һ���ֽڣ�
						0�����ر�LED�ƣ�
						1��������LED�ƣ�
					*/
					if(Reve_Data[1] == '0'){ HAL_GPIO_WritePin(LED_0_GPIO_Port, LED_0_Pin, GPIO_PIN_SET);}
					else if(Reve_Data[1] =='1') {	HAL_GPIO_WritePin(LED_0_GPIO_Port, LED_0_Pin, GPIO_PIN_RESET);}
					Reve_Order = ' ';
					printf("set led ok  \r\n");
				}
				
			else if(Reve_Order =='1')  //�������ڿ�����ʾ������
				{			
					/*���ݲ�����0��255���ֽڷ�Χ�ڣ�
						�����͹���������ֱ����ʾ����Ļ�ϡ�
					*/
					for(i=0;i<sizeof(Reve_Data);i++) //����Ҫ��ʾ������
						{
							if(Reve_Data[i]== '\0') //�����յ������ݵĽ�β
								{
									Reve_Data[i-2] = Reve_Data[i]; //�ѽ�β��'\0'��ǰ��'\r'ǰ�棬��ֹ��ʾ�����ݳ�������
								}
						}
					Reve_Data[0] = '>'; //����ʾ��������ǰ�����'>'�ַ�
					a_value = LCD_ShowString(0, a_value, Reve_Data, YELLOW); //��������ʾ����Ļ�ϣ���ʾ����Զ�����
					Reve_Order = ' '; //����������������
					printf("set OLED ok   \r\n");
				}
				
			else if(Reve_Order =='2')  //�������ڿ��ƶ������
				{		
					/*���յ�����Ϊa��p�Ŷ������Ƕȣ�ÿ�ο��Կ��ƶ�����
						�ѽ��յ�������ȫ��ѭ��������⵽��ĸ��ʱ���Զ�������ĸ�����ŵ���λ���ݣ�
						����λ���ݾ��Ƕ���ĽǶȣ����ƶ���ĽǶȵ�������ҪΪ001��090��180���ָ�ʽ�����ȱ�������λ�����Ȳ�����Ҫ��0���棻
					*/
					for(i=1;i<sizeof(Reve_Data);i++)
						{
							a_value = (int)Reve_Data[i]; //�ѽ��յ����ַ�������ת��Ϊ��������
							if( 96<a_value && a_value<113 ) //�ж���������ǲ�����a��p��Χ�ڵ���ĸ������ǣ���ʼ��ת������ĽǶ�����
								{
									value_PWM = ((int)Reve_Data[i+1]-48)*100+ ((int)Reve_Data[i+2]-48)*10+ (int)Reve_Data[i+3]-48;//���ַ��ͽǶ�����ת��������
									if(value_PWM>= 180)value_PWM = 179; //�ԽǶȴ�С���п��ƣ����ó�����Χ
									else if (value_PWM <= 1) value_PWM = 1; 
									printf("set PWM ok %c  value_PWM: %d  \r\n",a_value ,value_PWM);
									switch (a_value) //������ĸ�����Ĳ�ͬ�Ķ�������ò�ͬ�ĽǶ�
										{
											case 'a':PCA_MG9XX(0, value_PWM); break;//ͨ��0 ת��value_PWM��С�ĽǶ�
											case 'b':PCA_MG9XX(1, value_PWM); break;//ͨ��1 ת��value_PWM��С�ĽǶ�
											case 'c':PCA_MG9XX(2, value_PWM); break;//ͨ��2 ת��value_PWM��С�ĽǶ�
											case 'd':PCA_MG9XX(3, value_PWM); break;//ͨ��3 ת��value_PWM��С�ĽǶ�
											case 'e':PCA_MG9XX(4, value_PWM); break;//ͨ��4 ת��value_PWM��С�ĽǶ�
											case 'f':PCA_MG9XX(5, value_PWM); break;//ͨ��5 ת��value_PWM��С�ĽǶ�
											case 'g':PCA_MG9XX(6, value_PWM); break;//ͨ��6 ת��value_PWM��С�ĽǶ�
											case 'h':PCA_MG9XX(7, value_PWM); break;//ͨ��7 ת��value_PWM��С�ĽǶ�
											case 'i':PCA_MG9XX(8, value_PWM); break;//ͨ��8 ת��value_PWM��С�ĽǶ�
											case 'j':PCA_MG9XX(9, value_PWM); break;//ͨ��9 ת��value_PWM��С�ĽǶ�
											case 'k':PCA_MG9XX(10,value_PWM); break;//ͨ��10 ת��value_PWM��С�ĽǶ�
											case 'l':PCA_MG9XX(11,value_PWM); break;//ͨ��11 ת��value_PWM��С�ĽǶ�
											case 'm':PCA_MG9XX(12,value_PWM); break;//ͨ��12 ת��value_PWM��С�ĽǶ�
											case 'n':PCA_MG9XX(13,value_PWM); break;//ͨ��13 ת��value_PWM��С�ĽǶ�
											case 'o':PCA_MG9XX(14,value_PWM); break;//ͨ��14 ת��value_PWM��С�ĽǶ�
											case 'p':PCA_MG9XX(15,value_PWM); break;//ͨ��15 ת��value_PWM��С�ĽǶ�
										}
									a_value = 0; //�ѵõ�����ĸ���
								}
							if(Reve_Data[i] == '\n')break; //������ݽ������Զ�����forѭ��
						}
					Reve_Order = ' ';
				}
			else if(Reve_Order =='3')  //�������ڿ��Ƶ��̵��������
				{
					/*���ݲ������4���ֽ�
						���յ������ݷ�Ϊ��������Ŀ��ƣ�ÿ������Ŀ��Ʒ�Ϊǰ��ͽǶ�
						��������Ϊg0a9 Ĭ��ǰ�����ֽڿ��Ƶ�һ��������������ֽڿ�����һ��
						g��goҲ����ǰ�����ٶ�Ϊ0����ֹͣת����a��after�Ǻ��ˣ��ٶ�Ϊ9���������ת��
						�����������ݲ����ϸ�ʽ�������������ת��*/
					if( Reve_Data[1] == 'g')
						{
							__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1,1000-((int)Reve_Data[2]-48)*100); //���ݽ��յ�����ת�������Ϳ���PWM
							HAL_GPIO_WritePin(set_motor1_B_GPIO_Port,set_motor1_B_Pin,GPIO_PIN_SET); //�������һ����������Ϊ��ͨio��ֱ�ӿ��Ƶ�ƽ����������ת
						}
					else if( Reve_Data[1] == 'a')
						{
							__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, ((int)Reve_Data[2]-48)*100); //���ݽ��յ�����ת�������Ϳ���PWM
							HAL_GPIO_WritePin(set_motor1_B_GPIO_Port,set_motor1_B_Pin,GPIO_PIN_RESET);//�������һ����������Ϊ��ͨio��ֱ�ӿ��Ƶ�ƽ����������ת
						}
					else 
						{ //���������ֹͣת��
							__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, 1000); 
							HAL_GPIO_WritePin(set_motor1_B_GPIO_Port,set_motor1_B_Pin,GPIO_PIN_SET);
							__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 1000);
							HAL_GPIO_WritePin(set_motor2_B_GPIO_Port,set_motor2_B_Pin,GPIO_PIN_SET);
						}
						
					if( Reve_Data[3] == 'g')
						{
							__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, 1000-((int)Reve_Data[4]-48)*100); //���ݽ��յ�����ת�������Ϳ���PWM
							HAL_GPIO_WritePin(set_motor2_B_GPIO_Port,set_motor2_B_Pin,GPIO_PIN_SET); //�������һ����������Ϊ��ͨio��ֱ�ӿ��Ƶ�ƽ����������ת
						}
					else if( Reve_Data[3] == 'a')
						{
							__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, ((int)Reve_Data[4]-48)*100); //���ݽ��յ�����ת�������Ϳ���PWM
							HAL_GPIO_WritePin(set_motor2_B_GPIO_Port,set_motor2_B_Pin,GPIO_PIN_RESET); //�������һ����������Ϊ��ͨio��ֱ�ӿ��Ƶ�ƽ����������ת
						}
					else 
						{ //���������ֹͣת��
							__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, 1000); 
							HAL_GPIO_WritePin(set_motor1_B_GPIO_Port,set_motor1_B_Pin,GPIO_PIN_SET);
							__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 1000);
							HAL_GPIO_WritePin(set_motor2_B_GPIO_Port,set_motor1_B_Pin,GPIO_PIN_SET);
						}
					Reve_Order = ' ';
					printf("set MOTOR ok  \r\n");
				}
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
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the CPU, AHB and APB busses clocks 
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
  /** Initializes the CPU, AHB and APB busses clocks 
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
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV2;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
/*����ͨѶ����*/
/*���ڽ��յ��ĳ����Ϊ�����֣���һ���ֽ�����������������
*/
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
	{
		uint8_t i=0;
		UNUSED(huart);
		
		Uart1_RxBuff[Uart1_Rx_Cnt++] = aRxBuffer;   //��������ת��
	 
		if((Uart1_RxBuff[Uart1_Rx_Cnt-1] == 0x0A)&&(Uart1_RxBuff[Uart1_Rx_Cnt-2] == 0x0D)) //�жϽ���λ���ж����ݵ�\r\n
			{
				/*���յĵ�һ���ֽ�������������Ʋ�ͬ��ģ��*/
				Reve_Order = Uart1_RxBuff[0];
				/*���ʹ��value�޷����⴫�ݽ��յ��ַ���*/
				/*��Uart1_RxBuff��ǰ�����ֽڴ�����������ֽڴ������
					���ݰ����������ݺ�ͼƬ����һ��������λ�����͸���λ���Ŀ������ݣ��������ݲ�һ����
					*/
				for (i=0; i<sizeof(Uart1_RxBuff)-1; i++)
					{	
						Reve_Data[i] = Uart1_RxBuff[i]; 
					}
				
				printf("PA6 Voltage : %.4f \r\n",2*ADC1_Value*3.3f/4096);
				
				Uart1_Rx_Cnt = 0;
				
				memset(Uart1_RxBuff,0x00,sizeof(Uart1_RxBuff)); //�������
			}
	 HAL_UART_Receive_IT(&huart1, (uint8_t *)&aRxBuffer, 1);   //�ٿ��������ж�
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