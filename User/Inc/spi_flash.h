#ifndef __SPI_FLASH_H
#define __SPI_FLASH_H

#include "stm32f4xx.h"
extern SPI_HandleTypeDef hspi1;
#define sFLASH_CMD_WRITE          0x02  /* Write to Memory instruction */
#define sFLASH_CMD_WRSR           0x01  /* Write Status Register instruction */
#define sFLASH_CMD_WREN           0x06  /* Write enable instruction */
#define sFLASH_CMD_READ           0x03  /* Read from Memory instruction */
#define sFLASH_CMD_RDSR           0x05  /* Read Status Register instruction  */
#define sFLASH_CMD_RDID           0x9F  /* Read identification */
#define sFLASH_CMD_SE             0x20  /* Sector Erase instruction */
#define sFLASH_CMD_BE             0xD8  /* Bulk Erase instruction */

#define sFLASH_WIP_FLAG           0x01  /* Write In Progress (WIP) flag */

#define sFLASH_DUMMY_BYTE         0xA5

#define sFLASH_SPI_PAGESIZE       256

#define  sFLASH_ID                  0XEF4017     //W25Q64
#define Dummy_Byte                      0xA5
//#define  sFLASH_ID                0XEF4018     //W25Q128

/* M25P FLASH SPI Interface pins  */  


#define sFLASH_CS_PIN                        GPIO_PIN_3
#define sFLASH_CS_GPIO_PORT                  GPIOG
//#define sFLASH_CS_GPIO_CLK                   RCC_AHB1Periph_GPIOG

/* Exported macro ------------------------------------------------------------*/
/* Select sFLASH: Chip Select pin low */
#define sFLASH_CS_LOW()       HAL_GPIO_WritePin(sFLASH_CS_GPIO_PORT,sFLASH_CS_PIN,GPIO_PIN_RESET);
/* Deselect sFLASH: Chip Select pin high */
#define sFLASH_CS_HIGH()      HAL_GPIO_WritePin(sFLASH_CS_GPIO_PORT,sFLASH_CS_PIN,GPIO_PIN_SET);

/* Exported functions ------------------------------------------------------- */

/* High layer functions  */
void sFLASH_DeInit(void);
void sFLASH_Init(void);
void sFLASH_EraseSector(uint32_t SectorAddr);
void sFLASH_EraseBulk(void);
void sFLASH_WritePage(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite);
void sFLASH_WriteBuffer(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite);
void sFLASH_ReadBuffer(uint8_t* pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead);
uint32_t sFLASH_ReadID(void);
void sFLASH_StartReadSequence(uint32_t ReadAddr);

/* Low layer functions */
uint8_t sFLASH_ReadByte(void);
uint8_t sFLASH_SendByte(uint8_t byte);
uint16_t sFLASH_SendHalfWord(uint16_t HalfWord);
void sFLASH_WriteEnable(void);
void sFLASH_WaitForWriteEnd(void);

#endif /* __SPI_FLASH_H */


