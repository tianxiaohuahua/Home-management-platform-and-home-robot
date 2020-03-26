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
uint8_t aRxBuffer;   //接收中断缓冲
uint8_t Uart1_RxBuff[256];  //接收缓冲
uint8_t Uart1_Rx_Cnt = 0;  //接收缓冲计数

uint8_t Reve_Data[256];
uint8_t Reve_Order;

uint16_t ADC1_Value;

//uint8_t cAlmStr[] = "数据溢出(大于256)\r\n";
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
	PCA_MG9XX_Init(60,90);//初始化舵机驱动
  /* USER CODE BEGIN 2 */
	HAL_UART_Receive_IT(&huart1, (uint8_t *)&aRxBuffer, 1); //开启接收中断，用来接收串口发送过来的信息
	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1); //开启定时器PWM通道 定时器2 通道1
	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3); //开启定时器PWM通道 定时器2 通道3
	__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 0); //设置PWM的占空比，这里初始化占空比为0
	__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, 0); //设置PWM的占空比，这里初始化占空比为0
	
  /* USER CODE END 2 */
	/*屏幕因为线路原因接触不良，初始化两遍屏幕*/
	HAL_Delay(500);
	printf("初始化led \r\n");
	Lcd_Init();			//初始化OLED  
	printf("设置背景 \r\n");
	LCD_Clear(BLACK);
	printf("设置颜色 \r\n");
	BACK_COLOR=BLACK;

	printf("初始化led \r\n");
	Lcd_Init();			//初始化OLED  
	printf("设置背景 \r\n");
	LCD_Clear(BLACK);
	printf("设置颜色 \r\n");
	BACK_COLOR=BLACK;
	
	/*根据教程在这里显示几个大字！*/
//	LCD_ShowChinese32x32(10, 0, 0, 32, YELLOW);   //中
//	LCD_ShowChinese32x32(45, 0, 1, 32, YELLOW);   //景
//	LCD_ShowChinese32x32(80, 0, 2, 32, YELLOW);   //园
//	LCD_ShowChinese32x32(115, 0, 3, 32, YELLOW);  //电
//	LCD_ShowChinese32x32(150, 0, 4, 32, YELLOW);  //子
//	printf("写入图片 \r\n");
//	LCD_ShowPicture(0,50,100,100);
//	HAL_Delay(200);
//	LCD_ShowPicture(80,50,100,100);
//	HAL_Delay(200);
//	LCD_ShowPicture(80,100,100,100);
//	HAL_Delay(200);
//	LCD_ShowPicture(0,100,100,100);
//	HAL_Delay(200);
//	LCD_ShowPicture(0,50,100,100);
	LCD_ShowString(0, 0, "Welcome!", YELLOW); //把数据显示到屏幕上，显示完成自动换行
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
			/*
				函数作用：
				输入数据：
				返回数据
			*/
		
			/*设置ADC的引脚用来检测电池电压*/
			HAL_ADC_Start(&hadc1);
			HAL_ADC_PollForConversion(&hadc1, 10);
			if(HAL_IS_BIT_SET(HAL_ADC_GetState(&hadc1), HAL_ADC_STATE_REG_EOC))
			{
				ADC1_Value = HAL_ADC_GetValue(&hadc1);
			}
			/*开始进入串口控制部分
				串口可以控制驱动板的所有模块，串口发送过来的数据分为两部分，第一个字节是命令，第一个字节以后是控制数据
				第一个字节用来判断需要对哪一个模块进行操作；
				后面的字节根据不同的模块分配不同的参数
				 0 ： 控制LED的亮灭；
				 1 ： 控制显示屏幕显示特定数据；
				 2 ： 控制PCA9685模块，来控制16路PWM的输出；
				 3 ： 控制底盘电机的运动，可以控制两个电机的正反转和转速；
				
				注意： 
					Reve_Order 的值直接代表了串口接收到的第一个字节的数据
					Reve_Data  是接收到的从第二个字节开始的数据，一共有255个字节
			*/
			if(Reve_Order =='0') //处理串口控制LED部分
				{
					/*数据部分一共一个字节，
						0代表关闭LED灯；
						1代表开启LED灯；
					*/
					if(Reve_Data[1] == '0'){ HAL_GPIO_WritePin(LED_0_GPIO_Port, LED_0_Pin, GPIO_PIN_SET);}
					else if(Reve_Data[1] =='1') {	HAL_GPIO_WritePin(LED_0_GPIO_Port, LED_0_Pin, GPIO_PIN_RESET);}
					Reve_Order = ' ';
					printf("set led ok  \r\n");
				}
				
			else if(Reve_Order =='1')  //处理串口控制显示屏部分
				{			
					/*数据部分在0至255个字节范围内；
						将发送过来的数据直接显示在屏幕上。
					*/
					for(i=0;i<sizeof(Reve_Data);i++) //处理要显示的数据
						{
							if(Reve_Data[i]== '\0') //检测接收到的数据的结尾
								{
									Reve_Data[i-2] = Reve_Data[i]; //把结尾的'\0'提前到'\r'前面，防止显示的数据出现乱码
								}
						}
					Reve_Data[0] = '>'; //在显示的数据最前面加上'>'字符
					a_value = LCD_ShowString(0, a_value, Reve_Data, YELLOW); //把数据显示到屏幕上，显示完成自动换行
					Reve_Order = ' '; //把命令控制数据清除
					printf("set OLED ok   \r\n");
				}
				
			else if(Reve_Order =='2')  //处理串口控制舵机部分
				{		
					/*接收的数据为a至p号舵机及其角度，每次可以控制多个舵机
						把接收到的数据全部循环，当检测到字母的时候自动整理字母紧跟着的三位数据；
						这三位数据就是舵机的角度，控制舵机的角度的数据需要为001、090、180这种格式，长度必须是三位，长度不够需要用0代替；
					*/
					for(i=1;i<sizeof(Reve_Data);i++)
						{
							a_value = (int)Reve_Data[i]; //把接收到的字符型数据转换为整型数据
							if( 96<a_value && a_value<113 ) //判断这个数据是不是在a至p范围内的字母，如果是，开始则转换后面的角度数据
								{
									value_PWM = ((int)Reve_Data[i+1]-48)*100+ ((int)Reve_Data[i+2]-48)*10+ (int)Reve_Data[i+3]-48;//把字符型角度数据转换成整型
									if(value_PWM>= 180)value_PWM = 179; //对角度大小进行控制，放置超出范围
									else if (value_PWM <= 1) value_PWM = 1; 
									printf("set PWM ok %c  value_PWM: %d  \r\n",a_value ,value_PWM);
									switch (a_value) //根据字母代表的不同的舵机来设置不同的角度
										{
											case 'a':PCA_MG9XX(0, value_PWM); break;//通道0 转动value_PWM大小的角度
											case 'b':PCA_MG9XX(1, value_PWM); break;//通道1 转动value_PWM大小的角度
											case 'c':PCA_MG9XX(2, value_PWM); break;//通道2 转动value_PWM大小的角度
											case 'd':PCA_MG9XX(3, value_PWM); break;//通道3 转动value_PWM大小的角度
											case 'e':PCA_MG9XX(4, value_PWM); break;//通道4 转动value_PWM大小的角度
											case 'f':PCA_MG9XX(5, value_PWM); break;//通道5 转动value_PWM大小的角度
											case 'g':PCA_MG9XX(6, value_PWM); break;//通道6 转动value_PWM大小的角度
											case 'h':PCA_MG9XX(7, value_PWM); break;//通道7 转动value_PWM大小的角度
											case 'i':PCA_MG9XX(8, value_PWM); break;//通道8 转动value_PWM大小的角度
											case 'j':PCA_MG9XX(9, value_PWM); break;//通道9 转动value_PWM大小的角度
											case 'k':PCA_MG9XX(10,value_PWM); break;//通道10 转动value_PWM大小的角度
											case 'l':PCA_MG9XX(11,value_PWM); break;//通道11 转动value_PWM大小的角度
											case 'm':PCA_MG9XX(12,value_PWM); break;//通道12 转动value_PWM大小的角度
											case 'n':PCA_MG9XX(13,value_PWM); break;//通道13 转动value_PWM大小的角度
											case 'o':PCA_MG9XX(14,value_PWM); break;//通道14 转动value_PWM大小的角度
											case 'p':PCA_MG9XX(15,value_PWM); break;//通道15 转动value_PWM大小的角度
										}
									a_value = 0; //把得到的字母清除
								}
							if(Reve_Data[i] == '\n')break; //如果数据结束，自动跳出for循环
						}
					Reve_Order = ' ';
				}
			else if(Reve_Order =='3')  //处理串口控制底盘的两个电机
				{
					/*数据部分最多4个字节
						接收到的数据分为两个电机的控制，每个电机的控制分为前后和角度
						比如数据为g0a9 默认前两个字节控制第一个电机，后两个字节控制另一个
						g是go也就是前进，速度为0，则停止转动，a是after是后退，速度为9，以最高速转动
						如果输入的数据不符合格式，两个电机都不转动*/
					if( Reve_Data[1] == 'g')
						{
							__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1,1000-((int)Reve_Data[2]-48)*100); //根据接收的数据转换成整型控制PWM
							HAL_GPIO_WritePin(set_motor1_B_GPIO_Port,set_motor1_B_Pin,GPIO_PIN_SET); //电机的另一个引脚设置为普通io，直接控制电平来控制正反转
						}
					else if( Reve_Data[1] == 'a')
						{
							__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, ((int)Reve_Data[2]-48)*100); //根据接收的数据转换成整型控制PWM
							HAL_GPIO_WritePin(set_motor1_B_GPIO_Port,set_motor1_B_Pin,GPIO_PIN_RESET);//电机的另一个引脚设置为普通io，直接控制电平来控制正反转
						}
					else 
						{ //两个电机都停止转动
							__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, 1000); 
							HAL_GPIO_WritePin(set_motor1_B_GPIO_Port,set_motor1_B_Pin,GPIO_PIN_SET);
							__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 1000);
							HAL_GPIO_WritePin(set_motor2_B_GPIO_Port,set_motor2_B_Pin,GPIO_PIN_SET);
						}
						
					if( Reve_Data[3] == 'g')
						{
							__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, 1000-((int)Reve_Data[4]-48)*100); //根据接收的数据转换成整型控制PWM
							HAL_GPIO_WritePin(set_motor2_B_GPIO_Port,set_motor2_B_Pin,GPIO_PIN_SET); //电机的另一个引脚设置为普通io，直接控制电平来控制正反转
						}
					else if( Reve_Data[3] == 'a')
						{
							__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, ((int)Reve_Data[4]-48)*100); //根据接收的数据转换成整型控制PWM
							HAL_GPIO_WritePin(set_motor2_B_GPIO_Port,set_motor2_B_Pin,GPIO_PIN_RESET); //电机的另一个引脚设置为普通io，直接控制电平来控制正反转
						}
					else 
						{ //两个电机都停止转动
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
/*串口通讯部分*/
/*串口接收到的程序分为两部分，第一个字节是命令，后面的是数据
*/
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
	{
		uint8_t i=0;
		UNUSED(huart);
		
		Uart1_RxBuff[Uart1_Rx_Cnt++] = aRxBuffer;   //接收数据转存
	 
		if((Uart1_RxBuff[Uart1_Rx_Cnt-1] == 0x0A)&&(Uart1_RxBuff[Uart1_Rx_Cnt-2] == 0x0D)) //判断结束位，判断数据的\r\n
			{
				/*接收的第一个字节是命令，用来控制不同的模块*/
				Reve_Order = Uart1_RxBuff[0];
				/*如果使用value无法向外传递接收到字符串*/
				/*用Uart1_RxBuff的前两个字节存放命令，后面的字节存放数据
					数据包括文字数据和图片数据一起其他上位机发送给下位机的控制数据，数据内容不一定，
					*/
				for (i=0; i<sizeof(Uart1_RxBuff)-1; i++)
					{	
						Reve_Data[i] = Uart1_RxBuff[i]; 
					}
				
				printf("PA6 Voltage : %.4f \r\n",2*ADC1_Value*3.3f/4096);
				
				Uart1_Rx_Cnt = 0;
				
				memset(Uart1_RxBuff,0x00,sizeof(Uart1_RxBuff)); //清空数组
			}
	 HAL_UART_Receive_IT(&huart1, (uint8_t *)&aRxBuffer, 1);   //再开启接收中断
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
