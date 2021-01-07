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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "bno055_stm32.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define IDLE 0
#define INIT 1
#define MESSAGE  2
#define SEND_RESULT 3

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint8_t state=0;
uint8_t noOfBytes;
uint8_t nextByteIndex=0;
uint8_t RxBuffer[25];
uint8_t Result[25];
uint8_t noOfResultBytes;
uint8_t resultIndex=0;
/* USER CODE END PV */

volatile uint16_t tempData[500]; 

volatile uint16_t index =0;
volatile uint16_t oldCount =0;
volatile uint16_t newCount =0;
volatile uint16_t odd =0;
bno055_vector_t gyroV;
/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM1_Init(void);
static void MX_TIM3_Init(void);
static void MX_SPI1_Init(void);
static void MX_SPI2_Init(void);
//For BNO055
static void MX_I2C1_Init(void);
static void MX_TIM2_Init(void);
uint16_t ReadSerialNo(uint8_t);
/* USER CODE BEGIN PFP */
static void SetMotorPWMValue(uint8_t motorNo,uint8_t dir, uint16_t value );
static void WriteFPGA(uint8_t , uint16_t);
static void SetServoPWMValue(uint8_t, uint16_t);
uint16_t ReadQuadratureValue(uint8_t);
static void Process(uint8_t);
uint8_t RunCommand(uint8_t [],uint8_t []);
static void WriteQuadratureValue(uint8_t,uint16_t);
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

  LL_APB1_GRP2_EnableClock(LL_APB1_GRP2_PERIPH_SYSCFG);
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);

  /* System interrupt init*/

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();

  MX_TIM1_Init(); //PWM for Motors 4-7
  MX_TIM3_Init(); //PWM for Motors 0-3
  MX_SPI1_Init(); //For FPGA communication   (as Master)   
  MX_SPI2_Init();  //For FPGA communication  (as Slave)
  //BNO055
  MX_TIM2_Init();
  MX_I2C1_Init();
  /* USER CODE BEGIN 2 */
  state=IDLE;
  GPIOB -> BRR= LL_GPIO_PIN_11;//Not Busy
  GPIOD -> BRR= LL_GPIO_PIN_11;//Not Ready
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  
    
   /*
    uint16_t serialNo = ReadSerialNo(0x0F);
    SetServoPWMValue(0,6000);
    SetServoPWMValue(1,20000);
    SetServoPWMValue(2,15000);
    SetServoPWMValue(3,10000);
    SetServoPWMValue(4,15000);
    SetServoPWMValue(5,35000);
    SetServoPWMValue(6,15000);
    SetServoPWMValue(7,20000);
    
    SetMotorPWMValue(0,1,400);
    SetMotorPWMValue(1,0,300);
    SetMotorPWMValue(2,1,500);
    SetMotorPWMValue(3,0,400);
    SetMotorPWMValue(4,1,300);
    SetMotorPWMValue(5,0,500);
    SetMotorPWMValue(6,1,600);
    SetMotorPWMValue(7,0,600);
   uint16_t ReadQuadValue0;
   uint16_t ReadQuadValue1;
   uint16_t ReadQuadValue2;
   uint16_t ReadQuadValue3;
   uint16_t ReadQuadValue4;
   uint16_t ReadQuadValue5;
   uint16_t ReadQuadValue6;
   uint16_t ReadQuadValue7;*/

    
  while (1)
  {
    

  /*ReadQuadValue0=ReadQuadratureValue(0);
  ReadQuadValue1=ReadQuadratureValue(1);
  ReadQuadValue2=ReadQuadratureValue(2);
  ReadQuadValue3=ReadQuadratureValue(3);
  ReadQuadValue4=ReadQuadratureValue(4);
  ReadQuadValue5=ReadQuadratureValue(5);
  ReadQuadValue6=ReadQuadratureValue(6);
  ReadQuadValue7=ReadQuadratureValue(7);*/
  
    /* USER CODE END WHILE */
    uint8_t receivedByte;
    // gyroV =bno055_getVectorEuler();
    while(LL_SPI_IsActiveFlag_RXNE(SPI2)==RESET);
    receivedByte=LL_SPI_ReceiveData8(SPI2);
    Process(receivedByte); 
   
    
    
    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

static void Process(uint8_t receivedByte){

    uint8_t commandSuccess;
    
    switch (state){  
      case IDLE:
        if (receivedByte==0xBC){
          state=INIT;//Init
          GPIOB -> BSRR= LL_GPIO_PIN_11;//set Busy
        }
        break;
      case INIT:
        state=MESSAGE;//NoofByteState
        noOfBytes=receivedByte;
        RxBuffer[nextByteIndex] = receivedByte;
        nextByteIndex++;
        break;
      case MESSAGE:
        //status=3 ; //MessageState
        RxBuffer[nextByteIndex] = receivedByte;
        noOfBytes--;
        nextByteIndex++;
        if(noOfBytes ==0)
        {
          commandSuccess=RunCommand(RxBuffer,Result);
          noOfResultBytes=Result[0]+1;
          resultIndex=0;
          Result[0]=commandSuccess;
          LL_SPI_TransmitData8(SPI2,noOfResultBytes); //sending this many bytes
          //SPI_SendData8(SPI2,noOfResultBytes);
          GPIOD -> BSRR = LL_GPIO_PIN_11; //set Ready
          state= SEND_RESULT;
          nextByteIndex=0;
        }
        break;
      case SEND_RESULT:
        if(noOfResultBytes > 0){
         LL_SPI_TransmitData8(SPI2, Result[resultIndex]);
         //SPI_SendData8(SPI2, Result[resultIndex]);
         resultIndex++;
         noOfResultBytes--;
        } else {
         //while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
          state=IDLE;
          GPIOB -> BRR= LL_GPIO_PIN_11;//Not Busy
          GPIOD -> BRR= LL_GPIO_PIN_11;// Not Ready
        }
    }
   
 }

uint8_t RunCommand(uint8_t RxBytes[], uint8_t Result[] ){
    
    if (RxBytes[1]== 0x01){
      uint16_t motorValue =(RxBytes[4]<<8) | (RxBytes[5]);
      SetMotorPWMValue(RxBytes[2],RxBytes[3],motorValue);// motor no,dir, value
      Result[0]= 0x00;
      return 1;
    }else if(RxBytes[1]== 0x02){
      uint16_t servoValue =(RxBytes[3]<<8) | (RxBytes[4]);//servo no, value
      SetServoPWMValue(RxBytes[2],servoValue);
      Result[0]= 0x00;
      return 1;
    }else if(RxBytes[1]== 0x03){
      uint16_t ReadQuadValue=ReadQuadratureValue(RxBytes[2]);
      Result[0]= 0x02;
      Result[1]=(ReadQuadValue >> 8) & 0xff;
      Result[2]= ReadQuadValue & 0xff; 
      return 1;
    }else if(RxBytes[1]== 0x04){
      uint16_t QuadValue =(RxBytes[3]<<8) | (RxBytes[4]);
      WriteQuadratureValue(RxBytes[2],QuadValue);
      Result[0]= 0x00;
      return 1;
    }else if(RxBytes[1]== 0x05){
      uint16_t serialNo= ReadSerialNo(RxBytes[2]);
      Result[0]= 0x02;
      Result[1]=(serialNo >> 8) & 0xff;
      Result[2]= serialNo & 0xff; 
      return 1;
    }else{
      return 0;
      
    }
   
  }

 static void WriteQuadratureValue(uint8_t QuadNo, uint16_t value){
    
    WriteFPGA(QuadNo,value);
  } 

 static void WriteFPGA(uint8_t reg, uint16_t value){
   
    uint8_t regValue;
    uint8_t secondReg;
    uint8_t thirdReg;
    
    regValue = reg << 3;
    regValue = (regValue | 0x80);
    secondReg = (value >> 8) & 0xff;
    thirdReg = value & 0xff;
    
   GPIOB -> BRR=LL_GPIO_PIN_0;
    while (LL_SPI_IsActiveFlag_TXE(SPI1) == RESET); //wait until buffer is empty 
    LL_SPI_TransmitData8(SPI1, regValue);
    while(LL_SPI_IsActiveFlag_RXNE(SPI1)==RESET);// //wait until buffer is full 
    LL_SPI_ReceiveData8(SPI1);
    while (LL_SPI_IsActiveFlag_TXE(SPI1) == RESET); //wait until buffer is empty 
    LL_SPI_TransmitData8(SPI1, secondReg);
    while(LL_SPI_IsActiveFlag_RXNE(SPI1)==RESET);// //wait until buffer is full 
    LL_SPI_ReceiveData8(SPI1);
    while (LL_SPI_IsActiveFlag_TXE(SPI1) == RESET); //wait until buffer is empty 
    LL_SPI_TransmitData8(SPI1, thirdReg);
    while(LL_SPI_IsActiveFlag_RXNE(SPI1)==RESET);// //wait until buffer is full 
    LL_SPI_ReceiveData8(SPI1);

    while (LL_SPI_IsActiveFlag_BSY(SPI1)==SET);
    GPIOB -> BSRR=LL_GPIO_PIN_0;
   
  }

 uint16_t ReadFPGA(uint16_t reg){
    
    uint8_t readSecondBuffer;
    uint8_t readThirdBuffer;
    uint16_t secondThirdBuffer;
    uint8_t regValue;
    
    reg<<=3;
    regValue = reg & 0xff; 
    
    GPIOB -> BRR= LL_GPIO_PIN_0;
    
    while (LL_SPI_IsActiveFlag_TXE(SPI1) == RESET); //wait until buffer is empty 
    LL_SPI_TransmitData8(SPI1, regValue);
    while(LL_SPI_IsActiveFlag_RXNE(SPI1)==RESET);// //wait until buffer is full 
    LL_SPI_ReceiveData8(SPI1);
    while (LL_SPI_IsActiveFlag_TXE(SPI1) == RESET); //wait until buffer is empty 
    LL_SPI_TransmitData8(SPI1, 0x00);
    while(LL_SPI_IsActiveFlag_RXNE(SPI1)==RESET);// //wait until buffer is full 
    readSecondBuffer=LL_SPI_ReceiveData8(SPI1);
    while (LL_SPI_IsActiveFlag_TXE(SPI1) == RESET); //wait until buffer is empty 
    LL_SPI_TransmitData8(SPI1, 0x00);
    while(LL_SPI_IsActiveFlag_RXNE(SPI1)==RESET);// //wait until buffer is full 
    readThirdBuffer=LL_SPI_ReceiveData8(SPI1);
  
    while (LL_SPI_IsActiveFlag_BSY(SPI1)==SET);
    
    GPIOB -> BSRR=LL_GPIO_PIN_0;
   
    secondThirdBuffer =(readSecondBuffer<<8) | (readThirdBuffer);
   
    return secondThirdBuffer;
    
  }

 uint16_t ReadQuadratureValue(uint8_t QuadNo){
    uint16_t receivedBytes;
    receivedBytes= ReadFPGA(QuadNo);
    return receivedBytes;
  }


uint16_t ReadSerialNo(uint8_t serialNoReg){
   uint16_t receivedBytes;
    receivedBytes= ReadFPGA(serialNoReg);
   return receivedBytes;
 }

static void SetServoPWMValue(uint8_t servoNo, uint16_t value){
    
    int i;
   
    if (servoNo > 8){
      servoNo=0;
     }
    WriteFPGA(servoNo,value);//writing to pwm 0 for servo
    for (i=0;i<1000000;i++){
    } 
    
    
  }



static void SetMotorPWMValue(uint8_t motorNo,uint8_t dir, uint16_t value ){
   if (value > 799){
     value=799;
   }
   if (motorNo==0){
       
      LL_TIM_OC_SetCompareCH1(TIM3,value);
      
      if (dir==0){
        //Motor 0=> IN1- PD14, IN2- PD15, PWM1 pin PC6
        //Motor backward
        GPIOD->BSRR = LL_GPIO_PIN_14;
        GPIOD->BRR = LL_GPIO_PIN_15;
      }else {
         //Motor Forward
        GPIOD->BRR = LL_GPIO_PIN_15;
        GPIOD->BSRR = LL_GPIO_PIN_14;
      }
   }
   if (motorNo==1){
      LL_TIM_OC_SetCompareCH2(TIM3,value);
      if (dir==0){
        //Motor 1=> IN1- PD12, IN2- PD13, PWM1 pin PC7
        //Motor backward
        GPIOD->BSRR = LL_GPIO_PIN_12;
        GPIOD->BRR = LL_GPIO_PIN_13;
      }else {
        //Motor Forward
        GPIOD->BRR = LL_GPIO_PIN_13;
        GPIOD->BSRR = LL_GPIO_PIN_12;
      }
   }
   if (motorNo==2){
         LL_TIM_OC_SetCompareCH3(TIM3,value);
      if (dir==0){
        //Motor 2=> IN1- PA8 , IN2- PA9, PWM1 pin PC8
        //Motor backward
        GPIOA->BSRR = LL_GPIO_PIN_8;
        GPIOA->BRR = LL_GPIO_PIN_9;
      }else {
        //Motor Forward
        GPIOA->BRR = LL_GPIO_PIN_9;
        GPIOA->BSRR = LL_GPIO_PIN_8;
      }      
   }
   if(motorNo==3){
        LL_TIM_OC_SetCompareCH4(TIM3,value);
      if (dir==0){
        //Motor 3=> IN1- PA10 , IN2- PA11, PWM1 pin PC8
        //Motor backward
        GPIOA->BSRR = LL_GPIO_PIN_11;
        GPIOA->BRR = LL_GPIO_PIN_10;
      }else {
        //Motor Forward
        GPIOA->BRR = LL_GPIO_PIN_10;
        GPIOA->BSRR = LL_GPIO_PIN_11;
      }      
   }
   if(motorNo==4){
       LL_TIM_OC_SetCompareCH1(TIM1,value);
      if (dir==0){
        //Motor 4=> IN1- PB2 , IN2- PB1, PWM1 pin PE9
        //Motor backward
        GPIOB->BSRR = LL_GPIO_PIN_2;
        GPIOB->BRR = LL_GPIO_PIN_1;
      }else {
        //Motor Forward
        GPIOB->BRR = LL_GPIO_PIN_1;
        GPIOB->BSRR = LL_GPIO_PIN_2;
      }         
   }
   if(motorNo==5){
     
       LL_TIM_OC_SetCompareCH2(TIM1,value);
       
      if (dir==0){
        //Motor 5=> IN1- PE8 , IN2- PE7, PWM1 pin PE11
        //Motor backward
        GPIOE->BSRR = LL_GPIO_PIN_8;
        GPIOE->BRR = LL_GPIO_PIN_7;
      }else {
        //Motor Forward
        GPIOE->BRR = LL_GPIO_PIN_7;
        GPIOE->BSRR = LL_GPIO_PIN_8;
      }         
   }
   
   if(motorNo==6){
       LL_TIM_OC_SetCompareCH3(TIM1,value);
      if (dir==0){
        //Motor 4=> IN1- PE12 , IN2- PE10, PWM1 pin PE13
        //Motor backward
        GPIOE->BSRR = LL_GPIO_PIN_12;
        GPIOE->BRR = LL_GPIO_PIN_10;
      }else {
        //Motor Forward
        GPIOE->BRR = LL_GPIO_PIN_10;
        GPIOE->BSRR = LL_GPIO_PIN_12;
      }         
   }
   
   if(motorNo==7){
       LL_TIM_OC_SetCompareCH4(TIM1,value);
      if (dir==0){
        //Motor 4=> IN1- PB10 , IN2- PE15, PWM1 pin PE14
        //Motor backward
        GPIOB->BSRR = LL_GPIO_PIN_10;
        GPIOE->BRR = LL_GPIO_PIN_15;
      }else {
        //Motor Forward
        GPIOE->BRR = LL_GPIO_PIN_15;
        GPIOB->BSRR = LL_GPIO_PIN_10;
      }         
   }
   
}



/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  LL_FLASH_SetLatency(LL_FLASH_LATENCY_1);
  while(LL_FLASH_GetLatency() != LL_FLASH_LATENCY_1)
  {
  }
  LL_RCC_HSE_Enable();

   /* Wait till HSE is ready */
  while(LL_RCC_HSE_IsReady() != 1)
  {

  }
  LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSE, LL_RCC_PLL_MUL_3, LL_RCC_PREDIV_DIV_1);
  LL_RCC_PLL_Enable();

   /* Wait till PLL is ready */
  while(LL_RCC_PLL_IsReady() != 1)
  {

  }
  LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
  LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);
  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);

   /* Wait till System clock is ready */
  while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL)
  {

  }
  LL_Init1msTick(48000000);
  LL_SetSystemCoreClock(48000000);
  LL_RCC_SetI2CClockSource(LL_RCC_I2C1_CLKSOURCE_SYSCLK);
}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  LL_SPI_InitTypeDef SPI_InitStruct = {0};

  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* Peripheral clock enable */
  LL_APB1_GRP2_EnableClock(LL_APB1_GRP2_PERIPH_SPI1);

  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);
  /**SPI1 GPIO Configuration
  PA5   ------> SPI1_SCK
  PA6   ------> SPI1_MISO
  PA7   ------> SPI1_MOSI
  PB0 -----> SPI1_CS
  */
  GPIO_InitStruct.Pin = LL_GPIO_PIN_5;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_DOWN;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_0;
  LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = LL_GPIO_PIN_6;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_0;
  LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = LL_GPIO_PIN_7;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_0;
  LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* USER CODE BEGIN SPI1_Init 1 */
  //FPGA_CS 
  
  GPIO_InitStruct.Pin = LL_GPIO_PIN_0;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(GPIOB, &GPIO_InitStruct);
  
  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  SPI_InitStruct.TransferDirection = LL_SPI_FULL_DUPLEX;
  SPI_InitStruct.Mode = LL_SPI_MODE_MASTER;
  SPI_InitStruct.DataWidth = LL_SPI_DATAWIDTH_8BIT;
  SPI_InitStruct.ClockPolarity = LL_SPI_POLARITY_HIGH;
  SPI_InitStruct.ClockPhase = LL_SPI_PHASE_2EDGE;
  SPI_InitStruct.NSS = LL_SPI_NSS_SOFT;
  SPI_InitStruct.BaudRate = LL_SPI_BAUDRATEPRESCALER_DIV32;
  SPI_InitStruct.BitOrder = LL_SPI_MSB_FIRST;
  SPI_InitStruct.CRCCalculation = LL_SPI_CRCCALCULATION_DISABLE;
  SPI_InitStruct.CRCPoly = 7;
  LL_SPI_Init(SPI1, &SPI_InitStruct);
  LL_SPI_SetStandard(SPI1, LL_SPI_PROTOCOL_MOTOROLA);
  LL_SPI_DisableNSSPulseMgt(SPI1);
  /* USER CODE BEGIN SPI1_Init 2 */
  LL_SPI_SetRxFIFOThreshold(SPI1, LL_SPI_RX_FIFO_TH_QUARTER);
  LL_SPI_Enable(SPI1);
/* USER CODE END SPI1_Init 2 */

}

/**
  * @brief SPI2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI2_Init(void)
{

  /* USER CODE BEGIN SPI2_Init 0 */

  /* USER CODE END SPI2_Init 0 */

  LL_SPI_InitTypeDef SPI_InitStruct = {0};

  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* Peripheral clock enable */
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_SPI2);

  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB);
  /**SPI2 GPIO Configuration
  PB13   ------> SPI2_SCK
  PB14   ------> SPI2_MISO
  PB15   ------> SPI2_MOSI
  */
  GPIO_InitStruct.Pin = LL_GPIO_PIN_13;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_DOWN;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_0;
  LL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = LL_GPIO_PIN_14;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_0;
  LL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = LL_GPIO_PIN_15;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_0;
  LL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* USER CODE BEGIN SPI2_Init 1 */
  //Pin B12  CS
  GPIO_InitStruct.Pin = LL_GPIO_PIN_12;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_0;
  LL_GPIO_Init(GPIOB, &GPIO_InitStruct);
  
    
  //PD 8 for status
  GPIO_InitStruct.Pin = LL_GPIO_PIN_8;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_0;
  LL_GPIO_Init(GPIOD, &GPIO_InitStruct);
  
  //PB 11 Busy
  GPIO_InitStruct.Pin = LL_GPIO_PIN_11;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    
  //PD 11 Ready
  GPIO_InitStruct.Pin = LL_GPIO_PIN_11;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(GPIOD, &GPIO_InitStruct);
  
  
  /* USER CODE END SPI2_Init 1 */
  /* SPI2 parameter configuration*/
  SPI_InitStruct.TransferDirection = LL_SPI_FULL_DUPLEX;
  SPI_InitStruct.Mode = LL_SPI_MODE_SLAVE;
  SPI_InitStruct.DataWidth = LL_SPI_DATAWIDTH_8BIT;
  SPI_InitStruct.ClockPolarity = LL_SPI_POLARITY_LOW;
  SPI_InitStruct.ClockPhase = LL_SPI_PHASE_1EDGE;
  SPI_InitStruct.NSS = LL_SPI_NSS_SOFT;
  SPI_InitStruct.BitOrder = LL_SPI_MSB_FIRST;
  SPI_InitStruct.CRCCalculation = LL_SPI_CRCCALCULATION_DISABLE;
  SPI_InitStruct.CRCPoly = 7;
  LL_SPI_Init(SPI2, &SPI_InitStruct);
  LL_SPI_SetStandard(SPI2, LL_SPI_PROTOCOL_MOTOROLA);
  LL_SPI_DisableNSSPulseMgt(SPI2);
  /* USER CODE BEGIN SPI2_Init 2 */
  LL_SPI_SetRxFIFOThreshold(SPI2, LL_SPI_RX_FIFO_TH_QUARTER);
  LL_SPI_Enable(SPI2);
  /* USER CODE END SPI2_Init 2 */

}

static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  LL_I2C_InitTypeDef I2C_InitStruct = {0};

  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB);
  /**I2C1 GPIO Configuration
  PB8   ------> I2C1_SCL
  PB9   ------> I2C1_SDA
  */
  GPIO_InitStruct.Pin = LL_GPIO_PIN_8;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_OPENDRAIN;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_1;
  LL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = LL_GPIO_PIN_9;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_OPENDRAIN;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_1;
  LL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* Peripheral clock enable */
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_I2C1);

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  /** I2C Initialization
  */
  LL_I2C_DisableOwnAddress2(I2C1);
  LL_I2C_DisableGeneralCall(I2C1);
  LL_I2C_EnableClockStretching(I2C1);
  I2C_InitStruct.PeripheralMode = LL_I2C_MODE_I2C;
  I2C_InitStruct.Timing = 0x10900C20;
  I2C_InitStruct.AnalogFilter = LL_I2C_ANALOGFILTER_ENABLE;
  I2C_InitStruct.DigitalFilter = 0;
  I2C_InitStruct.OwnAddress1 = 0;
  I2C_InitStruct.TypeAcknowledge = LL_I2C_ACK;
  I2C_InitStruct.OwnAddrSize = LL_I2C_OWNADDRESS1_7BIT;
  LL_I2C_Init(I2C1, &I2C_InitStruct);
  LL_I2C_EnableAutoEndMode(I2C1);
  LL_I2C_SetOwnAddress2(I2C1, 0, LL_I2C_OWNADDRESS2_NOMASK);
  /* USER CODE BEGIN I2C1_Init 2 */
 
  /* USER CODE END I2C1_Init 2 */

}


/**
  * @brief TIM1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM1_Init(void)
{

  
  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  LL_TIM_InitTypeDef TIM_InitStruct = {0};
  LL_TIM_OC_InitTypeDef TIM_OC_InitStruct = {0};

  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* Peripheral clock enable */
  LL_APB1_GRP2_EnableClock(LL_APB1_GRP2_PERIPH_TIM1);

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  TIM_InitStruct.Prescaler = 5; //(48/6)=8mhz
  TIM_InitStruct.CounterMode = LL_TIM_COUNTERMODE_UP;
  TIM_InitStruct.Autoreload = 799;//(8mhz/800=10000=(10khz))//duty cycle 10khz
  TIM_InitStruct.ClockDivision = LL_TIM_CLOCKDIVISION_DIV1;
  LL_TIM_Init(TIM1, &TIM_InitStruct);
  LL_TIM_DisableARRPreload(TIM1);
  
  LL_TIM_SetClockSource(TIM1, LL_TIM_CLOCKSOURCE_INTERNAL);
  LL_TIM_SetTriggerOutput(TIM1, LL_TIM_TRGO_RESET);
  LL_TIM_DisableMasterSlaveMode(TIM1);
  
  //Motor 4
  LL_TIM_OC_EnablePreload(TIM1, LL_TIM_CHANNEL_CH1);
  TIM_OC_InitStruct.OCMode = LL_TIM_OCMODE_PWM2;
  TIM_OC_InitStruct.OCState = LL_TIM_OCSTATE_DISABLE;
  TIM_OC_InitStruct.OCNState = LL_TIM_OCSTATE_DISABLE;
  TIM_OC_InitStruct.CompareValue = 400;
  TIM_OC_InitStruct.OCPolarity = LL_TIM_OCPOLARITY_HIGH;
  LL_TIM_OC_Init(TIM1, LL_TIM_CHANNEL_CH1, &TIM_OC_InitStruct);
  LL_TIM_OC_DisableFast(TIM1, LL_TIM_CHANNEL_CH1);
  LL_TIM_CC_EnableChannel(TIM1,  LL_TIM_CHANNEL_CH1); 
  
   //Motor 5
  LL_TIM_OC_EnablePreload(TIM1, LL_TIM_CHANNEL_CH2);
  TIM_OC_InitStruct.OCState = LL_TIM_OCSTATE_DISABLE;
  TIM_OC_InitStruct.OCNState = LL_TIM_OCSTATE_DISABLE;
  TIM_OC_InitStruct.CompareValue = 350;
  LL_TIM_OC_Init(TIM1, LL_TIM_CHANNEL_CH2, &TIM_OC_InitStruct);
  LL_TIM_OC_DisableFast(TIM1, LL_TIM_CHANNEL_CH2);
  LL_TIM_CC_EnableChannel(TIM1,  LL_TIM_CHANNEL_CH2); 
  
   //Motor 6
  LL_TIM_OC_EnablePreload(TIM1, LL_TIM_CHANNEL_CH3);
  TIM_OC_InitStruct.OCState = LL_TIM_OCSTATE_DISABLE;
  TIM_OC_InitStruct.OCNState = LL_TIM_OCSTATE_DISABLE;
  TIM_OC_InitStruct.CompareValue = 600;
  LL_TIM_OC_Init(TIM1, LL_TIM_CHANNEL_CH3, &TIM_OC_InitStruct);
  LL_TIM_OC_DisableFast(TIM1, LL_TIM_CHANNEL_CH3);
  LL_TIM_CC_EnableChannel(TIM1,  LL_TIM_CHANNEL_CH3); 
  
   //Motor 7
  LL_TIM_OC_EnablePreload(TIM1, LL_TIM_CHANNEL_CH4);
  TIM_OC_InitStruct.OCState = LL_TIM_OCSTATE_DISABLE;
  TIM_OC_InitStruct.OCNState = LL_TIM_OCSTATE_DISABLE;
  TIM_OC_InitStruct.CompareValue = 200;
  LL_TIM_OC_Init(TIM1, LL_TIM_CHANNEL_CH4, &TIM_OC_InitStruct);
  LL_TIM_OC_DisableFast(TIM1, LL_TIM_CHANNEL_CH4);
  LL_TIM_CC_EnableChannel(TIM1,  LL_TIM_CHANNEL_CH4); 
   
  TIM1->BDTR |= TIM_BDTR_MOE;
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE BEGIN TIM1_Init 2 */
 


  /* USER CODE END TIM1_Init 2 */
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOE);
  /**TIM1 GPIO Configuration
  PE9   ------> TIM1_CH1        Motor 4
  PE11   ------> TIM1_CH2       Motor 5
  PE13   ------> TIM1_CH3       Motor 6
  PE14   ------> TIM1_CH4       Motor 7
  */
  GPIO_InitStruct.Pin = LL_GPIO_PIN_9;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_0;
  LL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = LL_GPIO_PIN_11;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_0;
  LL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = LL_GPIO_PIN_13;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_0;
  LL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = LL_GPIO_PIN_14;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_0;
  LL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  LL_TIM_EnableCounter(TIM1);
  
   
}

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  LL_TIM_InitTypeDef TIM_InitStruct = {0};
  LL_TIM_OC_InitTypeDef TIM_OC_InitStruct = {0};

  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* Peripheral clock enable */
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM3);

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  TIM_InitStruct.Prescaler = 5; //1.5/5=0.3MHz (3.33 us each stick)
  TIM_InitStruct.CounterMode = LL_TIM_COUNTERMODE_UP;
  TIM_InitStruct.Autoreload = 799; //2.6ms clock cycle
  TIM_InitStruct.ClockDivision = LL_TIM_CLOCKDIVISION_DIV1;
  LL_TIM_Init(TIM3, &TIM_InitStruct);
  LL_TIM_DisableARRPreload(TIM3);
  
  LL_TIM_SetClockSource(TIM3, LL_TIM_CLOCKSOURCE_INTERNAL);
  LL_TIM_SetTriggerOutput(TIM3, LL_TIM_TRGO_RESET);
  LL_TIM_DisableMasterSlaveMode(TIM3);
  
   //Motor 0-PC6
  LL_TIM_OC_EnablePreload(TIM3, LL_TIM_CHANNEL_CH1);
  TIM_OC_InitStruct.OCMode = LL_TIM_OCMODE_PWM2;
  TIM_OC_InitStruct.OCState = LL_TIM_OCSTATE_DISABLE;
  TIM_OC_InitStruct.OCNState = LL_TIM_OCSTATE_DISABLE;
  TIM_OC_InitStruct.CompareValue = 400;//(3.33*400=1332us) //1.33ms
  TIM_OC_InitStruct.OCPolarity = LL_TIM_OCPOLARITY_HIGH;
  LL_TIM_OC_Init(TIM3, LL_TIM_CHANNEL_CH1, &TIM_OC_InitStruct);
  LL_TIM_OC_DisableFast(TIM3, LL_TIM_CHANNEL_CH1);
  LL_TIM_CC_EnableChannel(TIM3,  LL_TIM_CHANNEL_CH1); 
  
   //Motor 1-PC7
  LL_TIM_OC_EnablePreload(TIM3, LL_TIM_CHANNEL_CH2);
  TIM_OC_InitStruct.OCState = LL_TIM_OCSTATE_DISABLE;
  TIM_OC_InitStruct.OCNState = LL_TIM_OCSTATE_DISABLE;
  TIM_OC_InitStruct.CompareValue = 350;//(3.33*350=1165us) //1.16ms
  LL_TIM_OC_Init(TIM3, LL_TIM_CHANNEL_CH2, &TIM_OC_InitStruct);
  LL_TIM_OC_DisableFast(TIM3, LL_TIM_CHANNEL_CH2);
  LL_TIM_CC_EnableChannel(TIM3,  LL_TIM_CHANNEL_CH2); 
  
   //Motor 2-PC8
  LL_TIM_OC_EnablePreload(TIM3, LL_TIM_CHANNEL_CH3);
  TIM_OC_InitStruct.OCState = LL_TIM_OCSTATE_DISABLE;
  TIM_OC_InitStruct.OCNState = LL_TIM_OCSTATE_DISABLE;
  TIM_OC_InitStruct.CompareValue = 600;//(3.33*600=1998us) //1.99ms
  LL_TIM_OC_Init(TIM3, LL_TIM_CHANNEL_CH3, &TIM_OC_InitStruct);
  LL_TIM_OC_DisableFast(TIM3, LL_TIM_CHANNEL_CH3);
  LL_TIM_CC_EnableChannel(TIM3,  LL_TIM_CHANNEL_CH3); 
  
   //Motor 3-PC9
  LL_TIM_OC_EnablePreload(TIM3, LL_TIM_CHANNEL_CH4);
  TIM_OC_InitStruct.OCState = LL_TIM_OCSTATE_DISABLE;
  TIM_OC_InitStruct.OCNState = LL_TIM_OCSTATE_DISABLE;
  TIM_OC_InitStruct.CompareValue = 200;//(3.33*200=666us) //0.66ms
  LL_TIM_OC_Init(TIM3, LL_TIM_CHANNEL_CH4, &TIM_OC_InitStruct);
  LL_TIM_OC_DisableFast(TIM3, LL_TIM_CHANNEL_CH4);
  LL_TIM_CC_EnableChannel(TIM3,  LL_TIM_CHANNEL_CH4); 
   
 
  /* USER CODE BEGIN TIM3_Init 2 */
  
  /* USER CODE END TIM3_Init 2 */
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOC);

  /**TIM3 GPIO Configuration
  PC6   ------> TIM3_CH1
  PC7   ------> TIM3_CH2
  PC8   ------> TIM3_CH3
  PC9   ------> TIM3_CH4
  */
  GPIO_InitStruct.Pin = LL_GPIO_PIN_6;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_0;
  LL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = LL_GPIO_PIN_7;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_0;
  LL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = LL_GPIO_PIN_8;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_0;
  LL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = LL_GPIO_PIN_9;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_0;
  LL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  LL_TIM_EnableCounter(TIM3);

}

static void MX_TIM2_Init(void)
{
  LL_TIM_InitTypeDef TIM_InitStruct = {0};
  /* Peripheral clock enable */
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM2);
  TIM_InitStruct.Prescaler = 48-LL_TIM_IC_FILTER_FDIV1_N2;
  TIM_InitStruct.CounterMode = LL_TIM_COUNTERMODE_UP;
  TIM_InitStruct.Autoreload = 999;
  TIM_InitStruct.ClockDivision = LL_TIM_CLOCKDIVISION_DIV1;
  LL_TIM_Init(TIM2, &TIM_InitStruct);
  LL_TIM_EnableARRPreload(TIM2);
  LL_TIM_SetClockSource(TIM2, LL_TIM_CLOCKSOURCE_INTERNAL);
  LL_TIM_SetTriggerOutput(TIM2, LL_TIM_TRGO_RESET);
  LL_TIM_DisableMasterSlaveMode(TIM2);
  
   LL_TIM_EnableCounter(TIM2);
}




/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOF);
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB);
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOE);
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOD);
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOC);

  /**/
  LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_1);

  /**/
  LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_2);

  /**/
  LL_GPIO_ResetOutputPin(GPIOE, LL_GPIO_PIN_7);

  /**/
  LL_GPIO_ResetOutputPin(GPIOE, LL_GPIO_PIN_8);

  /**/
  LL_GPIO_ResetOutputPin(GPIOE, LL_GPIO_PIN_10);

  /**/
  LL_GPIO_ResetOutputPin(GPIOE, LL_GPIO_PIN_12);

  /**/
  LL_GPIO_ResetOutputPin(GPIOE, LL_GPIO_PIN_15);

  /**/
  LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_10);

  /**/
  LL_GPIO_ResetOutputPin(GPIOD, LL_GPIO_PIN_12);

  /**/
  LL_GPIO_ResetOutputPin(GPIOD, LL_GPIO_PIN_13);

  /**/
  LL_GPIO_ResetOutputPin(GPIOD, LL_GPIO_PIN_14);

  /**/
  LL_GPIO_ResetOutputPin(GPIOD, LL_GPIO_PIN_15);

  /**/
  LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_8);

  /**/
  LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_9);

  /**/
  LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_10);

  /**/
  LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_11);

  /**/
  
  

  //Motor 4 ,IN-B
  GPIO_InitStruct.Pin = LL_GPIO_PIN_1;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(GPIOB, &GPIO_InitStruct); 

  
  //Motor 4 ,IN-A
  GPIO_InitStruct.Pin = LL_GPIO_PIN_2;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  
  //Motor 5 ,IN-B
  GPIO_InitStruct.Pin = LL_GPIO_PIN_7;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  
  //Motor 5 ,IN-A
  GPIO_InitStruct.Pin = LL_GPIO_PIN_8;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  
  //Motor 6 ,IN-B
  GPIO_InitStruct.Pin = LL_GPIO_PIN_10;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  
  //Motor 6 ,IN-A
  GPIO_InitStruct.Pin = LL_GPIO_PIN_12;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  
  //Motor 7 ,IN-B
  GPIO_InitStruct.Pin = LL_GPIO_PIN_15;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  
  //Motor 7 ,IN-A
  GPIO_InitStruct.Pin = LL_GPIO_PIN_10;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  
  //Motor 1 ,IN-A
  GPIO_InitStruct.Pin = LL_GPIO_PIN_12;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  
   //Motor 1 ,IN-B
  GPIO_InitStruct.Pin = LL_GPIO_PIN_13;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  
   //Motor 0 ,IN-A
  GPIO_InitStruct.Pin = LL_GPIO_PIN_14;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  
  //Motor 0 ,IN-B
  GPIO_InitStruct.Pin = LL_GPIO_PIN_15;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  
  //Motor 2 ,IN-A
  GPIO_InitStruct.Pin = LL_GPIO_PIN_8;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  
  //Motor 2 ,IN-B
  GPIO_InitStruct.Pin = LL_GPIO_PIN_9;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  
  //Motor 3 ,IN-A
  GPIO_InitStruct.Pin = LL_GPIO_PIN_10;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  
  //Motor 3 ,IN-B
  GPIO_InitStruct.Pin = LL_GPIO_PIN_11;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(GPIOA, &GPIO_InitStruct);
 
  
  //LED1
  GPIO_InitStruct.Pin = LL_GPIO_PIN_7;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(GPIOD, &GPIO_InitStruct);
  
  //LED2
  GPIO_InitStruct.Pin = LL_GPIO_PIN_3;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(GPIOB, &GPIO_InitStruct);
  
  //LED3
  GPIO_InitStruct.Pin = LL_GPIO_PIN_4;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(GPIOB, &GPIO_InitStruct);
  
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
