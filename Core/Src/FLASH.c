#include "FLASH.h"
#include "stm32f0xx_flash.h"
#include "string.h"
#include "stdio.h"



//uint32_t Flash_Write_Data (uint32_t StartPageAddress, float * DATA_16)
uint32_t Flash_Write_Data (uint32_t StartPageAddress, uint16_t * DATA_16)
{
        
  	int i=0;
        uint16_t DATA;
        //uint32_t DATA;
        //int numberofwords = (strlen((char *)DATA_16)/2) + ((strlen((char *)DATA_16) % 2) != 0);
        FLASH_Unlock();
        FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPERR);
        if (FLASH_ErasePage(StartPageAddress)!= FLASH_COMPLETE)
          {
            while (1){
            }
         }
           
	   /* Program the user Flash area word by word*/

          while (StartPageAddress < 0x0800FC23)
	   {
              DATA= *((int16_t *)&DATA_16[i]);
             //DATA= *((uint32_t *)&DATA_16[i]);
              
             //if (FLASH_ProgramWord(StartPageAddress, DATA) == FLASH_COMPLETE)
             if (FLASH_ProgramHalfWord(StartPageAddress, DATA) == FLASH_COMPLETE)
              {
                  StartPageAddress += 2; 
                //StartPageAddress += 4; 
                  i++;
              }
              else
              {
                while (1){
                }
              }
	   }

	   /* Lock the Flash to disable the flash control register access (recommended
	      to protect the FLASH memory against possible unwanted operation) *********/
	   FLASH_Lock();

	   return 0;
}

//void Flash_Read_Data (uint32_t StartPageAddress, float * DATA_16)
void Flash_Read_Data (uint32_t StartPageAddress, uint16_t * DATA_16)
{
	while (1)
	{
                *DATA_16 = *(uint16_t *)StartPageAddress;
               // *DATA_16 = *(float *)StartPageAddress;
              //*DATA_16 = *(float *)StartPageAddress;
                //if (*(uint32_t *)StartPageAddress==0xffffffff)
               if (*(uint16_t *)StartPageAddress==0xffff)
		{
			*DATA_16 = '\0';
			break;
		}
                StartPageAddress += 2;
               // StartPageAddress += 4;
		DATA_16++;
	}
}