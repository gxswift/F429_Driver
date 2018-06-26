#include "touch.h"


#include "funopts.h"


TouchStructure touchInfo;	//½á¹¹ÌåÉùÃ÷

static void T_Delay(uint32_t a)
{
	int i;
	while (a --)
	{
		for (i = 0; i < 5; i++);
	}
}
#define Touch_INT_CLK     RCC_AHB1Periph_GPIOA			// INT
#define Touch_INT_PORT    GPIOA                   		
#define Touch_INT_PIN     GPIO_Pin_15             		

#define Touch_RST_CLK     RCC_AHB1Periph_GPIOI			// RST
#define Touch_RST_PORT    GPIOI                   		
#define Touch_RST_PIN     GPIO_Pin_11            		

//-----------------------------------------------------------------
//ÍÆÍìÊä³ö ÇĞ»»·½Ïò
//º¯Êı£º³õÊ¼»¯IICµÄGPIO¿Ú
//PB2	:SDA
//PC13:SCL
// º¯Êı: ´¥ÃşÆÁ³õÊ¼»¯
//
void Touch_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);

  GPIO_InitStruct.Pin = GPIO_PIN_2;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
}
#define LIB 1
#if LIB
//	º¯Êı£ºÅäÖÃIICµÄÊı¾İ½ÅÎªÊä³öÄ£Ê½

void IIC_Touch_SDA_Out(void)
{
//	HAL_GPIO_DeInit(GPIOB,GPIO_PIN_2);
	GPIO_InitTypeDef GPIO_InitStruct;
  GPIO_InitStruct.Pin = GPIO_PIN_2;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

//	º¯Êı£ºÅäÖÃIICµÄÊı¾İ½ÅÎªÊäÈëÄ£Ê½
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
GPIOB->MODER|=1<<2*2;//output
}

void IIC_Touch_SDA_In(void)
{
GPIOB->MODER&=~(3<<(2*2));
GPIOB->MODER|=0<<2*2;//input
}
#endif
//-----------------------------------------------------------------
// IO¿Ú²Ù×÷
/*
#define SCL(a) HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, a)
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
void IIC_Touch_Start(void)
{
	//IIC_Touch_SDA_Out();
	SDA(1);
	SCL(1);
	T_Delay( Touch_DelayVaule );
	
	SDA(0);
	T_Delay( Touch_DelayVaule );
	SCL(0);
	T_Delay( Touch_DelayVaule );
}

//	º¯Êı£ºIICÍ£Ö¹ĞÅºÅ
void IIC_Touch_Stop(void)
{
    SCL(0);
    T_Delay( Touch_DelayVaule );
    SDA(0);
    T_Delay( Touch_DelayVaule );
    SCL(1);
    T_Delay( Touch_DelayVaule );
    SDA(1);
    T_Delay( Touch_DelayVaule );
}

//	º¯Êı£ºIICÓ¦´ğĞÅºÅ
void IIC_Touch_Response(void)
{
	//IIC_Touch_SDA_Out();
	SDA(0);
	T_Delay( Touch_DelayVaule );	
	SCL(1);
	T_Delay( Touch_DelayVaule );
	SCL(0);
	T_Delay( Touch_DelayVaule );
}

//	º¯Êı£ºIIC·ÇÓ¦´ğĞÅºÅ
void IIC_Touch_NoResponse(void)
{
//	IIC_Touch_SDA_Out();
	SCL(0);	
	T_Delay( Touch_DelayVaule );
	SDA(1);
	T_Delay( Touch_DelayVaule );
	SCL(1);
	T_Delay( Touch_DelayVaule );
	SCL(0);
	T_Delay( Touch_DelayVaule );
}

//	º¯Êı£ºµÈ´ıÉè±¸·¢³ö»ØÓ¦ĞÍºÅ
uint8_t IIC_Touch_WaitResponse(void)
{

	SCL(0);
	T_Delay( Touch_DelayVaule );
	SDA(1);
	T_Delay( Touch_DelayVaule );
	SCL(1);
//	IIC_Touch_SDA_In();	//ÅäÖÃÎªÊäÈëÄ£Ê½
	T_Delay( Touch_DelayVaule );
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
	//IIC_Touch_SDA_Out(); //Êı¾İ½ÅÎªÊä³öÄ£Ê½
	for (i = 0; i < 8; i++)
	{
		SDA(IIC_Data & 0x80);
		T_Delay( Touch_DelayVaule );
		SCL(1);
		T_Delay( Touch_DelayVaule );
		SCL(0);		
		IIC_Data <<= 1;
	}
	return (IIC_Touch_WaitResponse()); //µÈ´ıÉè±¸ÏìÓ¦
}

// º¯Êı£ºIIC¶Á×Ö½Ú
//	²ÎÊı£ºResponseMode - Ó¦´ğÄ£Ê½Ñ¡Ôñ
//       ResponseMode = 1 Ê±£¬CPU·¢³öÏìÓ¦ĞÅºÅ£»Îª 0 Ê±£¬CPU·¢³ö·ÇÓ¦´ğĞÅºÅ
//	·µ»Ø£º¶Á³öµÄÊı¾İ
uint8_t IIC_Touch_ReadByte(uint8_t ResponseMode)
{
	uint8_t IIC_Data;
	uint8_t i;
	
	SDA(1);
	SCL(0);
	//IIC_Touch_SDA_In(); //ÊäÈëÄ£Ê½
	//¶ÁÒ»×Ö½ÚÊı¾İ
	for (i = 0; i < 8; i++)
	{
		IIC_Data <<= 1;
		SCL(1);
		T_Delay( Touch_DelayVaule );
		IIC_Data |= (HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_2) & 0x01);
		SCL(0);
		T_Delay( Touch_DelayVaule );
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
#if LCD7
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

// º¯Êı£º´¥ÃşÉ¨Ãè

uint8_t	Touch_Scan(void)
{
 	uint8_t  touchData[2 + 8 * TOUCH_MAX ]; //ÓÃÓÚ´æ´¢´¥ÃşÊı¾İ
	memset(touchData,0,10);
	GT9XX_ReadData (GT9XX_READ_ADDR,2 + 8 * TOUCH_MAX ,touchData);	//¶ÁÊı¾İ
	GT9XX_WriteData (GT9XX_READ_ADDR,0);	//	Çå³ı´¥ÃşĞ¾Æ¬µÄ¼Ä´æÆ÷±êÖ¾Î»
	touchInfo.num = touchData[0] & 0x0f;	//È¡µ±Ç°µÄ´¥ÃşµãÊı
	
	if ( (touchInfo.num ) &&touchData[5]<200 && touchData[3]<200 ) //µ±´¥ÃşÊıÔÚ 1-5 Ö®¼äÊ±>= 1) && (touchInfo.num <=5
	{
		// È¡ÏàÓ¦µÄ´¥Ãş×ø±ê
		touchInfo.y[0] = ((touchData[5]<<8) | touchData[4])*0.81+1;
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
#else

// ´¥Ãş²ÎÊıÅäÖÃÊı×é£¬ÔÚº¯Êı GT9XX_SendCfg() Àïµ÷ÓÃ£¬ÓÃÓÚÅäÖÃ´¥ÃşICµÄÏà¹Ø²ÎÊı
//	ÓÉÓÚGT9147¿ÉÒÔ¹Ì»¯±£´æÕâĞ©²ÎÊı£¬ËùÒÔÓÃ»§Ò»°ãÇé¿öÏÂÎŞĞèÔÙ½øĞĞÅäÖÃ
//	ÏêÏ¸µÄ¼Ä´æÆ÷¹¦ÄÜÇë²Î¿¼¡¶GT9147±à³ÌÖ¸ÄÏ¡·
//
const uint8_t GT9XX_CFG_DATA[] = 	
{                              	 
	0XAA,			// ¼Ä´æÆ÷µØÖ·£º0x8047£¬¹¦ÄÜ£ºÅäÖÃ°æ±¾ºÅ
	
	0XE0,0X01,	// ¼Ä´æÆ÷µØÖ·£º0x8048~0x8049£¬¹¦ÄÜ£ºX×ø±ê×î´óÖµ£¬µÍÎ»ÔÚÇ°
	0X10,0X01,	// ¼Ä´æÆ÷µØÖ·£º0x804A~0x804B£¬¹¦ÄÜ£ºY×ø±ê×î´óÖµ£¬µÍÎ»ÔÚÇ°
	
	0X05,			// ¼Ä´æÆ÷µØÖ·£º0x804C£¬¹¦ÄÜ£ºÉèÖÃ×î´ó´¥ÃşµãÊı£¬1~5µã
	0X0E,			// ¼Ä´æÆ÷µØÖ·£º0x804D£¬¹¦ÄÜ£ºÉèÖÃINT´¥·¢·½Ê½¡¢XY×ø±ê½»»»
	0X00,			// ¸Ã¼Ä´æÆ÷ÎŞĞèÅäÖÃ
	0X88,			// ¼Ä´æÆ÷µØÖ·£º0x804F£¬¹¦ÄÜ£º°´ÏÂ»òËÉ¿ªÈ¥¶¶´ÎÊı
	0X0B,			// ¼Ä´æÆ÷µØÖ·£º0x8050£¬¹¦ÄÜ£ºÔ­Ê¼×ø±ê´°¿ÚÂË²¨Öµ
	0X80,0X08,0X50,0X3C,0X0F,0X00,0X00,0X00,0XFF,0X67,		// 0X8051 ~ 0X805A
	0X50,0X00,0X00,0X18,0X1A,0X1E,0X14,0X89,0X28,0X0A,		// 0X805B ~ 0X8064
	0X30,0X2E,0XBB,0X0A,0X03,0X00,0X00,0X02,0X33,0X1D,		// 0X8065 ~ 0X806E
	0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X32,0X00,0X00,		// 0X806F ~ 0X8078
	0X2A,0X1C,0X5A,0X94,0XC5,0X02,0X07,0X00,0X00,0X00,		// 0X8079 ~ 0X8082
	0XB5,0X1F,0X00,0X90,0X28,0X00,0X77,0X32,0X00,0X62,		// 0X8083 ~ 0X808C
	0X3F,0X00,0X52,0X50,0X00,0X52,0X00,0X00,0X00,0X00,		// 0X808D ~ 0X8096
	0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,		// 0X8097 ~ 0X80A0
	0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X0F,		// 0X80A1 ~ 0X80AA
	0X0F,0X03,0X06,0X10,0X42,0XF8,0X0F,0X14,0X00,0X00,		// 0X80AB ~ 0X80B4
	0X00,0X00,

/******************************************************************************************
*	¼Ä´æÆ÷µØÖ·:	0x80B7~0X80C4
*	¹¦ÄÜËµÃ÷  :	ĞŞ¸Ä¸ĞÓ¦Í¨µÀ¶ÔÓ¦µÄĞ¾Æ¬Í¨µÀºÅ£¬¿ÉÒÔ¸Ä±ä´¥ÃşÃæ°åµÄ´¹Ö±É¨Ãè·½Ïò
*******************************************************************************************/

	0X08,0X0A,0X0C,0X0E,0X10,0X12,0X14,0X16,0X18,0X1A,		// É¨Ãè·½Ïò´Ó ÉÏ µ½ ÏÂ
//	0X1A,0X18,0X16,0X14,0X12,0X10,0X0E,0X0C,0X0A,0X08,		// É¨Ãè·½Ïò´Ó ÏÂ µ½ ÉÏ	
	0X00,0X00,0X00,0X00,												// Î´Ê¹ÓÃµÄ¸ĞÓ¦Í¨µÀ£¬ÎŞĞè¸ü¸ÄÉèÖÃ
	
/******************************************************************************************/

	0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,		// ÎŞ×÷ÓÃ¼Ä´æÆ÷£¬ÎŞĞèÅäÖÃ
	0X00,0X00,0X00,0X00,0x00,0x00, 								// ÎŞ×÷ÓÃ¼Ä´æÆ÷£¬ÎŞĞèÅäÖÃ
	
/*******************************************************************************************
*	¼Ä´æÆ÷µØÖ·:	0x80D5~0X80EE
*	¹¦ÄÜËµÃ÷  :	ĞŞ¸ÄÇı¶¯Í¨µÀ¶ÔÓ¦µÄĞ¾Æ¬Í¨µÀºÅ£¬¿ÉÒÔ¸Ä±ä´¥ÃşÃæ°åµÄË®Æ½É¨Ãè·½Ïò
********************************************************************************************/

	0x00,0x02,0x04,0x05,0x06,0x08,0x0a,0x0c, 				// É¨Ãè·½Ïò´Ó ×ó µ½ ÓÒ
	0x0e,0x1d,0x1e,0x1f,0x20,0x22,0x24,0x28,0x29,      

// 0X29,0X28,0X24,0X22,0X20,0X1F,0X1E,0X1D,				// É¨Ãè·½Ïò´Ó ÓÒ µ½ ×ó
//	0X0E,0X0C,0X0A,0X08,0X06,0X05,0X04,0X02,0X00,	
	
	0xff, 	// Î´Ê¹ÓÃµÄÇı¶¯Í¨µÀ£¬ÎŞĞè¸ü¸ÄÉèÖÃ
	
/*******************************************************************************************/

	0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,		// Í¨µÀµ÷ÕûÏµÊı¼Ä´æÆ÷£¬ÎŞĞèĞŞ¸Ä
	0X00,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,		// Í¨µÀµ÷ÕûÏµÊı¼Ä´æÆ÷£¬ÎŞĞèĞŞ¸Ä
	0XFF,0XFF,0XFF,0XFF,												// Í¨µÀµ÷ÕûÏµÊı¼Ä´æÆ÷£¬ÎŞĞèĞŞ¸Ä
};


/*****************************************************************************************
*	º¯ Êı Ãû:	GT9XX_Reset
*	Èë¿Ú²ÎÊı:	ÎŞ
*	·µ »Ø Öµ:	ÎŞ
*	º¯Êı¹¦ÄÜ:	¸´Î»GT9147
*	Ëµ    Ã÷:¸´Î»GT9147£¬²¢½«Ğ¾Æ¬µÄIICµØÖ·ÅäÖÃÎª0xBA/0xBB
******************************************************************************************/

void GT9XX_Reset(void)
{
	Touch_INT_Out();	//	½«INTÒı½ÅÅäÖÃÎªÊä³ö
	
	// ³õÊ¼»¯Òı½Å×´Ì¬
	GPIO_ResetBits(Touch_INT_PORT,Touch_INT_PIN); 	// ½«INTÀ­µÍ
	GPIO_SetBits  (Touch_RST_PORT,Touch_RST_PIN);	// ½«RSTÀ­¸ß
	T_Delay(10000);	
	
	// ¿ªÊ¼Ö´ĞĞ¸´Î»
	//	INTÒı½Å±£³ÖµÍµçÆ½²»±ä£¬½«Æ÷¼şµØÖ·ÉèÖÃÎª0XBA/0XBB
	GPIO_ResetBits(Touch_RST_PORT,Touch_RST_PIN);			// À­µÍ¸´Î»Òı½Å£¬´ËÊ±Ğ¾Æ¬Ö´ĞĞ¸´Î»
	T_Delay(350000);										// ÑÓÊ±
	GPIO_SetBits  (Touch_RST_PORT,Touch_RST_PIN);			// À­¸ß¸´Î»Òı½Å£¬¸´Î»½áÊø
	Touch_IIC_Delay(350000);										// ÑÓÊ±
	Touch_INT_In();													// INTÒı½Å×ªÎª¸¡¿ÕÊäÈë
	Touch_IIC_Delay(350000);										// ÑÓÊ±
}

/*****************************************************************************************
*	º¯ Êı Ãû:	GT9XX_WriteHandle
*	Èë¿Ú²ÎÊı:	addr - Òª²Ù×÷µÄ¼Ä´æÆ÷
*	·µ »Ø Öµ:	SUCCESS - ²Ù×÷³É¹¦
*				ERROR	  - ²Ù×÷Ê§°Ü
*	º¯Êı¹¦ÄÜ:	GT9XX Ğ´²Ù×÷
*	Ëµ    Ã÷:¶ÔÖ¸¶¨µÄ¼Ä´æÆ÷Ö´ĞĞĞ´²Ù×÷
******************************************************************************************/

uint8_t GT9XX_WriteHandle (uint16_t addr)
{
	uint8_t status;	// ×´Ì¬±êÖ¾Î»

	Touch_IIC_Start();	// Æô¶¯IICÍ¨ĞÅ
	
	if( Touch_IIC_WriteByte(GT9XX_IIC_WADDR) == 1 ) //Ğ´Êı¾İÖ¸Áî
	{
		if( Touch_IIC_WriteByte((uint8_t)(addr >> 8)) == 1 ) //Ğ´Èë16Î»µØÖ·
		{
			if( Touch_IIC_WriteByte((uint8_t)(addr)) != 1 )
			{
				status = ERROR;	// ²Ù×÷Ê§°Ü
			}			
		}
	}
	status = SUCCESS;	// ²Ù×÷³É¹¦
	return status;	
}

/*****************************************************************************************
*	º¯ Êı Ãû:	GT9XX_WriteData
*	Èë¿Ú²ÎÊı:	addr - ÒªĞ´ÈëµÄ¼Ä´æÆ÷
*				value - ÒªĞ´ÈëµÄÊı¾İ
*	·µ »Ø Öµ:	SUCCESS - ²Ù×÷³É¹¦
*				ERROR	  - ²Ù×÷Ê§°Ü
*	º¯Êı¹¦ÄÜ:	GT9XX Ğ´Ò»×Ö½ÚÊı¾İ
*	Ëµ    Ã÷:¶ÔÖ¸¶¨µÄ¼Ä´æÆ÷Ğ´ÈëÒ»×Ö½ÚÊı¾İ
******************************************************************************************/

uint8_t GT9XX_WriteData (uint16_t addr,uint8_t value)
{
	uint8_t status;
	
	Touch_IIC_Start(); //Æô¶¯IICÍ¨Ñ¶

	if( GT9XX_WriteHandle(addr) == SUCCESS)	//Ğ´ÈëÒª²Ù×÷µÄ¼Ä´æÆ÷
	{
		if (Touch_IIC_WriteByte(value) != 1) //Ğ´Êı¾İ
		{
			status = ERROR;	// Ğ´ÈëÊ§°Ü					
		}
	}	
	Touch_IIC_Stop(); // Í£Ö¹Í¨Ñ¶
	
	status = SUCCESS;	// Ğ´Èë³É¹¦
	return status;
}

/*****************************************************************************************
*	º¯ Êı Ãû:	GT9XX_WriteReg
*	Èë¿Ú²ÎÊı:	addr - ÒªĞ´ÈëµÄ¼Ä´æÆ÷ÇøÓòÊ×µØÖ·
*				cnt  - Êı¾İ³¤¶È
*				value - ÒªĞ´ÈëµÄÊı¾İÇø
*	·µ »Ø Öµ:	SUCCESS - ²Ù×÷³É¹¦
*				ERROR	  - ²Ù×÷Ê§°Ü
*	º¯Êı¹¦ÄÜ:	GT9XX Ğ´¼Ä´æÆ÷
*	Ëµ    Ã÷:ÍùĞ¾Æ¬µÄ¼Ä´æÆ÷ÇøĞ´ÈëÖ¸¶¨³¤¶ÈµÄÊı¾İ
******************************************************************************************/

uint8_t GT9XX_WriteReg (uint16_t addr, uint8_t cnt, uint8_t *value)
{
	uint8_t status = 0;
	uint8_t i = 0;

	Touch_IIC_Start();	// Æô¶¯IICÍ¨ĞÅ

	if( GT9XX_WriteHandle(addr) == SUCCESS) //Ğ´ÈëÒª²Ù×÷µÄ¼Ä´æÆ÷
	{
		for(i = 0 ; i < cnt; i++)		// ¼ÆÊı
		{
			Touch_IIC_WriteByte(value[i]);	// Ğ´ÈëÊı¾İ
		}					
		Touch_IIC_Stop();		// Í£Ö¹IICÍ¨ĞÅ
		status = SUCCESS;		// Ğ´Èë³É¹¦
	}
	else
	{
		Touch_IIC_Stop();		// Í£Ö¹IICÍ¨ĞÅ
		status = ERROR;		// Ğ´ÈëÊ§°Ü
	}

	return status;	
}

/*****************************************************************************************
*	º¯ Êı Ãû:	GT9XX_ReadReg
*	Èë¿Ú²ÎÊı:	addr - Òª¶ÁÈ¡µÄ¼Ä´æÆ÷ÇøÓòÊ×µØÖ·
*				cnt  - Êı¾İ³¤¶È
*				value - Òª¶ÁÈ¡µÄÊı¾İÇø
*	·µ »Ø Öµ:	SUCCESS - ²Ù×÷³É¹¦
*				ERROR	  - ²Ù×÷Ê§°Ü
*	º¯Êı¹¦ÄÜ:	GT9XX ¶Á¼Ä´æÆ÷
*	Ëµ    Ã÷:´ÓĞ¾Æ¬µÄ¼Ä´æÆ÷Çø¶ÁÈ¡Ö¸¶¨³¤¶ÈµÄÊı¾İ
******************************************************************************************/

uint8_t GT9XX_ReadReg (uint16_t addr, uint8_t cnt, uint8_t *value)
{
	uint8_t status = 0;
	uint8_t i = 0;

	status = ERROR;
	Touch_IIC_Start();	// Æô¶¯IICÍ¨ĞÅ

	if( GT9XX_WriteHandle(addr) == SUCCESS) 	//	Ğ´ÈëÒª²Ù×÷µÄ¼Ä´æÆ÷
	{
		Touch_IIC_Start(); // ÖØĞÂÆô¶¯IICÍ¨Ñ¶

		if (Touch_IIC_WriteByte(GT9XX_IIC_RADDR) == 1)	// ·¢ËÍ¶ÁÃüÁî
		{	
			for(i = 0 ; i < cnt; i++)	// ¼ÆÊı
			{
				if (i == (cnt - 1))
				{
					value[i] = Touch_IIC_ReadByte(0);	//	¶Áµ½×îºóÒ»¸öÊı¾İÊ±·¢ËÍ ·ÇÓ¦´ğĞÅºÅ
				}
				else
				{
					value[i] = Touch_IIC_ReadByte(1);	// ·¢ËÍÓ¦´ğĞÅºÅ
				}
			}					
			Touch_IIC_Stop();	// Í£Ö¹IICÍ¨ĞÅ
			status = SUCCESS;
		}
	}
	Touch_IIC_Stop();
	return (status);	
}

/*****************************************************************************************
*	º¯ Êı Ãû:	GT9XX_SendCfg
*	Èë¿Ú²ÎÊı:	ÎŞ
*	·µ »Ø Öµ:ÎŞ
*	º¯Êı¹¦ÄÜ:	·¢ËÍGT9147ÅäÖÃ²ÎÊı
*	Ëµ    Ã÷:ÓÉÓÚGT9147¿ÉÒÔµôµç±£´æÅäÖÃ²ÎÊı£¬²¢ÇÒ³ö³§Ê±ÒÑÅäÖÃºÃ£¬Ò»°ãÇé¿öÏÂÓÃ»§ÎŞĞè¸ü¸Ä£¬
*				ÓÃ»§ĞŞ¸Ä²ÎÊıÖ®ºó£¬ĞèÒª½«¸Ãº¯ÊıÆÁ±Îµô£¬²»È»Æµ·±µÄĞ´Èë»á½«´¥ÃşĞ¾Æ¬µÄFlashĞ´»µ
******************************************************************************************/

void GT9XX_SendCfg(void)
{
	uint8_t GT9XX_Check[2];
	uint8_t i=0;
	
	GT9XX_Check[1] = 1;		// ÅäÖÃ¸üĞÂ±êÖ¾
	
	for(i=0;i<sizeof(GT9XX_CFG_DATA);i++)
	{
		GT9XX_Check[0] += GT9XX_CFG_DATA[i];	//¼ÆËãĞ£ÑéºÍ
	}
   GT9XX_Check [0] = (~GT9XX_Check[0])+1;		
	
	GT9XX_WriteReg(0X8047,sizeof(GT9XX_CFG_DATA),(uint8_t*)GT9XX_CFG_DATA);	//	·¢ËÍ¼Ä´æÆ÷ÅäÖÃ
	GT9XX_WriteReg(0X80FF,2,GT9XX_Check); // Ğ´ÈëĞ£ÑéºÍ£¬²¢¸üĞÂÅäÖÃ
} 

/*****************************************************************************************
*	º¯ Êı Ãû:	GT9XX_ReadCfg
*	Èë¿Ú²ÎÊı:	ÎŞ
*	·µ »Ø Öµ:ÎŞ
*	º¯Êı¹¦ÄÜ:	¶ÁÈ¡GT9147ÅäÖÃ²ÎÊı
*	Ëµ    Ã÷:Í¨¹ı´®¿Ú´òÓ¡Êı¾İ£¬ÔÚĞŞ¸ÄÊı¾İÖ®Ç°£¬ĞèÒª½«Ô­³§µÄ²ÎÊı¶ÁÈ¡²¢×öºÃ±¸·İ£¬ÒÔÃâ¸Ä¶¯¹Ø¼üµÄ²ÎÊı		
******************************************************************************************/

void GT9XX_ReadCfg(void)
{
	uint8_t GT9XX_Cfg[184];	// Êı×é³¤¶ÈÈ¡¾öÓÚÊµ¼ÊµÄĞ¾Æ¬µÄ¼Ä´æÆ÷¸öÊı
	uint16_t i = 0;

	printf("-----------------------------------------\r\n");	
	printf("¶ÁÈ¡Ğ¾Æ¬ÅäÖÃĞÅÏ¢£¬´®¿Ú´òÓ¡Êä³ö\r\n");
	
	GT9XX_ReadReg (GT9XX_CFG_ADDR,184,GT9XX_Cfg);	// ¶ÁÅäÖÃĞÅÏ¢
	for(i=0;i<184;i++)
	{	
		if( (i%10 == 0) && (i>0) )			
		{
			printf("\r\n");
		}
		printf("0X%.2x,",GT9XX_Cfg[i]);
	}
	printf("\r\n-----------------------------------------\r\n");	
}

/*****************************************************************************************
*	º¯ Êı Ãû: Touch_Init
*	Èë¿Ú²ÎÊı: ÎŞ
*	·µ »Ø Öµ:	 SUCCESS  - ³õÊ¼»¯³É¹¦
*            ERROR 	 - ´íÎó£¬Î´¼ì²âµ½´¥ÃşÆÁ	
*	º¯Êı¹¦ÄÜ: ´¥ÃşIC³õÊ¼»¯£¬²¢¶ÁÈ¡ÏàÓ¦ĞÅÏ¢·¢ËÍµ½´®¿Ú
*	Ëµ    Ã÷: ÔÚ³ÌĞòÀïÖÜÆÚĞÔµÄµ÷ÓÃ¸Ãº¯Êı£¬ÓÃÒÔ¼ì²â´¥Ãş²Ù×÷£¬´¥ÃşĞÅÏ¢´æ´¢ÔÚ touchInfo ½á¹¹Ìå
******************************************************************************************/

uint8_t Touch_Init(void)
{
	uint8_t GT9XX_Info[11];	// ´¥ÃşÆÁICĞÅÏ¢
	uint8_t cfgVersion = 0;	// ´¥ÃşÅäÖÃ°æ±¾
	
	Touch_IIC_GPIO_Config(); 	// ³õÊ¼»¯IICÒı½Å
	GT9XX_Reset();					// GT9147 ¸´Î»
	
//	 //¶ÁÈ¡GT9147ÅäÖÃ²ÎÊı£¬Í¨¹ı´®¿Ú´òÓ¡Êä³ö	
//	//	Í¨¹ı´®¿Ú´òÓ¡Êı¾İ£¬ÔÚĞŞ¸ÄÊı¾İÖ®Ç°£¬ĞèÒª½«Ô­³§µÄ²ÎÊı¶ÁÈ¡²¢×öºÃ±¸·İ£¬ÒÔÃâ¸Ä¶¯¹Ø¼üµÄ²ÎÊı
//	GT9XX_ReadCfg();	
	
//	// ·¢ËÍÅäÖÃ²ÎÊı£¬ÓÉÓÚGT9147¿ÉÒÔµôµç±£´æÅäÖÃ²ÎÊı£¬Ò»°ãÇé¿öÏÂÓÃ»§ÎŞĞè¸ü¸Ä
//	//ÓÃ»§ĞŞ¸Ä²ÎÊıÖ®ºó£¬ĞèÒª½«¸Ãº¯ÊıÆÁ±Îµô£¬²»È»Æµ·±µÄĞ´Èë»á½«´¥ÃşĞ¾Æ¬µÄFlashĞ´»µ	
//	GT9XX_SendCfg();	
	
	GT9XX_ReadReg (GT9XX_ID_ADDR,11,GT9XX_Info);		// ¶Á´¥ÃşÆÁICĞÅÏ¢
	GT9XX_ReadReg (GT9XX_CFG_ADDR,1,&cfgVersion);	// ¶Á´¥ÃşÅäÖÃ°æ±¾
	
	if( GT9XX_Info[0] == '9' )		//	ÅĞ¶ÏµÚÒ»¸ö×Ö·ûÊÇ·ñÎª ¡®9¡¯
	{
//		printf("Touch ID£º GT%.4s \r\n",GT9XX_Info);									// ´òÓ¡´¥ÃşĞ¾Æ¬µÄID
//		printf("¹Ì¼ş°æ±¾£º 0X%.4x\r\n",(GT9XX_Info[5]<<8) + GT9XX_Info[4]);	// Ğ¾Æ¬¹Ì¼ş°æ±¾
//		printf("´¥Ãş·Ö±æÂÊ£º%d * %d\r\n",(GT9XX_Info[7]<<8) + GT9XX_Info[6],(GT9XX_Info[9]<<8) +GT9XX_Info[8]);	// µ±Ç°´¥Ãş·Ö±æÂÊ		
//		printf("´¥Ãş²ÎÊıÅäÖÃ°æ±¾£º 0X%.2x \r\n",cfgVersion);	// ´¥ÃşÅäÖÃ°æ±¾	
		return SUCCESS;
	}
	else
	{
//		printf("Î´¼ì²âµ½´¥ÃşIC\r\n");			//´íÎó£¬Î´¼ì²âµ½´¥ÃşÆÁ
		return ERROR;
	}
}

/*****************************************************************************************
*	º¯ Êı Ãû: Touch_Scan
*	Èë¿Ú²ÎÊı: ÎŞ
*	·µ »Ø Öµ: ÎŞ
*	º¯Êı¹¦ÄÜ: ´¥ÃşÉ¨Ãè
*	Ëµ    Ã÷: ÔÚ³ÌĞòÀïÖÜÆÚĞÔµÄµ÷ÓÃ¸Ãº¯Êı£¬ÓÃÒÔ¼ì²â´¥Ãş²Ù×÷£¬´¥ÃşĞÅÏ¢´æ´¢ÔÚ touchInfo ½á¹¹Ìå
******************************************************************************************/

void Touch_Scan(void)
{
 	uint8_t  touchData[2 + 8 * TOUCH_MAX ]; //ÓÃÓÚ´æ´¢´¥ÃşÊı¾İ
	uint8_t  i = 0;	
	
	GT9XX_ReadReg (GT9XX_READ_ADDR,2 + 8 * TOUCH_MAX ,touchData);	//¶ÁÊı¾İ
	GT9XX_WriteData (GT9XX_READ_ADDR,0);	//	Çå³ı´¥ÃşĞ¾Æ¬µÄ¼Ä´æÆ÷±êÖ¾Î»
	touchInfo.num = touchData[0] & 0x0f;	// È¡µ±Ç°µÄ´¥ÃşµãÊı£¬ÈôÎª0Ôò´ú±íÎŞ´¥ÃşÊı¾İ·¢Éú
	
	if ( (touchInfo.num >= 1) && (touchInfo.num <=5) ) 	//	µ±´¥ÃşÊıÔÚ 1-5 Ö®¼äÊ±
	{
		for(i=0;i<touchInfo.num;i++)		// È¡ÏàÓ¦µÄ´¥Ãş×ø±ê
		{
			touchInfo.y[i] = (touchData[5+8*i]<<8) | touchData[4+8*i];	// »ñÈ¡Y×ø±ê
			touchInfo.x[i] = (touchData[3+8*i]<<8) | touchData[2+8*i];	//	»ñÈ¡X×ø±ê			
		}
		touchInfo.flag = 1;	// ´¥Ãş±êÖ¾Î»ÖÃ1£¬´ú±íÓĞ´¥Ãş¶¯×÷·¢Éú
	}
	else                       
	{
		touchInfo.flag = 0;	// ´¥Ãş±êÖ¾Î»ÖÃ0£¬ÎŞ´¥Ãş¶¯×÷
	}
}


#endif








#define TOUCH_DEBUG 0
#if TOUCH_DEBUG
void	GUI_TouchScan(void)
{
	Touch_Scan();
	printf("x:%d\ty:%d\r\n",touchInfo.x[0],touchInfo.y[0]);
}
#else
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

#endif

