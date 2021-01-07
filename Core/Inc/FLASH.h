
#include "stm32f0xx.h"


typedef struct
{
  uint32_t TypeErase;   /*!< TypeErase: Mass erase or page erase.
                             This parameter can be a value of @ref FLASHEx_Type_Erase */

  uint32_t PageAddress; /*!< PageAdress: Initial FLASH page address to erase when mass erase is disabled
                             This parameter must be a number between Min_Data = FLASH_BASE and Max_Data = FLASH_BANK1_END */
  
  uint32_t NbPages;     /*!< NbPages: Number of pagess to be erased.
                             This parameter must be a value between Min_Data = 1 and Max_Data = (max number of pages - value of initial page)*/
                                                          
} FLASH_EraseInitTypeDef;


#define FLASH_TYPEERASE_PAGES     (0x00U)  /*!<Pages erase only*/
#define FLASH_TYPEERASE_MASSERASE (0x01U)  /*!<Flash mass erase activation*/



#define FLASH_PAGE_SIZE          0x800U

uint32_t Flash_Write_Data (uint32_t StartPageAddress, uint16_t * DATA_16);
//uint32_t Flash_Write_Data (uint32_t StartPageAddress, float * DATA_16);
void Flash_Read_Data (uint32_t StartPageAddress, uint16_t * DATA_16);
//void Flash_Read_Data (uint32_t StartPageAddress, float * DATA_16);