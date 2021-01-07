#ifndef BNO055_STM32_H_
#define BNO055_STM32_H_

#ifdef __cplusplus
  extern "C" {
#endif

//#include "i2c.h"

#include "main.h"    
    
#include "bno055.h"



void bno055_delay(int time) {
  delayMS(time);
}

void bno055_writeData(uint8_t reg, uint8_t data) {

  while(LL_I2C_IsActiveFlag_BUSY(I2C1)){};
  
  LL_I2C_HandleTransfer(I2C1, (BNO055_I2C_ADDR<<1) , LL_I2C_ADDRSLAVE_7BIT,2,LL_I2C_MODE_AUTOEND,LL_I2C_GENERATE_START_WRITE);

  while(!LL_I2C_IsActiveFlag_TXIS(I2C1)){};
  LL_I2C_TransmitData8(I2C1,reg);   
  while(!LL_I2C_IsActiveFlag_TXIS(I2C1)){};
  LL_I2C_TransmitData8(I2C1,data);   
  
  while (!LL_I2C_IsActiveFlag_STOP(I2C1)){};
  LL_I2C_ClearFlag_STOP(I2C1);
  
  I2C1->CR2 &= ((uint32_t)~((uint32_t)(I2C_CR2_SADD | I2C_CR2_HEAD10R | I2C_CR2_NBYTES | I2C_CR2_RELOAD | I2C_CR2_RD_WRN)));
         
}

void bno055_readData(uint8_t reg, uint8_t *data, uint8_t len) {
  
  int index=0;
  while(LL_I2C_IsActiveFlag_BUSY(I2C1)){};
  LL_I2C_HandleTransfer(I2C1, (BNO055_I2C_ADDR<<1) , LL_I2C_ADDRSLAVE_7BIT,1,LL_I2C_MODE_AUTOEND,LL_I2C_GENERATE_START_WRITE);
  while(!LL_I2C_IsActiveFlag_TXIS(I2C1)){};
  LL_I2C_TransmitData8(I2C1,reg);    
  while (!LL_I2C_IsActiveFlag_STOP(I2C1)){};
  LL_I2C_ClearFlag_STOP(I2C1);
  I2C1->CR2 &= ((uint32_t)~((uint32_t)(I2C_CR2_SADD | I2C_CR2_HEAD10R | I2C_CR2_NBYTES | I2C_CR2_RELOAD | I2C_CR2_RD_WRN)));
 
  while(LL_I2C_IsActiveFlag_BUSY(I2C1)){};
  LL_I2C_HandleTransfer(I2C1,(BNO055_I2C_ADDR<<1), LL_I2C_ADDRSLAVE_7BIT,len,LL_I2C_MODE_AUTOEND,LL_I2C_GENERATE_START_READ);
  while(index < len){
        while(!LL_I2C_IsActiveFlag_RXNE(I2C1)){};  
        data[index] = LL_I2C_ReceiveData8(I2C1);
        index++;
  }
  while (!LL_I2C_IsActiveFlag_STOP(I2C1)){};
  LL_I2C_ClearFlag_STOP(I2C1);  
  I2C1->CR2 &= ((uint32_t)~((uint32_t)(I2C_CR2_SADD | I2C_CR2_HEAD10R | I2C_CR2_NBYTES | I2C_CR2_RELOAD | I2C_CR2_RD_WRN)));
    
}

#ifdef __cplusplus
  }
#endif

#endif  // BNO055_STM32_H_
