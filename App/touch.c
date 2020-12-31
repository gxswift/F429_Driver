#include "touch.h"


#include "funopts.h"


TouchStructure touchInfo;	//�ṹ������

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
//������� �л�����
//��������ʼ��IIC��GPIO��
//PB2	:SDA
//PC13:SCL
// ����: ��������ʼ��
//
void Touch_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOI_CLK_ENABLE();  
  
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
  
  GPIO_InitStruct.Pin = GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = GPIO_PIN_11;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOI, &GPIO_InitStruct);
}
#define LIB 1
#if LIB
//	����������IIC�����ݽ�Ϊ���ģʽ

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

//	����������IIC�����ݽ�Ϊ����ģʽ
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
GPIOB->MODER|=1<<2*2;//�?3?
GPIOB->MODER|=0<<2*2;//�?�?
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
// IO�ڲ���
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

//	������IIC��ʼ�ź�
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

//	������IICֹͣ�ź�
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

//	������IICӦ���ź�
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

//	������IIC��Ӧ���ź�
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

//	�������ȴ��豸������Ӧ�ͺ�
uint8_t IIC_Touch_WaitResponse(void)
{

	SCL(0);
	T_Delay( Touch_DelayVaule );
	SDA(1);
	T_Delay( Touch_DelayVaule );
	SCL(1);
//	IIC_Touch_SDA_In();	//����Ϊ����ģʽ
	T_Delay( Touch_DelayVaule );
	SCL(0);	
	if( HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_2) != 0) //�ж��豸�Ƿ���������Ӧ
	{		
		return (Response_ERR);
	}
	else
	{
		return (Response_OK);
	}
}

// ������IICд�ֽ�
//	������IIC_Data - Ҫд���8λ����
//	���أ��豸����Ӧ�򷵻� 1������Ϊ0
//
uint8_t IIC_Touch_WriteByte(uint8_t IIC_Data)
{
	uint8_t i;
	//IIC_Touch_SDA_Out(); //���ݽ�Ϊ���ģʽ
	for (i = 0; i < 8; i++)
	{
		SDA(IIC_Data & 0x80);
		T_Delay( Touch_DelayVaule );
		SCL(1);
		T_Delay( Touch_DelayVaule );
		SCL(0);		
		IIC_Data <<= 1;
	}
	return (IIC_Touch_WaitResponse()); //�ȴ��豸��Ӧ
}

// ������IIC���ֽ�
//	������ResponseMode - Ӧ��ģʽѡ��
//       ResponseMode = 1 ʱ��CPU������Ӧ�źţ�Ϊ 0 ʱ��CPU������Ӧ���ź�
//	���أ�����������
uint8_t IIC_Touch_ReadByte(uint8_t ResponseMode)
{
	uint8_t IIC_Data;
	uint8_t i;
	
	SDA(1);
	SCL(0);
	//IIC_Touch_SDA_In(); //����ģʽ
	//��һ�ֽ�����
	for (i = 0; i < 8; i++)
	{
		IIC_Data <<= 1;
		SCL(1);
		T_Delay( Touch_DelayVaule );
		IIC_Data |= (HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_2) & 0x01);
		SCL(0);
		T_Delay( Touch_DelayVaule );
	}
	//	������Ӧ�ź�
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



/*---------------------- GT9XX��غ��� ------------------------*/
#if LCD7
// ������GT9XX д����
//	������addr - Ҫ�����ļĴ���
//			
uint8_t GT9XX_WriteHandle (uint16_t addr)
{
	uint8_t status;

	IIC_Touch_Start();
	if( IIC_Touch_WriteByte(GT9XX_IIC_WADDR) == Response_OK ) //д����ָ��
	{
		if( IIC_Touch_WriteByte((uint8_t)(addr >> 8)) == Response_OK ) //д��16��ַ
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

// ������GT9XX д����
//	������addr - Ҫд���ݵĵ�ַ
//			value - д�������
//
uint8_t GT9XX_WriteData (uint16_t addr,uint8_t value)
{
	uint8_t status;
	
	IIC_Touch_Start(); //����IICͨѶ

	if( GT9XX_WriteHandle(addr) == SUCCESS)	//д��Ҫ�����ļĴ���
	{
		if (IIC_Touch_WriteByte(value) != Response_OK) //д����
		{
			status = ERROR;						
		}
	}
	
	IIC_Touch_Stop(); //ֹͣͨѶ
	
	status = SUCCESS;
	return status;
}

// ������GT9XX ������
//	������addr - Ҫ�����ݵĵ�ַ
//			num - �������ֽ���
//			*value - ���ڻ�ȡ�洢���ݵ��׵�ַ
//
uint8_t GT9XX_ReadData (uint16_t addr, uint8_t cnt, uint8_t *value)
{
	uint8_t status;
	uint8_t i;

	status = ERROR;
	IIC_Touch_Start();

	if( GT9XX_WriteHandle(addr) == SUCCESS) //д��Ҫ�����ļĴ���
	{
		IIC_Touch_Start(); //��������IICͨѶ

		if (IIC_Touch_WriteByte(GT9XX_IIC_RADDR) == Response_OK)
		{	
			for(i = 0 ; i < cnt; i++)
			{
				if (i == (cnt - 1))
				{
					value[i] = IIC_Touch_ReadByte(0);//�������һ������ʱ���� ��Ӧ���ź�
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


#else
//4.3LCD
// ���������������飬�ں��� GT9XX_SendCfg() ����ã��������ô���IC����ز���
//	����GT9147���Թ̻�������Щ�����������û�һ������������ٽ�������
//	��ϸ�ļĴ���������ο���GT9147���ָ�ϡ�
//
const uint8_t GT9XX_CFG_DATA[] = 	
{                              	 
	0XAA,			// �Ĵ�����ַ��0x8047�����ܣ����ð汾��
	
	0XE0,0X01,	// �Ĵ�����ַ��0x8048~0x8049�����ܣ�X�������ֵ����λ��ǰ
	0X10,0X01,	// �Ĵ�����ַ��0x804A~0x804B�����ܣ�Y�������ֵ����λ��ǰ
	
	0X05,			// �Ĵ�����ַ��0x804C�����ܣ����������������1~5��
	0X0E,			// �Ĵ�����ַ��0x804D�����ܣ�����INT������ʽ��XY���꽻��
	0X00,			// �üĴ�����������
	0X88,			// �Ĵ�����ַ��0x804F�����ܣ����»��ɿ�ȥ������
	0X0B,			// �Ĵ�����ַ��0x8050�����ܣ�ԭʼ���괰���˲�ֵ
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
*	�Ĵ�����ַ:	0x80B7~0X80C4
*	����˵��  :	�޸ĸ�Ӧͨ����Ӧ��оƬͨ���ţ����Ըı䴥�����Ĵ�ֱɨ�跽��
*******************************************************************************************/

	0X08,0X0A,0X0C,0X0E,0X10,0X12,0X14,0X16,0X18,0X1A,		// ɨ�跽��� �� �� ��
//	0X1A,0X18,0X16,0X14,0X12,0X10,0X0E,0X0C,0X0A,0X08,		// ɨ�跽��� �� �� ��	
	0X00,0X00,0X00,0X00,												// δʹ�õĸ�Ӧͨ���������������
	
/******************************************************************************************/

	0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,		// �����üĴ�������������
	0X00,0X00,0X00,0X00,0x00,0x00, 								// �����üĴ�������������
	
/*******************************************************************************************
*	�Ĵ�����ַ:	0x80D5~0X80EE
*	����˵��  :	�޸�����ͨ����Ӧ��оƬͨ���ţ����Ըı䴥������ˮƽɨ�跽��
********************************************************************************************/

	0x00,0x02,0x04,0x05,0x06,0x08,0x0a,0x0c, 				// ɨ�跽��� �� �� ��
	0x0e,0x1d,0x1e,0x1f,0x20,0x22,0x24,0x28,0x29,      

// 0X29,0X28,0X24,0X22,0X20,0X1F,0X1E,0X1D,				// ɨ�跽��� �� �� ��
//	0X0E,0X0C,0X0A,0X08,0X06,0X05,0X04,0X02,0X00,	
	
	0xff, 	// δʹ�õ�����ͨ���������������
	
/*******************************************************************************************/

	0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,		// ͨ������ϵ���Ĵ����������޸�
	0X00,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,		// ͨ������ϵ���Ĵ����������޸�
	0XFF,0XFF,0XFF,0XFF,												// ͨ������ϵ���Ĵ����������޸�
};


/*****************************************************************************************
*	�� �� ��:	GT9XX_Reset
*	��ڲ���:	��
*	�� �� ֵ:	��
*	��������:	��λGT9147
*	˵    ��:��λGT9147������оƬ��IIC��ַ����Ϊ0xBA/0xBB
******************************************************************************************/

void GT9XX_Reset(void)
{
	//Touch_INT_Out();	//	��INT��������Ϊ���
	
	// ��ʼ������״̬
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_15,0); 	// ��INT����
	HAL_GPIO_WritePin(GPIOI,GPIO_PIN_11,1);	// ��RST����
	T_Delay(10000);	
	
	// ��ʼִ�и�λ
	//	INT���ű��ֵ͵�ƽ���䣬��������ַ����Ϊ0XBA/0XBB
	HAL_GPIO_WritePin(GPIOI,GPIO_PIN_11,0);			// ���͸�λ���ţ���ʱоƬִ�и�λ
	T_Delay(350000);										// ��ʱ
	HAL_GPIO_WritePin(GPIOI,GPIO_PIN_11,1);			// ���߸�λ���ţ���λ����
	T_Delay(350000);										// ��ʱ
	//Touch_INT_In();													// INT����תΪ��������
	T_Delay(350000);										// ��ʱ
}

/*****************************************************************************************
*	�� �� ��:	GT9XX_WriteHandle
*	��ڲ���:	addr - Ҫ�����ļĴ���
*	�� �� ֵ:	SUCCESS - �����ɹ�
*				ERROR	  - ����ʧ��
*	��������:	GT9XX д����
*	˵    ��:��ָ���ļĴ���ִ��д����
******************************************************************************************/

uint8_t GT9XX_WriteHandle (uint16_t addr)
{
	uint8_t status;	// ״̬��־λ

	IIC_Touch_Start();	// ����IICͨ��
	
	if( IIC_Touch_WriteByte(GT9XX_IIC_WADDR) == 1 ) //д����ָ��
	{
		if( IIC_Touch_WriteByte((uint8_t)(addr >> 8)) == 1 ) //д��16λ��ַ
		{
			if( IIC_Touch_WriteByte((uint8_t)(addr)) != 1 )
			{
				status = ERROR;	// ����ʧ��
			}			
		}
	}
	status = SUCCESS;	// �����ɹ�
	return status;	
}

/*****************************************************************************************
*	�� �� ��:	GT9XX_WriteData
*	��ڲ���:	addr - Ҫд��ļĴ���
*				value - Ҫд�������
*	�� �� ֵ:	SUCCESS - �����ɹ�
*				ERROR	  - ����ʧ��
*	��������:	GT9XX дһ�ֽ�����
*	˵    ��:��ָ���ļĴ���д��һ�ֽ�����
******************************************************************************************/

uint8_t GT9XX_WriteData (uint16_t addr,uint8_t value)
{
	uint8_t status;
	
	IIC_Touch_Start(); //����IICͨѶ

	if( GT9XX_WriteHandle(addr) == SUCCESS)	//д��Ҫ�����ļĴ���
	{
		if (IIC_Touch_WriteByte(value) != 1) //д����
		{
			status = ERROR;	// д��ʧ��					
		}
	}	
	IIC_Touch_Stop(); // ֹͣͨѶ
	
	status = SUCCESS;	// д��ɹ�
	return status;
}

/*****************************************************************************************
*	�� �� ��:	GT9XX_WriteReg
*	��ڲ���:	addr - Ҫд��ļĴ��������׵�ַ
*				cnt  - ���ݳ���
*				value - Ҫд���������
*	�� �� ֵ:	SUCCESS - �����ɹ�
*				ERROR	  - ����ʧ��
*	��������:	GT9XX д�Ĵ���
*	˵    ��:��оƬ�ļĴ�����д��ָ�����ȵ�����
******************************************************************************************/

uint8_t GT9XX_WriteReg (uint16_t addr, uint8_t cnt, uint8_t *value)
{
	uint8_t status = 0;
	uint8_t i = 0;

	IIC_Touch_Start();	// ����IICͨ��

	if( GT9XX_WriteHandle(addr) == SUCCESS) //д��Ҫ�����ļĴ���
	{
		for(i = 0 ; i < cnt; i++)		// ����
		{
			IIC_Touch_WriteByte(value[i]);	// д������
		}					
		IIC_Touch_Stop();		// ֹͣIICͨ��
		status = SUCCESS;		// д��ɹ�
	}
	else
	{
		IIC_Touch_Stop();		// ֹͣIICͨ��
		status = ERROR;		// д��ʧ��
	}

	return status;	
}

/*****************************************************************************************
*	�� �� ��:	GT9XX_ReadReg
*	��ڲ���:	addr - Ҫ��ȡ�ļĴ��������׵�ַ
*				cnt  - ���ݳ���
*				value - Ҫ��ȡ��������
*	�� �� ֵ:	SUCCESS - �����ɹ�
*				ERROR	  - ����ʧ��
*	��������:	GT9XX ���Ĵ���
*	˵    ��:��оƬ�ļĴ�������ȡָ�����ȵ�����
******************************************************************************************/

uint8_t GT9XX_ReadData (uint16_t addr, uint8_t cnt, uint8_t *value)
{
	uint8_t status = 0;
	uint8_t i = 0;

	status = ERROR;
	IIC_Touch_Start();	// ����IICͨ��

	if( GT9XX_WriteHandle(addr) == SUCCESS) 	//	д��Ҫ�����ļĴ���
	{
		IIC_Touch_Start(); // ��������IICͨѶ

		if (IIC_Touch_WriteByte(GT9XX_IIC_RADDR) == 1)	// ���Ͷ�����
		{	
			for(i = 0 ; i < cnt; i++)	// ����
			{
				if (i == (cnt - 1))
				{
					value[i] = IIC_Touch_ReadByte(0);	//	�������һ������ʱ���� ��Ӧ���ź�
				}
				else
				{
					value[i] = IIC_Touch_ReadByte(1);	// ����Ӧ���ź�
				}
			}					
			IIC_Touch_Stop();	// ֹͣIICͨ��
			status = SUCCESS;
		}
	}
	IIC_Touch_Stop();
	return (status);	
}

/*****************************************************************************************
*	�� �� ��:	GT9XX_SendCfg
*	��ڲ���:	��
*	�� �� ֵ:��
*	��������:	����GT9147���ò���
*	˵    ��:����GT9147���Ե��籣�����ò��������ҳ���ʱ�����úã�һ��������û�������ģ�
*				�û��޸Ĳ���֮����Ҫ���ú������ε�����ȻƵ����д��Ὣ����оƬ��Flashд��
******************************************************************************************/

void GT9XX_SendCfg(void)
{
	uint8_t GT9XX_Check[2];
	uint8_t i=0;
	
	GT9XX_Check[1] = 1;		// ���ø��±�־
	
	for(i=0;i<sizeof(GT9XX_CFG_DATA);i++)
	{
		GT9XX_Check[0] += GT9XX_CFG_DATA[i];	//����У���
	}
   GT9XX_Check [0] = (~GT9XX_Check[0])+1;		
	
	GT9XX_WriteReg(0X8047,sizeof(GT9XX_CFG_DATA),(uint8_t*)GT9XX_CFG_DATA);	//	���ͼĴ�������
	GT9XX_WriteReg(0X80FF,2,GT9XX_Check); // д��У��ͣ�����������
} 

/*****************************************************************************************
*	�� �� ��:	GT9XX_ReadCfg
*	��ڲ���:	��
*	�� �� ֵ:��
*	��������:	��ȡGT9147���ò���
*	˵    ��:ͨ�����ڴ�ӡ���ݣ����޸�����֮ǰ����Ҫ��ԭ���Ĳ�����ȡ�����ñ��ݣ�����Ķ��ؼ��Ĳ���		
******************************************************************************************/

void GT9XX_ReadCfg(void)
{
	uint8_t GT9XX_Cfg[184];	// ���鳤��ȡ����ʵ�ʵ�оƬ�ļĴ�������
	uint16_t i = 0;

	printf("-----------------------------------------\r\n");	
	printf("��ȡоƬ������Ϣ�����ڴ�ӡ���\r\n");
	
	GT9XX_ReadData (GT9XX_CFG_ADDR,184,GT9XX_Cfg);	// ��������Ϣ
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
*	�� �� ��: Touch_Init
*	��ڲ���: ��
*	�� �� ֵ:	 SUCCESS  - ��ʼ���ɹ�
*            ERROR 	 - ����δ��⵽������	
*	��������: ����IC��ʼ��������ȡ��Ӧ��Ϣ���͵�����
*	˵    ��: �ڳ����������Եĵ��øú��������Լ�ⴥ��������������Ϣ�洢�� touchInfo �ṹ��
******************************************************************************************/

uint8_t Touch43_Init(void)
{
	uint8_t GT9XX_Info[11];	// ������IC��Ϣ
	uint8_t cfgVersion = 0;	// �������ð汾
	
//	Touch_IIC_GPIO_Config(); 	// ��ʼ��IIC����
	GT9XX_Reset();					// GT9147 ��λ
	
//	 //��ȡGT9147���ò�����ͨ�����ڴ�ӡ���	
//	//	ͨ�����ڴ�ӡ���ݣ����޸�����֮ǰ����Ҫ��ԭ���Ĳ�����ȡ�����ñ��ݣ�����Ķ��ؼ��Ĳ���
//	GT9XX_ReadCfg();	
	
//	// �������ò���������GT9147���Ե��籣�����ò�����һ��������û��������
//	//�û��޸Ĳ���֮����Ҫ���ú������ε�����ȻƵ����д��Ὣ����оƬ��Flashд��	
//	GT9XX_SendCfg();	
	
	GT9XX_ReadData (GT9XX_ID_ADDR,11,GT9XX_Info);		// ��������IC��Ϣ
	GT9XX_ReadData (GT9XX_CFG_ADDR,1,&cfgVersion);	// ���������ð汾
	
	if( GT9XX_Info[0] == '9' )		//	�жϵ�һ���ַ��Ƿ�Ϊ ��9��
	{
//		printf("Touch ID�� GT%.4s \r\n",GT9XX_Info);									// ��ӡ����оƬ��ID
//		printf("�̼��汾�� 0X%.4x\r\n",(GT9XX_Info[5]<<8) + GT9XX_Info[4]);	// оƬ�̼��汾
//		printf("�����ֱ��ʣ�%d * %d\r\n",(GT9XX_Info[7]<<8) + GT9XX_Info[6],(GT9XX_Info[9]<<8) +GT9XX_Info[8]);	// ��ǰ�����ֱ���		
//		printf("�����������ð汾�� 0X%.2x \r\n",cfgVersion);	// �������ð汾	
		return SUCCESS;
	}
	else
	{
//		printf("δ��⵽����IC\r\n");			//����δ��⵽������
		return ERROR;
	}
}

/*

void Touch_Scan(void)
{
 	uint8_t  touchData[2 + 8 * TOUCH_MAX ]; //���ڴ洢��������
	uint8_t  i = 0;	
	
	GT9XX_ReadReg (GT9XX_READ_ADDR,2 + 8 * TOUCH_MAX ,touchData);	//������
	GT9XX_WriteData (GT9XX_READ_ADDR,0);	//	�������оƬ�ļĴ�����־λ
	touchInfo.num = touchData[0] & 0x0f;	// ȡ��ǰ�Ĵ�����������Ϊ0������޴������ݷ���
	
	if ( (touchInfo.num >= 1) && (touchInfo.num <=5) ) 	//	���������� 1-5 ֮��ʱ
	{
		for(i=0;i<touchInfo.num;i++)		// ȡ��Ӧ�Ĵ�������
		{
			touchInfo.y[i] = (touchData[5+8*i]<<8) | touchData[4+8*i];	// ��ȡY����
			touchInfo.x[i] = (touchData[3+8*i]<<8) | touchData[2+8*i];	//	��ȡX����			
		}
		touchInfo.flag = 1;	// ������־λ��1�������д�����������
	}
	else                       
	{
		touchInfo.flag = 0;	// ������־λ��0���޴�������
	}
}

*/
#endif


// ����������ɨ��

uint8_t	Touch_Scan(void)
{
 	uint8_t  touchData[2 + 8 * TOUCH_MAX ]; //���ڴ洢��������
	memset(touchData,0,10);
	GT9XX_ReadData (GT9XX_READ_ADDR,2 + 8 * TOUCH_MAX ,touchData);	//������
	GT9XX_WriteData (GT9XX_READ_ADDR,0);	//	�������оƬ�ļĴ�����־λ
	touchInfo.num = touchData[0] & 0x0f;	//ȡ��ǰ�Ĵ�������
	
	if ( (touchInfo.num ) &&touchData[5]<200 && touchData[3]<200 ) //���������� 1-5 ֮��ʱ>= 1) && (touchInfo.num <=5
	{
		// ȡ��Ӧ�Ĵ�������
		#if LCD7	
		touchInfo.y[0] = ((touchData[5]<<8) | touchData[4])*0.81+1;
		touchInfo.x[0] = ((touchData[3]<<8) | touchData[2])*0.78+1;	
		#else
		touchInfo.y[0] = (touchData[5]<<8) | touchData[4];
		touchInfo.x[0] = (touchData[3]<<8) | touchData[2];	
		#endif
		return	SUCCESS ;	
	}
	else                       
	{	
		touchInfo.x[0] = 0;
		touchInfo.y[0] = 0;
		return	ERROR ;		
	}
}






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
	Touch_Scan(); //����ɨ��
	State.x = touchInfo.x[0];
	State.y = touchInfo.y[0];
	#if 0
	if (touchInfo.x[0]) {
	State.Pressed = 1;
	} else {
	State.Pressed = 0;
	}
	GUI_TOUCH_StoreStateEx(&State);
	#else
	if (0 == touchInfo.x[0])
	{
		State.x = -1;
		State.y = -1;
	}		
	
	GUI_TOUCH_StoreState(State.x, State.y);
	#endif
}
#endif

