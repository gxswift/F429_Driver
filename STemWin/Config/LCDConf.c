/**
  ******************************************************************************
  * @file    lcdconf.c
  * @author  MCD Application Team
  * @version V1.0.1
  * @date    21-September-2015
  * @brief   This file implements the configuration for the GUI library
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2015 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#include "LCDConf.h"
#include "GUI_Private.h"
#include "ltdc.h"
#include "dma2d.h"

/** @addtogroup LCD CONFIGURATION
* @{
*/

/** @defgroup LCD CONFIGURATION
* @brief This file contains the LCD Configuration
* @{
*/ 

/** @defgroup LCD CONFIGURATION_Private_TypesDefinitions
* @{
*/ 
/**
* @}
*/ 

/** @defgroup LCD CONFIGURATION_Private_Defines
* @{
*/ 




#undef  LCD_SWAP_XY
#undef  LCD_MIRROR_Y

#undef  LCD_SWAP_RB

#define LCD_SWAP_XY  0
#define LCD_MIRROR_Y 1

#define LCD_SWAP_RB  1

#define XSIZE_PHYS LCD_Width
#define YSIZE_PHYS LCD_Height

static U32 aBufferDMA2D [XSIZE_PHYS * sizeof(U32)];
static U32 aBuffer_FG   [XSIZE_PHYS * sizeof(U32)];
static U32 aBuffer_BG   [XSIZE_PHYS * sizeof(U32)];

#define NUM_BUFFERS  3 /* Number of multiple buffers to be used */
#define NUM_VSCREENS 1 /* Number of virtual screens to be used */

#define BK_COLOR GUI_DARKBLUE

#undef  GUI_NUM_LAYERS
#define GUI_NUM_LAYERS 1

#define COLOR_CONVERSION_0 GUICC_M888
#define DISPLAY_DRIVER_0   GUIDRV_LIN_24


#if (GUI_NUM_LAYERS > 1)
  #define COLOR_CONVERSION_1 GUICC_M8888I
  #define DISPLAY_DRIVER_1   GUIDRV_LIN_32

#endif

#ifndef   XSIZE_PHYS
  #error Physical X size of display is not defined!
#endif
#ifndef   YSIZE_PHYS
  #error Physical Y size of display is not defined!
#endif
#ifndef   NUM_VSCREENS
  #define NUM_VSCREENS 1
#else
  #if (NUM_VSCREENS <= 0)
    #error At least one screeen needs to be defined!
  #endif
#endif
#if (NUM_VSCREENS > 1) && (NUM_BUFFERS > 1)
  #error Virtual screens and multiple buffers are not allowed!
#endif

/* From SDRAM */
#define LCD_LAYER0_FRAME_BUFFER  LCD_MemoryAdd
#define LCD_LAYER1_FRAME_BUFFER  LCD_MemoryAdd_OFFSET+LCD_MemoryAdd
#define DEFINEDMA2D_COLORCONVERSION(PFIX, PIXELFORMAT)                                                             \
static void Color2IndexBulk_##PFIX##DMA2D(LCD_COLOR * pColor, void * pIndex, U32 NumItems, U8 SizeOfIndex) { \
  DMA2D_Color2IndexBulk(pColor, pIndex, NumItems, SizeOfIndex, PIXELFORMAT);                                         \
}                                                                                                                   \
static void Index2ColorBulk_##PFIX##DMA2D(void * pIndex, LCD_COLOR * pColor, U32 NumItems, U8 SizeOfIndex) { \
  DMA2D_Index2ColorBulk(pIndex, pColor, NumItems, SizeOfIndex, PIXELFORMAT);  \
}
  
/**
* @}
*/ 


/** @defgroup LCD CONFIGURATION_Private_Macros
* @{
*/ 
/**
* @}
*/ 


/** @defgroup LCD CONFIGURATION_Private_Variables
* @{
*/
//LTDC_HandleTypeDef                   hltdc;  
//static DMA2D_HandleTypeDef           hdma2d;
static LCD_LayerPropTypedef          layer_prop[GUI_NUM_LAYERS];




static const LCD_API_COLOR_CONV * apColorConvAPI[] = 
{
  COLOR_CONVERSION_0,
#if GUI_NUM_LAYERS > 1
  COLOR_CONVERSION_1,
#endif
};

/**
* @}
*/ 

/** @defgroup LCD CONFIGURATION_Private_FunctionPrototypes
* @{
*/ 
static void     DMA2D_CopyBuffer         (U32 LayerIndex, void * pSrc, void * pDst, U32 xSize, U32 ySize, U32 OffLineSrc, U32 OffLineDst);
static void     DMA2D_FillBuffer(U32 LayerIndex, void * pDst, U32 xSize, U32 ySize, U32 OffLine, U32 ColorIndex);
static void     LCD_LL_Init(void); 
static void     LCD_LL_LayerInit(U32 LayerIndex); 

static void     CUSTOM_CopyBuffer(int LayerIndex, int IndexSrc, int IndexDst);
static void     CUSTOM_CopyRect(int LayerIndex, int x0, int y0, int x1, int y1, int xSize, int ySize);
static void     CUSTOM_FillRect(int LayerIndex, int x0, int y0, int x1, int y1, U32 PixelIndex);
static void     CUSTOM_DrawBitmap32bpp(int LayerIndex, int x, int y, U8 const * p,  int xSize, int ySize, int BytesPerLine);
static U32      GetBufferSize(U32 LayerIndex);

static void DMA2D_AlphaBlending(LCD_COLOR * pColorFG, LCD_COLOR * pColorBG, LCD_COLOR * pColorDst, U32 NumItems);
static LCD_PIXELINDEX * _LCD_GetpPalConvTable(const LCD_LOGPALETTE GUI_UNI_PTR * pLogPal, const GUI_BITMAP GUI_UNI_PTR * pBitmap, int LayerIndex);
static LCD_COLOR DMA2D_MixColors(LCD_COLOR Color, LCD_COLOR BkColor, U8 Intens);
static void LCD_MixColorsBulk(U32 * pFG, U32 * pBG, U32 * pDst, unsigned OffFG, unsigned OffBG, unsigned OffDest, unsigned xSize, unsigned ySize, U8 Intens);


DEFINEDMA2D_COLORCONVERSION(M8888I, LTDC_PIXEL_FORMAT_ARGB8888)
DEFINEDMA2D_COLORCONVERSION(M888,   LTDC_PIXEL_FORMAT_ARGB8888)
DEFINEDMA2D_COLORCONVERSION(M565,   LTDC_PIXEL_FORMAT_RGB565)
DEFINEDMA2D_COLORCONVERSION(M1555I, LTDC_PIXEL_FORMAT_ARGB1555)
DEFINEDMA2D_COLORCONVERSION(M4444I, LTDC_PIXEL_FORMAT_ARGB4444)


/**
* @}
*/ 

/** @defgroup LCD CONFIGURATION_Private_Functions
* @{
*/ 
/**
  * @brief  Return Pixel format for a given layer
  * @param  LayerIndex : Layer Index 
  * @retval Status ( 0 : 0k , 1: error)
  */
static inline U32 LCD_LL_GetPixelformat(U32 LayerIndex)
{
  if (LayerIndex == 0)
  {
    return LTDC_PIXEL_FORMAT_RGB565;
  } 
  else
  {
    return LTDC_PIXEL_FORMAT_ARGB1555;
  } 
}
/*******************************************************************************
                       LTDC and DMA2D BSP Routines
*******************************************************************************/
/**
  * @brief DMA2D MSP Initialization 
  *        This function configures the hardware resources used in this example: 
  *           - Peripheral's clock enable
  *           - Peripheral's GPIO Configuration  
  * @param hdma2d: DMA2D handle pointer
  * @retval None
  */
//void HAL_DMA2D_MspInit(DMA2D_HandleTypeDef *hdma2d)
//{  
//  /* Enable peripheral */
//  __HAL_RCC_DMA2D_CLK_ENABLE();   
//}

///**
//  * @brief DMA2D MSP De-Initialization 
//  *        This function frees the hardware resources used in this example:
//  *          - Disable the Peripheral's clock
//  * @param hdma2d: DMA2D handle pointer
//  * @retval None
//  */
//void HAL_DMA2D_MspDeInit(DMA2D_HandleTypeDef *hdma2d)
//{
//  /* Enable DMA2D reset state */
//  __HAL_RCC_DMA2D_FORCE_RESET();
//  
//  /* Release DMA2D from reset state */ 
//  __HAL_RCC_DMA2D_RELEASE_RESET();
//}

///**
//  * @brief LTDC MSP Initialization 
//  *        This function configures the hardware resources used in this example: 
//  *           - Peripheral's clock enable
//  *           - Peripheral's GPIO Configuration  
//  * @param hltdc: LTDC handle pointer
//  * @retval None
//  */
//void HAL_LTDC_MspInit(LTDC_HandleTypeDef *hltdc)
//{  
//  GPIO_InitTypeDef gpio_init_structure;
//  static RCC_PeriphCLKInitTypeDef  PeriphClkInitStruct;
//  
//  /* Enable the LTDC clocks */
//  __HAL_RCC_LTDC_CLK_ENABLE();
//  
//  
//  /* LCD clock configuration */
//  /* PLLSAI_VCO Input = HSE_VALUE/PLL_M = 1 Mhz */
//  /* PLLSAI_VCO Output = PLLSAI_VCO Input * PLLSAIN = 429 Mhz */
//  /* PLLLCDCLK = PLLSAI_VCO Output/PLLSAIR = 429/5 = 85 Mhz */
//  /* LTDC clock frequency = PLLLCDCLK / LTDC_PLLSAI_DIVR_2 = 85/4 = 21 Mhz */
//  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_LTDC;
//  PeriphClkInitStruct.PLLSAI.PLLSAIN = 192;
//  PeriphClkInitStruct.PLLSAI.PLLSAIR = 5;
//  PeriphClkInitStruct.PLLSAIDivR = RCC_PLLSAIDIVR_4;
//  HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct);   
//  
//  
//  /* Enable GPIOs clock */
//  __HAL_RCC_GPIOE_CLK_ENABLE();
//  __HAL_RCC_GPIOG_CLK_ENABLE();
//  __HAL_RCC_GPIOI_CLK_ENABLE();
//  __HAL_RCC_GPIOJ_CLK_ENABLE();
//  __HAL_RCC_GPIOK_CLK_ENABLE();
//  
//  /*** LTDC Pins configuration ***/
//  /* GPIOE configuration */
//  gpio_init_structure.Pin       = GPIO_PIN_4;
//  gpio_init_structure.Mode      = GPIO_MODE_AF_PP;
//  gpio_init_structure.Pull      = GPIO_NOPULL;
//  gpio_init_structure.Speed     = GPIO_SPEED_FAST;
//  gpio_init_structure.Alternate = GPIO_AF14_LTDC;  
//  HAL_GPIO_Init(GPIOE, &gpio_init_structure);

//  /* GPIOG configuration */
//  gpio_init_structure.Pin       = GPIO_PIN_12;
//  gpio_init_structure.Mode      = GPIO_MODE_AF_PP;
//  gpio_init_structure.Alternate = GPIO_AF9_LTDC;
//  HAL_GPIO_Init(GPIOG, &gpio_init_structure);

//  /* GPIOI LTDC alternate configuration */
//  gpio_init_structure.Pin       = GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | \
//                                  GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
//  gpio_init_structure.Mode      = GPIO_MODE_AF_PP;
//  gpio_init_structure.Alternate = GPIO_AF14_LTDC;
//  HAL_GPIO_Init(GPIOI, &gpio_init_structure);

//  /* GPIOJ configuration */  
//  gpio_init_structure.Pin       = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | \
//                                  GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7 | \
//                                  GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 | \
//                                  GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
//  gpio_init_structure.Mode      = GPIO_MODE_AF_PP;
//  gpio_init_structure.Alternate = GPIO_AF14_LTDC;
//  HAL_GPIO_Init(GPIOJ, &gpio_init_structure);  

//  /* GPIOK configuration */  
//  gpio_init_structure.Pin       = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_4 | \
//                                  GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7;
//  gpio_init_structure.Mode      = GPIO_MODE_AF_PP;
//  gpio_init_structure.Alternate = GPIO_AF14_LTDC;
//  HAL_GPIO_Init(GPIOK, &gpio_init_structure);
//  
//  /* LCD_DISP GPIO configuration */
//  gpio_init_structure.Pin       = GPIO_PIN_12;     /* LCD_DISP pin has to be manually controlled */
//  gpio_init_structure.Mode      = GPIO_MODE_OUTPUT_PP;
//  HAL_GPIO_Init(GPIOI, &gpio_init_structure);

//  /* LCD_BL_CTRL GPIO configuration */
//  gpio_init_structure.Pin       = GPIO_PIN_3;  /* LCD_BL_CTRL pin has to be manually controlled */
//  gpio_init_structure.Mode      = GPIO_MODE_OUTPUT_PP;
//  HAL_GPIO_Init(GPIOK, &gpio_init_structure);

//  /* Set LTDC Interrupt to the lowest priority */
//  HAL_NVIC_SetPriority(LTDC_IRQn, 0xE, 0);
//  
//  /* Enable LTDC Interrupt */
//  HAL_NVIC_EnableIRQ(LTDC_IRQn);
//}

///**
//  * @brief LTDC MSP De-Initialization 
//  *        This function frees the hardware resources used in this example:
//  *          - Disable the Peripheral's clock
//  * @param hltdc: LTDC handle pointer
//  * @retval None
//  */
//void HAL_LTDC_MspDeInit(LTDC_HandleTypeDef *hltdc)
//{
//  /* Reset peripherals */
//  /* Enable LTDC reset state */
//  __HAL_RCC_LTDC_FORCE_RESET();
//  
//  /* Release LTDC from reset state */ 
//  __HAL_RCC_LTDC_RELEASE_RESET();
//}

/**
  * @brief  Line Event callback.
  * @param  hltdc: pointer to a LTDC_HandleTypeDef structure that contains
  *                the configuration information for the specified LTDC.
  * @retval None
  */
	static void DMA2D_AlphaBlendingBulk(LCD_COLOR * pColorFG, LCD_COLOR * pColorBG, LCD_COLOR * pColorDst, U32 NumItems) 
{
  /* Set up mode */
  DMA2D->CR      = 0x00020000UL | (1 << 9);         /* Control Register (Memory to memory with blending of FG and BG and TCIE) */
  
  /* Set up pointers */
  DMA2D->FGMAR   = (uint32_t)pColorFG;                   /* Foreground Memory Address Register */
  DMA2D->BGMAR   = (uint32_t)pColorBG;                   /* Background Memory Address Register */
  DMA2D->OMAR    = (uint32_t)pColorDst;                  /* Output Memory Address Register (Destination address) */
  
  /* Set up offsets */
  DMA2D->FGOR    = 0;                               /* Foreground Offset Register */
  DMA2D->BGOR    = 0;                               /* Background Offset Register */
  DMA2D->OOR     = 0;                               /* Output Offset Register */
  
  /* Set up pixel format */
  DMA2D->FGPFCCR = LTDC_PIXEL_FORMAT_ARGB8888;       /* Foreground PFC Control Register (Defines the FG pixel format) */
  DMA2D->BGPFCCR = LTDC_PIXEL_FORMAT_ARGB8888;       /* Background PFC Control Register (Defines the BG pixel format) */
  DMA2D->OPFCCR  = LTDC_PIXEL_FORMAT_ARGB8888;       /* Output     PFC Control Register (Defines the output pixel format) */
  
  /* Set up size */
  DMA2D->NLR     = (uint32_t)(NumItems << 16) | 1;       /* Number of Line Register (Size configuration of area to be transfered) */
  
  /* Execute operation */
  
  DMA2D->CR     |= DMA2D_CR_START;  
    
  while (DMA2D->CR & DMA2D_CR_START) 
  {
  }
}
void HAL_LTDC_LineEvenCallback(LTDC_HandleTypeDef *hltdc) {
  U32 Addr;
  U32 layer;
  printf("HAL_LTDC_LineEvenCallback------start\r\n");
  for (layer = 0; layer < GUI_NUM_LAYERS; layer++)
  {
    if (layer_prop[layer].pending_buffer >= 0) 
    {
      /* Calculate address of buffer to be used  as visible frame buffer */
      Addr = layer_prop[layer].address + \
             layer_prop[layer].xSize * layer_prop[layer].ySize * layer_prop[layer].pending_buffer * layer_prop[layer].BytesPerPixel;
      
      __HAL_LTDC_LAYER(hltdc, layer)->CFBAR = Addr;
     
      __HAL_LTDC_RELOAD_CONFIG(hltdc);
      
      /* Notify STemWin that buffer is used */
      GUI_MULTIBUF_ConfirmEx(layer, layer_prop[layer].pending_buffer);

      /* Clear pending buffer flag of layer */
      layer_prop[layer].pending_buffer = -1;
    }
  }
  printf("HAL_LTDC_LineEvenCallback------end\r\n");
  HAL_LTDC_ProgramLineEvent(hltdc, 0);
}

/*******************************************************************************
                          Display configuration
*******************************************************************************/
/**
  * @brief  Called during the initialization process in order to set up the
  *          display driver configuration
  * @param  None
  * @retval None
  */


void LCD_X_Config(void) 
{
  U32 i;

  LCD_LL_Init ();
    printf("LCD_X_Config------start\r\n");
  /* At first initialize use of multiple buffers on demand */
#if (NUM_BUFFERS > 1)
    for (i = 0; i < GUI_NUM_LAYERS; i++) 
    {
      GUI_MULTIBUF_ConfigEx(i, NUM_BUFFERS);
    }
#endif

  /* Set display driver and color conversion for 1st layer */
  GUI_DEVICE_CreateAndLink(DISPLAY_DRIVER_0, COLOR_CONVERSION_0, 0, 0);
  
  /* Set size of 1st layer */
  if (LCD_GetSwapXYEx(0)) {
    LCD_SetSizeEx (0, YSIZE_PHYS, XSIZE_PHYS);
    LCD_SetVSizeEx(0, YSIZE_PHYS * NUM_VSCREENS, XSIZE_PHYS);
  } else {
    LCD_SetSizeEx (0, XSIZE_PHYS, YSIZE_PHYS);
    LCD_SetVSizeEx(0, XSIZE_PHYS, YSIZE_PHYS * NUM_VSCREENS);
  }
  #if (GUI_NUM_LAYERS > 1)
    
    /* Set display driver and color conversion for 2nd layer */
    GUI_DEVICE_CreateAndLink(DISPLAY_DRIVER_1, COLOR_CONVERSION_1, 0, 1);
    
    /* Set size of 2nd layer */
    if (LCD_GetSwapXYEx(1)) {
      LCD_SetSizeEx (1, YSIZE_PHYS, XSIZE_PHYS);
      LCD_SetVSizeEx(1, YSIZE_PHYS * NUM_VSCREENS, XSIZE_PHYS);
    } else {
      LCD_SetSizeEx (1, XSIZE_PHYS, YSIZE_PHYS);
      LCD_SetVSizeEx(1, XSIZE_PHYS, YSIZE_PHYS * NUM_VSCREENS);
    }
  #endif
  
    
    /*Initialize GUI Layer structure */
    layer_prop[0].address = LCD_LAYER0_FRAME_BUFFER;
    
#if (GUI_NUM_LAYERS > 1)
    layer_prop[1].address = LCD_LAYER1_FRAME_BUFFER; 
#endif
       
   /* Setting up VRam address and custom functions for CopyBuffer-, CopyRect- and FillRect operations */
  for (i = 0; i < GUI_NUM_LAYERS; i++) 
  {

    layer_prop[i].pColorConvAPI = (LCD_API_COLOR_CONV *)apColorConvAPI[i];
     
    layer_prop[i].pending_buffer = -1;

    /* Set VRAM address */
    LCD_SetVRAMAddrEx(i, (void *)(layer_prop[i].address));

    /* Remember color depth for further operations */
    layer_prop[i].BytesPerPixel = LCD_GetBitsPerPixelEx(i) >> 3;

    /* Set custom functions for several operations */
    LCD_SetDevFunc(i, LCD_DEVFUNC_COPYBUFFER, (void(*)(void))CUSTOM_CopyBuffer);
    LCD_SetDevFunc(i, LCD_DEVFUNC_COPYRECT,   (void(*)(void))CUSTOM_CopyRect);
    LCD_SetDevFunc(i, LCD_DEVFUNC_FILLRECT, (void(*)(void))CUSTOM_FillRect);

    /* Set up drawing routine for 16bpp bitmap using DMA2D */
   
    /* Set up drawing routine for 32bpp bitmap using DMA2D */
    if (LCD_LL_GetPixelformat(i) == LTDC_PIXEL_FORMAT_RGB565) {
			printf("LTDC_PIXEL_FORMAT_RGB565\r\n");
     LCD_SetDevFunc(i, LCD_DEVFUNC_DRAWBMP_16BPP, (void(*)(void))CUSTOM_DrawBitmap32bpp);     /* Set up drawing routine for 32bpp bitmap using DMA2D. Makes only sense with ARGB8888 */
    }    
  }
	printf("LCD_X_Config------end\r\n");
	   
    /* Set up custom alpha blending function using DMA2D */
    GUI_SetFuncAlphaBlending(DMA2D_AlphaBlending);                                                /* Set up custom alpha blending function using DMA2D */
    
    /* Set up custom function for translating a bitmap palette into index values.
    Required to load a bitmap palette into DMA2D CLUT in case of a 8bpp indexed bitmap */
    GUI_SetFuncGetpPalConvTable(_LCD_GetpPalConvTable);
    
    /* Set up a custom function for mixing up single colors using DMA2D */
    GUI_SetFuncMixColors(DMA2D_MixColors);
    
    /* Set up a custom function for mixing up arrays of colors using DMA2D */
    GUI_SetFuncMixColorsBulk(LCD_MixColorsBulk);
}
*/
/**
  * @brief  This function is called by the display driver for several purposes.
  *         To support the according task the routine needs to be adapted to
  *         the display controller. Please note that the commands marked with
  *         'optional' are not cogently required and should only be adapted if
  *         the display controller supports these features
  * @param  LayerIndex: Index of layer to be configured 
  * @param  Cmd       :Please refer to the details in the switch statement below
  * @param  pData     :Pointer to a LCD_X_DATA structure
  * @retval Status (-1 : Error,  0 : Ok)
  */
int LCD_X_DisplayDriver(unsigned LayerIndex, unsigned Cmd, void * pData) 
{
  int r = 0;
  U32 addr;
  int xPos, yPos;
  U32 Color;
    //printf("LCD_X_DisplayDriver------start\r\n"); 
  switch (Cmd) 
  {
  case LCD_X_INITCONTROLLER: 
    LCD_LL_LayerInit(LayerIndex);
	 printf("LCD_X_INITCONTROLLER------end %d \r\n",LayerIndex); 
    break;

  case LCD_X_SETORG: 
		printf("LCD_X_SETORG------end %d \r\n",LayerIndex); 
    addr = layer_prop[LayerIndex].address + ((LCD_X_SETORG_INFO *)pData)->yPos * layer_prop[LayerIndex].xSize * layer_prop[LayerIndex].BytesPerPixel;
    HAL_LTDC_SetAddress(&hltdc, addr, LayerIndex);
    break;

  case LCD_X_SHOWBUFFER: 
		printf("LCD_X_SHOWBUFFER------end %d \r\n",LayerIndex); 
    layer_prop[LayerIndex].pending_buffer = ((LCD_X_SHOWBUFFER_INFO *)pData)->Index;
    break;

  case LCD_X_SETLUTENTRY: 
		printf("LCD_X_SETLUTENTRY------end %d \r\n",LayerIndex); 
    HAL_LTDC_ConfigCLUT(&hltdc, (uint32_t *)&(((LCD_X_SETLUTENTRY_INFO *)pData)->Color), 1, LayerIndex);
    break;

  case LCD_X_ON: 
		printf("LCD_X_ON------end %d \r\n",LayerIndex); 
    __HAL_LTDC_ENABLE(&hltdc);
    break;

  case LCD_X_OFF: 
			printf("LCD_X_OFF------end %d \r\n",LayerIndex); 
    __HAL_LTDC_DISABLE(&hltdc);
    break;
    
  case LCD_X_SETVIS:
		printf("LCD_X_SETVIS------end %d \r\n",LayerIndex); 
    if(((LCD_X_SETVIS_INFO *)pData)->OnOff  == ENABLE )
    {
      __HAL_LTDC_LAYER_ENABLE(&hltdc, LayerIndex); 
    }
    else
    {
      __HAL_LTDC_LAYER_DISABLE(&hltdc, LayerIndex); 
    }
    __HAL_LTDC_RELOAD_CONFIG(&hltdc); 
    break;
    
  case LCD_X_SETPOS: 
		printf("LCD_X_SETPOS------end %d \r\n",LayerIndex); 
    HAL_LTDC_SetWindowPosition(&hltdc, 
                               ((LCD_X_SETPOS_INFO *)pData)->xPos, 
                               ((LCD_X_SETPOS_INFO *)pData)->yPos, 
                               LayerIndex);
    break;

  case LCD_X_SETSIZE:
			printf("LCD_X_SETSIZE------end %d \r\n",LayerIndex); 
		
    GUI_GetLayerPosEx(LayerIndex, &xPos, &yPos);
	
    layer_prop[LayerIndex].xSize = ((LCD_X_SETSIZE_INFO *)pData)->xSize;
    layer_prop[LayerIndex].ySize = ((LCD_X_SETSIZE_INFO *)pData)->ySize;
		printf("LCD_X_SETSIZE---x---end %d \r\n",layer_prop[LayerIndex].xSize); 
			printf("LCD_X_SETSIZE---y---end %d \r\n",layer_prop[LayerIndex].ySize); 
    HAL_LTDC_SetWindowPosition(&hltdc, xPos, yPos, LayerIndex);
    break;

  case LCD_X_SETALPHA:
		printf("LCD_X_SETALPHA------end %d \r\n",LayerIndex); 
		printf("LCD_X_SETALPHA---Alpha---end %d \r\n",((LCD_X_SETALPHA_INFO *)pData)->Alpha); 
    HAL_LTDC_SetAlpha(&hltdc, ((LCD_X_SETALPHA_INFO *)pData)->Alpha, LayerIndex);
    break;

  case LCD_X_SETCHROMAMODE:
			printf("LCD_X_SETCHROMAMODE------end %d \r\n",LayerIndex); 
    if(((LCD_X_SETCHROMAMODE_INFO *)pData)->ChromaMode != 0)
    {
      HAL_LTDC_EnableColorKeying(&hltdc, LayerIndex);
    }
    else
    {
      HAL_LTDC_DisableColorKeying(&hltdc, LayerIndex);      
    }
    break;

  case LCD_X_SETCHROMA:
	printf("LCD_X_SETCHROMA------end %d \r\n",LayerIndex); 
    Color = ((((LCD_X_SETCHROMA_INFO *)pData)->ChromaMin & 0xFF0000) >> 16) |\
             (((LCD_X_SETCHROMA_INFO *)pData)->ChromaMin & 0x00FF00) |\
            ((((LCD_X_SETCHROMA_INFO *)pData)->ChromaMin & 0x0000FF) << 16);
    
    HAL_LTDC_ConfigColorKeying(&hltdc, Color, LayerIndex);
    break;

  default:
			printf("default------end %d \r\n",LayerIndex); 
    r = -1;
  }
	// printf("LCD_X_DisplayDriver------end\r\n"); 
  return r;
}

/**
  * @brief  Initialize the LCD Controller.
  * @param  LayerIndex : layer Index.
  * @retval None
  */

/**
  * @brief  Return Pixel format for a given layer
  * @param  LayerIndex : Layer Index 
  * @retval Status ( 0 : 0k , 1: error)
  */
static void DMA2D_CopyBuffer(U32 LayerIndex, void * pSrc, void * pDst, U32 xSize, U32 ySize, U32 OffLineSrc, U32 OffLineDst)
{
 U32 PixelFormat;

  PixelFormat = LCD_LL_GetPixelformat(LayerIndex);
  DMA2D->CR      = 0x00000000UL | (1 << 9);  
  	
  /* Set up pointers */
  DMA2D->FGMAR   = (U32)pSrc;                       
  DMA2D->OMAR    = (U32)pDst;                       
  DMA2D->FGOR    = OffLineSrc;                      
  DMA2D->OOR     = OffLineDst; 
  
  /* Set up pixel format */  
  DMA2D->FGPFCCR = PixelFormat;  
  
  /*  Set up size */
  DMA2D->NLR     = (U32)(xSize << 16) | (U16)ySize; 
  
  DMA2D->CR     |= DMA2D_CR_START;   
 
  /* Wait until transfer is done */
  while (DMA2D->CR & DMA2D_CR_START) 
  {
  }
	

}

/**
  * @brief  Fill Buffer
  * @param  LayerIndex : Layer Index
  * @param  pDst:        pointer to destination
  * @param  xSize:       X size
  * @param  ySize:       Y size
  * @param  OffLine:     offset after each line
  * @param  ColorIndex:  color to be used.           
  * @retval None.
  */
static void DMA2D_FillBuffer(U32 LayerIndex, void * pDst, U32 xSize, U32 ySize, U32 OffLine, U32 ColorIndex) 
{

  U32 PixelFormat;

  PixelFormat = LCD_LL_GetPixelformat(LayerIndex);
	
  /* Set up mode */
  DMA2D->CR      = 0x00030000UL | (1 << 9);        
  DMA2D->OCOLR   = ColorIndex;                     

  /* Set up pointers */
  DMA2D->OMAR    = (U32)pDst;                      

  /* Set up offsets */
  DMA2D->OOR     = OffLine;                        

  /* Set up pixel format */
  DMA2D->OPFCCR  = PixelFormat;                    

  /*  Set up size */
  DMA2D->NLR     = (U32)(xSize << 16) | (U16)ySize;
    
  DMA2D->CR     |= DMA2D_CR_START; 
  
  /* Wait until transfer is done */
  while (DMA2D->CR & DMA2D_CR_START) 
  {
  }
}


/**
  * @brief  Get buffer size
  * @param  LayerIndex : Layer Index           
  * @retval None.
  */
static U32 GetBufferSize(U32 LayerIndex) 
{
  U32 BufferSize;

  BufferSize = layer_prop[LayerIndex].xSize * layer_prop[LayerIndex].ySize * layer_prop[LayerIndex].BytesPerPixel;
  return BufferSize;
}

/**
  * @brief  Customized copy buffer
  * @param  LayerIndex : Layer Index
  * @param  IndexSrc:    index source
  * @param  IndexDst:    index destination           
  * @retval None.
  */
static void CUSTOM_CopyBuffer(int LayerIndex, int IndexSrc, int IndexDst) {
  U32 BufferSize, AddrSrc, AddrDst;

  BufferSize = GetBufferSize(LayerIndex);
  AddrSrc    = layer_prop[LayerIndex].address + BufferSize * IndexSrc;
  AddrDst    = layer_prop[LayerIndex].address + BufferSize * IndexDst;
  DMA2D_CopyBuffer(LayerIndex, (void *)AddrSrc, (void *)AddrDst, layer_prop[LayerIndex].xSize, layer_prop[LayerIndex].ySize, 0, 0);
  layer_prop[LayerIndex].buffer_index = IndexDst;
}

/**
  * @brief  Copy rectangle
  * @param  LayerIndex : Layer Index
  * @param  x0:          X0 position
  * @param  y0:          Y0 position
  * @param  x1:          X1 position
  * @param  y1:          Y1 position
  * @param  xSize:       X size. 
  * @param  ySize:       Y size.            
  * @retval None.
  */
static void CUSTOM_CopyRect(int LayerIndex, int x0, int y0, int x1, int y1, int xSize, int ySize) 
{
  U32 BufferSize, AddrSrc, AddrDst;

  BufferSize = GetBufferSize(LayerIndex);
  AddrSrc = layer_prop[LayerIndex].address + BufferSize * layer_prop[LayerIndex].pending_buffer + (y0 * layer_prop[LayerIndex].xSize + x0) * layer_prop[LayerIndex].BytesPerPixel;
  AddrDst = layer_prop[LayerIndex].address + BufferSize * layer_prop[LayerIndex].pending_buffer + (y1 * layer_prop[LayerIndex].xSize + x1) * layer_prop[LayerIndex].BytesPerPixel;
  DMA2D_CopyBuffer(LayerIndex, (void *)AddrSrc, (void *)AddrDst, xSize, ySize, layer_prop[LayerIndex].xSize - xSize, 0);
}

/**
  * @brief  Fill rectangle
  * @param  LayerIndex : Layer Index
  * @param  x0:          X0 position
  * @param  y0:          Y0 position
  * @param  x1:          X1 position
  * @param  y1:          Y1 position
  * @param  PixelIndex:  Pixel index.             
  * @retval None.
  */
static void CUSTOM_FillRect(int LayerIndex, int x0, int y0, int x1, int y1, U32 PixelIndex) 
{
  U32 BufferSize, AddrDst;
  int xSize, ySize;


  if (GUI_GetDrawMode() == GUI_DM_XOR) 
  {		
    LCD_SetDevFunc(LayerIndex, LCD_DEVFUNC_FILLRECT, NULL);
    LCD_FillRect(x0, y0, x1, y1);
    LCD_SetDevFunc(LayerIndex, LCD_DEVFUNC_FILLRECT, (void(*)(void))CUSTOM_FillRect);
  } 
  else 
  {
    xSize = x1 - x0 + 1;
    ySize = y1 - y0 + 1;
    BufferSize = GetBufferSize(LayerIndex);
		AddrDst = layer_prop[LayerIndex].address + BufferSize * layer_prop[LayerIndex].buffer_index + (y0 * layer_prop[LayerIndex].xSize + x0) * layer_prop[LayerIndex].BytesPerPixel;
    DMA2D_FillBuffer(LayerIndex, (void *)AddrDst, xSize, ySize, layer_prop[LayerIndex].xSize - xSize, PixelIndex);
	}
}

/**
  * @brief  Draw indirect color bitmap
  * @param  pSrc: pointer to the source
  * @param  pDst: pointer to the destination
  * @param  OffSrc: offset source
  * @param  OffDst: offset destination
  * @param  PixelFormatDst: pixel format for destination
  * @param  xSize: X size
  * @param  ySize: Y size
  * @retval None
  */
static void CUSTOM_DrawBitmap32bpp(int LayerIndex, int x, int y, U8 const * p, int xSize, int ySize, int BytesPerLine)
{
	
  U32 BufferSize, AddrDst;
  int OffLineSrc, OffLineDst;

  BufferSize = GetBufferSize(LayerIndex);
  AddrDst = layer_prop[LayerIndex].address + BufferSize * layer_prop[LayerIndex].buffer_index + (y * layer_prop[LayerIndex].xSize + x) * layer_prop[LayerIndex].BytesPerPixel;
  OffLineSrc = (BytesPerLine / 2) - xSize;
  OffLineDst = layer_prop[LayerIndex].xSize - xSize;
  DMA2D_CopyBuffer(LayerIndex, (void *)p, (void *)AddrDst, xSize, ySize, OffLineSrc, OffLineDst);
	

}
static void LCD_LL_LayerInit(U32 LayerIndex) 
{
  LTDC_LayerCfgTypeDef             layer_cfg;
  
  if (LayerIndex < GUI_NUM_LAYERS) 
  { 
    /* Layer configuration */
    layer_cfg.WindowX0 = 0;
    layer_cfg.WindowX1 = XSIZE_PHYS;
    layer_cfg.WindowY0 = 0;
    layer_cfg.WindowY1 = YSIZE_PHYS; 
    layer_cfg.PixelFormat = LCD_LL_GetPixelformat(LayerIndex);
    layer_cfg.FBStartAdress = ((uint32_t)0xD0000000);
    layer_cfg.Alpha = 255;
    layer_cfg.Alpha0 = 2505;
    layer_cfg.Backcolor.Blue = 0;
    layer_cfg.Backcolor.Green = 0;
    layer_cfg.Backcolor.Red = 0;
    layer_cfg.BlendingFactor1 = LTDC_BLENDING_FACTOR1_PAxCA;
    layer_cfg.BlendingFactor2 = LTDC_BLENDING_FACTOR2_PAxCA;
    layer_cfg.ImageWidth = XSIZE_PHYS;
    layer_cfg.ImageHeight = YSIZE_PHYS;
    HAL_LTDC_ConfigLayer(&hltdc, &layer_cfg, LayerIndex);  
    
    /* Enable LUT on demand */
    if (LCD_GetBitsPerPixelEx(LayerIndex) <= 8) 
    {
      /* Enable usage of LUT for all modes with <= 8bpp*/
      HAL_LTDC_EnableCLUT(&hltdc, LayerIndex);
    } 
  } 
}

/**
  * @brief  Initialize the LCD Controller.
  * @param  LayerIndex : layer Index.
  * @retval None
  */
static void LCD_LL_Init(void) 
{ 
  /* DeInit */
//	MX_LTDC_Init();
	
	//HAL_Delay(1000);	HAL_Delay(1000);
 // HAL_LTDC_DeInit(&hltdc);
  
  /* Set LCD Timings */
	hltdc.Init.HorizontalSync = HSW - 1;
  hltdc.Init.VerticalSync = VSW - 1;
  hltdc.Init.AccumulatedHBP = HSW + HBP - 1;
  hltdc.Init.AccumulatedVBP = VSW + VBP - 1;  
  hltdc.Init.AccumulatedActiveH = VSW + VBP +  YSIZE_PHYS - 1;
  hltdc.Init.AccumulatedActiveW =  HSW + HBP + XSIZE_PHYS - 1;
  hltdc.Init.TotalHeigh = VSW + VBP + YSIZE_PHYS + VFP - 1;
  hltdc.Init.TotalWidth = HSW + HBP + XSIZE_PHYS + HFP - 1;
  
  /* background value */
  hltdc.Init.Backcolor.Blue = 0;
  hltdc.Init.Backcolor.Green = 0;
  hltdc.Init.Backcolor.Red = 0;  
  
  /* Polarity */
  hltdc.Init.HSPolarity = LTDC_HSPOLARITY_AL;
  hltdc.Init.VSPolarity = LTDC_VSPOLARITY_AL; 
  hltdc.Init.DEPolarity = LTDC_DEPOLARITY_AL;  
  hltdc.Init.PCPolarity = LTDC_PCPOLARITY_IPC;
  hltdc.Instance = LTDC;
  
  HAL_LTDC_Init(&hltdc);
  HAL_LTDC_ProgramLineEvent(&hltdc, 0);
  
  /* Enable dithering */
  HAL_LTDC_EnableDither(&hltdc);
    
   /* Configure the DMA2D default mode */ 
  hdma2d.Init.Mode         = DMA2D_R2M;
  hdma2d.Init.ColorMode    = DMA2D_RGB565;
  hdma2d.Init.OutputOffset = 0x0;     

  hdma2d.Instance          = DMA2D; 

  if(HAL_DMA2D_Init(&hdma2d) != HAL_OK)
  {
    while (1);
  }
  
  /* Assert display enable LCD_DISP pin */
//  HAL_GPIO_WritePin(GPIOI, GPIO_PIN_12, GPIO_PIN_SET);

//  /* Assert backlight LCD_BL_CTRL pin */
//  HAL_GPIO_WritePin(GPIOK, GPIO_PIN_3, GPIO_PIN_SET);

}

static void DMA2D_AlphaBlending(LCD_COLOR * pColorFG, LCD_COLOR * pColorBG, LCD_COLOR * pColorDst, U32 NumItems)
{
  /* Invert alpha values */
  InvertAlpha(pColorFG, aBuffer_FG, NumItems);
  InvertAlpha(pColorBG, aBuffer_BG, NumItems);
  
  /* Use DMA2D for mixing */
  DMA2D_AlphaBlendingBulk(aBuffer_FG, aBuffer_BG, aBufferDMA2D, NumItems);
  
  /* Invert alpha values */
  InvertAlpha(aBufferDMA2D, pColorDst, NumItems);
}

/**
  * @brief  This routine is used by the STemWin color conversion routines to use DMA2D for
  *         color conversion. It converts the given index values to 32 bit colors.
  *         Because STemWin uses ABGR internally and 0x00 and 0xFF for opaque and fully
  *         transparent the color array needs to be converted after DMA2D has been used.
  * @param  pIndex 
  * @param  pColor
  * @param  NumItems
  * @param  SizeOfIndex
  * @param  PixelFormat
  * @retval None
  */
static void DMA2D_Index2ColorBulk(void * pIndex, LCD_COLOR * pColor, uint32_t NumItems, U8 SizeOfIndex, uint32_t PixelFormat)
{
  /* Use DMA2D for the conversion */
  DMA2D_ConvertColor(pIndex, aBufferDMA2D, PixelFormat, LTDC_PIXEL_FORMAT_ARGB8888, NumItems);
  
  /* Convert colors from ARGB to ABGR and invert alpha values */
  InvertAlpha_SwapRB(aBufferDMA2D, pColor, NumItems);
}

/**
  * @brief  This routine is used by the STemWin color conversion routines to use DMA2D for
  *         color conversion. It converts the given 32 bit color array to index values.
  *         Because STemWin uses ABGR internally and 0x00 and 0xFF for opaque and fully
  *         transparent the given color array needs to be converted before DMA2D can be used.
  * @param  pColor
  * @param  pIndex 
  * @param  NumItems
  * @param  SizeOfIndex
  * @param  PixelFormat
  * @retval None
  */
static void DMA2D_Color2IndexBulk(LCD_COLOR * pColor, void * pIndex, uint32_t NumItems, U8 SizeOfIndex, uint32_t PixelFormat) 
{
  /* Convert colors from ABGR to ARGB and invert alpha values */
  InvertAlpha_SwapRB(pColor, aBufferDMA2D, NumItems);
  
  /* Use DMA2D for the conversion */
  DMA2D_ConvertColor(aBufferDMA2D, pIndex, LTDC_PIXEL_FORMAT_ARGB8888, PixelFormat, NumItems);
}

/**
  * @brief  
  * @param  pFG 
  * @param  pBG
  * @param  pDst
  * @param  OffFG
  * @param  OffBG
  * @param  OffDest 
  * @param  xSize
  * @param  ySize
  * @param  Intens
  * @retval None
  */
static void LCD_MixColorsBulk(U32 * pFG, U32 * pBG, U32 * pDst, unsigned OffFG, unsigned OffBG, unsigned OffDest, unsigned xSize, unsigned ySize, U8 Intens)
{
  int32_t y;

  GUI_USE_PARA(OffFG);
  GUI_USE_PARA(OffDest);
  for (y = 0; y < ySize; y++) 
  {
    /* Invert alpha values */
    InvertAlpha(pFG, aBuffer_FG, xSize);
    InvertAlpha(pBG, aBuffer_BG, xSize);

    DMA2D_MixColorsBulk(aBuffer_FG, aBuffer_BG, aBufferDMA2D, Intens, xSize);

    InvertAlpha(aBufferDMA2D, pDst, xSize);
    pFG  += xSize + OffFG;
    pBG  += xSize + OffBG;
    pDst += xSize + OffDest;
  }
}

/**
  * @brief  
  * @param  pSrc
  * @param  pDst
  * @param  OffSrc
  * @param  OffDst
  * @param  PixelFormatDst
  * @param  xSize
  * @param  ySize
  * @retval None
  */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
