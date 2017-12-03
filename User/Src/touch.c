#include "touch.h"


TouchStructure touchInfo;	//½á¹¹ÌåÉùÃ÷


void IIC_Touch_Delay(uint16_t a)
{
	int i;
	while (a --)
	{
		for (i = 0; i < 5; i++);
	}
}
//	º¯Êı£º³õÊ¼»¯IICµÄGPIO¿Ú
//
//PB2	:SDA
//PC13:SCL

void IIC_Touch_GPIO_Config (void)
{
  GPIO_InitTypeDef GPIO_InitStruct;

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);

  /*Configure GPIO pin : PD12 */
  GPIO_InitStruct.Pin = GPIO_PIN_2;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : PG3 PG7 */
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
	
}
#define LIB 1
#if LIB
//	º¯Êı£ºÅäÖÃIICµÄÊı¾İ½ÅÎªÊä³öÄ£Ê½
//
void IIC_Touch_SDA_Out(void)
{
//	HAL_GPIO_DeInit(GPIOB,GPIO_PIN_2);
	GPIO_InitTypeDef GPIO_InitStruct;
  /*Configure GPIO pin : PD12 */
  GPIO_InitStruct.Pin = GPIO_PIN_2;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

//	º¯Êı£ºÅäÖÃIICµÄÊı¾İ½ÅÎªÊäÈëÄ£Ê½
//
void IIC_Touch_SDA_In(void)
{
//	HAL_GPIO_DeInit(GPIOB,GPIO_PIN_2);
	GPIO_InitTypeDef GPIO_InitStruct;
  GPIO_InitStruct.Pin = GPIO_PIN_2;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

/*
GPIOB->MODER&=~(3<<(2*2));
GPIOB->MODER|=1<<2*2;//ê?3?
GPIOB->MODER|=0<<2*2;//ê?è?
*/
#else
void IIC_Touch_SDA_Out(void)
{
GPIOB->MODER&=~(3<<(2*2));
GPIOB->MODER|=1<<2*2;//ê?3?
}

//	o¯êy?o????IICµ?êy?Y???aê?è???ê?
//
void IIC_Touch_SDA_In(void)
{
GPIOB->MODER&=~(3<<(2*2));
GPIOB->MODER|=0<<2*2;//ê?è?
}

#endif
// IO¿Ú²Ù×÷
/*#define SCL(a) HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, a)

#define SDA(a) HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, a)
*/
void SCL(uint8_t mode)
{
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, mode);
}

void SDA(uint8_t mode)
{
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, mode);
}

//	º¯Êı£ºIICÆğÊ¼ĞÅºÅ
//
void IIC_Touch_Start(void)
{
	IIC_Touch_SDA_Out();
	
	SDA(1);
	SCL(1);
	IIC_Touch_Delay( Touch_DelayVaule );
	
	SDA(0);
	IIC_Touch_Delay( Touch_DelayVaule );
	SCL(0);
	IIC_Touch_Delay( Touch_DelayVaule );
}

//	º¯Êı£ºIICÍ£Ö¹ĞÅºÅ
//
void IIC_Touch_Stop(void)
{
    SCL(0);
    IIC_Touch_Delay( Touch_DelayVaule );
    SDA(0);
    IIC_Touch_Delay( Touch_DelayVaule );
    SCL(1);
    IIC_Touch_Delay( Touch_DelayVaule );
    SDA(1);
    IIC_Touch_Delay( Touch_DelayVaule );
}

//	º¯Êı£ºIICÓ¦´ğĞÅºÅ
//
void IIC_Touch_Response(void)
{
	IIC_Touch_SDA_Out();

	SDA(0);
	IIC_Touch_Delay( Touch_DelayVaule );	
	SCL(1);
	IIC_Touch_Delay( Touch_DelayVaule );
	SCL(0);
	IIC_Touch_Delay( Touch_DelayVaule );
}

//	º¯Êı£ºIIC·ÇÓ¦´ğĞÅºÅ
//
void IIC_Touch_NoResponse(void)
{
	IIC_Touch_SDA_Out();
	
	SCL(0);	
	IIC_Touch_Delay( Touch_DelayVaule );
	SDA(1);
	IIC_Touch_Delay( Touch_DelayVaule );
	SCL(1);
	IIC_Touch_Delay( Touch_DelayVaule );
	SCL(0);
	IIC_Touch_Delay( Touch_DelayVaule );
}

//	º¯Êı£ºµÈ´ıÉè±¸·¢³ö»ØÓ¦ĞÍºÅ
//
uint8_t IIC_Touch_WaitResponse(void)
{

	SCL(0);
	IIC_Touch_Delay( Touch_DelayVaule );
	SDA(1);
	IIC_Touch_Delay( Touch_DelayVaule );
	SCL(1);

	IIC_Touch_SDA_In();	//ÅäÖÃÎªÊäÈëÄ£Ê½
	IIC_Touch_Delay( Touch_DelayVaule );
	
	SCL(0);	
	if( HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_2) != 0) //ÅĞ¶ÏÉè±¸ÊÇ·ñÓĞ×ö³öÏìÓ¦
	{		
		return (Response_ERR);
	}
	else
	{
		return (Response_OK);
	}

}

// º¯Êı£ºIICĞ´×Ö½Ú
//	²ÎÊı£ºIIC_Data - ÒªĞ´ÈëµÄ8Î»Êı¾İ
//	·µ»Ø£ºÉè±¸ÓĞÏìÓ¦Ôò·µ»Ø 1£¬·ñÔòÎª0
//
uint8_t IIC_Touch_WriteByte(uint8_t IIC_Data)
{
	uint8_t i;

	IIC_Touch_SDA_Out(); //Êı¾İ½ÅÎªÊä³öÄ£Ê½
	
	for (i = 0; i < 8; i++)
	{
		SDA(IIC_Data & 0x80);
		
		IIC_Touch_Delay( Touch_DelayVaule );
		SCL(1);
		IIC_Touch_Delay( Touch_DelayVaule );
		SCL(0);		
		
		IIC_Data <<= 1;
	}

	return (IIC_Touch_WaitResponse()); //µÈ´ıÉè±¸ÏìÓ¦
}

// º¯Êı£ºIIC¶Á×Ö½Ú
//	²ÎÊı£ºResponseMode - Ó¦´ğÄ£Ê½Ñ¡Ôñ
//       ResponseMode = 1 Ê±£¬CPU·¢³öÏìÓ¦ĞÅºÅ£»Îª 0 Ê±£¬CPU·¢³ö·ÇÓ¦´ğĞÅºÅ
//	·µ»Ø£º¶Á³öµÄÊı¾İ
//
uint8_t IIC_Touch_ReadByte(uint8_t ResponseMode)
{
	uint8_t IIC_Data;
	uint8_t i;
	
	SDA(1);
	SCL(0);

	IIC_Touch_SDA_In(); //ÊäÈëÄ£Ê½
	
	//¶ÁÒ»×Ö½ÚÊı¾İ
	for (i = 0; i < 8; i++)
	{
		IIC_Data <<= 1;
		
		SCL(1);
		IIC_Touch_Delay( Touch_DelayVaule );

		IIC_Data |= (HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_2) & 0x01);
		
		SCL(0);
		IIC_Touch_Delay( Touch_DelayVaule );
	}

	//	×ö³öÏàÓ¦ĞÅºÅ
	if (ResponseMode)
	{
		IIC_Touch_Response();
	}
	else
	{
		IIC_Touch_NoResponse();
	}
	
	return (IIC_Data); 
}



/*---------------------- GT9XXÏà¹Øº¯Êı ------------------------*/

// º¯Êı£ºGT9XX Ğ´²Ù×÷
//	²ÎÊı£ºaddr - Òª²Ù×÷µÄ¼Ä´æÆ÷
//			
uint8_t GT9XX_WriteHandle (uint16_t addr)
{
	uint8_t status;

	IIC_Touch_Start();
	if( IIC_Touch_WriteByte(GT9XX_IIC_WADDR) == Response_OK ) //Ğ´Êı¾İÖ¸Áî
	{
		if( IIC_Touch_WriteByte((uint8_t)(addr >> 8)) == Response_OK ) //Ğ´Èë16µØÖ·
		{
			if( IIC_Touch_WriteByte((uint8_t)(addr)) != Response_OK )
			{
				status = ERROR;
			}			
		}
	}
	status = SUCCESS;
	return status;	
}

// º¯Êı£ºGT9XX Ğ´Êı¾İ
//	²ÎÊı£ºaddr - ÒªĞ´Êı¾İµÄµØÖ·
//			value - Ğ´ÈëµÄÊı¾İ
//
uint8_t GT9XX_WriteData (uint16_t addr,uint8_t value)
{
	uint8_t status;
	
	IIC_Touch_Start(); //Æô¶¯IICÍ¨Ñ¶

	if( GT9XX_WriteHandle(addr) == SUCCESS)	//Ğ´ÈëÒª²Ù×÷µÄ¼Ä´æÆ÷
	{
		if (IIC_Touch_WriteByte(value) != Response_OK) //Ğ´Êı¾İ
		{
			status = ERROR;						
		}
	}
	
	IIC_Touch_Stop(); //Í£Ö¹Í¨Ñ¶
	
	status = SUCCESS;
	return status;
}

// º¯Êı£ºGT9XX ¶ÁÊı¾İ
//	²ÎÊı£ºaddr - Òª¶ÁÊı¾İµÄµØÖ·
//			num - ¶Á³öµÄ×Ö½ÚÊı
//			*value - ÓÃÓÚ»ñÈ¡´æ´¢Êı¾İµÄÊ×µØÖ·
//
uint8_t GT9XX_ReadData (uint16_t addr, uint8_t cnt, uint8_t *value)
{
	uint8_t status;
	uint8_t i;

	status = ERROR;
	IIC_Touch_Start();

	if( GT9XX_WriteHandle(addr) == SUCCESS) //Ğ´ÈëÒª²Ù×÷µÄ¼Ä´æÆ÷
	{
		IIC_Touch_Start(); //ÖØĞÂÆô¶¯IICÍ¨Ñ¶

		if (IIC_Touch_WriteByte(GT9XX_IIC_RADDR) == Response_OK)
		{	
			for(i = 0 ; i < cnt; i++)
			{
				if (i == (cnt - 1))
				{
					value[i] = IIC_Touch_ReadByte(0);//¶Áµ½×îºóÒ»¸öÊı¾İÊ±·¢ËÍ ·ÇÓ¦´ğĞÅºÅ
				}
				else
				{
					value[i] = IIC_Touch_ReadByte(1);
				}

			}					
			IIC_Touch_Stop();
			status = SUCCESS;
		}
	}
	
	IIC_Touch_Stop();
	return (status);	
}

// º¯Êı: ´¥ÃşÆÁ³õÊ¼»¯
//
void Touch_Init(void)
{
//	IIC_Touch_GPIO_Config(); //³õÊ¼»¯ÓÃÓÚÍ¨ĞÅµÄIICÒı½Å
	GPIO_InitTypeDef GPIO_InitStruct;

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);

  /*Configure GPIO pin : PD12 */
  GPIO_InitStruct.Pin = GPIO_PIN_2;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : PG3 PG7 */
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
}

// º¯Êı£º´¥ÃşÉ¨Ãè
//	ËµÃ÷£ºÔÚ³ÌĞòÀïÖÜÆÚĞÔµÄµ÷ÓÃ¸Ãº¯Êı£¬ÓÃÒÔ¼ì²â´¥Ãş²Ù×÷
//
uint8_t	Touch_Scan(void)
{
 	uint8_t  touchData[2 + 8 * TOUCH_MAX ]; //ÓÃÓÚ´æ´¢´¥ÃşÊı¾İ
	memset(touchData,0,10);
	GT9XX_ReadData (GT9XX_READ_ADDR,2 + 8 * TOUCH_MAX ,touchData);	//¶ÁÊı¾İ
	GT9XX_WriteData (GT9XX_READ_ADDR,0);	//	Çå³ı´¥ÃşĞ¾Æ¬µÄ¼Ä´æÆ÷±êÖ¾Î»
	touchInfo.num = touchData[0] & 0x0f;	//È¡µ±Ç°µÄ´¥ÃşµãÊı
	
	if ( (touchInfo.num ) ) //µ±´¥ÃşÊıÔÚ 1-5 Ö®¼äÊ±>= 1) && (touchInfo.num <=5
	{
		// È¡ÏàÓ¦µÄ´¥Ãş×ø±ê
	if(touchData[5]<200)
						touchInfo.y[0] = ((touchData[5]<<8) | touchData[4])*0.81+1;

		if(touchData[3]<200)
				touchInfo.x[0] = ((touchData[3]<<8) | touchData[2])*0.78+1;	

		return	SUCCESS ;	
	}

	else                       
	{	
		
		touchInfo.x[0] = 0;
		touchInfo.y[0] = 0;

		return	ERROR ;		
	}
	
}

void	GUI_TouchScan(void)
{
	GUI_PID_STATE State;
	Touch_Scan(); //´¥ÃşÉ¨Ãè
	State.x = touchInfo.x[0];
	State.y = touchInfo.y[0];
	if (touchInfo.x[0]) {
	State.Pressed = 1;
	} else {
	State.Pressed = 0;
	}
	GUI_TOUCH_StoreStateEx(&State);
}
