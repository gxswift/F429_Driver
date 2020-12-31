#include "app_task.h"

#include "dma2d.h"
#include "dma.h"
#include "ltdc.h"
#include "sdio.h"
#include "spi.h"
#include "usart.h"
#include "gpio.h"
#include "fmc.h"
#include "crc.h"
#include "fatfs.h"
#include "usb_device.h"
#include "adc.h"
#include "tim.h"


#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "croutine.h"
#include "timers.h"

#include "display.h"

#include "GUIDEMO.h"
#include "GUI_VNC.h"
#include "vnc_thread.h"
#include "touch.h"


#include "lwip.h"
#include "httpserver-netconn.h"
#include "smtp.h"
#include "lwip/apps/httpd.h"
#include "lwip/apps/netbiosns.h"
#include "lwip/apps/sntp.h"

#include "tcpecho.h"
#include "udpecho.h"
#include "ntp_client.h"

#include "bmp.h"
#include "Font_Test.h"
#include "tftp_file.h"


#include "funopts.h"

#include "GetData.h"


static TaskHandle_t xHandleTaskLed = NULL;
static TaskHandle_t xHandleTaskMsgPro = NULL;
static TaskHandle_t xHandleTaskSD = NULL;
static TaskHandle_t xHandleTaskTouch = NULL;
static xTimerHandle TouchScreenTimer = NULL;
static TaskHandle_t xHandleTaskScreen = NULL;
static TaskHandle_t xHandleTaskGUI = NULL;
static TaskHandle_t xHandleTaskGUIRAM = NULL;
//--------------------------------------------------------------
//--------------------------------------------------------------
//LED任务
uint8_t Led_Flag;
uint16_t Led_Time;
uint16_t Led_P;

static void vTaskLed(void *pvParameters)
{
	Led_Time = 500;
	Led_Flag = 1;
	HAL_TIM_PWM_Start(&htim4,TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim4,TIM_CHANNEL_2);
	static uint16_t flash;
	TIM4->CCR2 = 500;
	//BackLight Test
/*
	while(1)
	{
		TIM4->CCR2 = 700;//亮 4.3
		TIM4->CCR1 = 700;
		vTaskDelay(1000);
		TIM4->CCR2 = 200;
		TIM4->CCR1 = 200;
		vTaskDelay(500);	
		HAL_GPIO_TogglePin(GPIOG,GPIO_PIN_7);
	}
	*/
	while(1)
	{
		if (Led_Flag==1)
		{
			if(++flash>=Led_Time/2)
			{
				flash = 0;
				HAL_GPIO_TogglePin(GPIOG,GPIO_PIN_7);
			}
		}
		else if (Led_Flag == 0)
		{
			HAL_GPIO_WritePin(GPIOG,GPIO_PIN_7,0);
		}
		else
		{
			HAL_GPIO_WritePin(GPIOG,GPIO_PIN_7,1);
		}
		TIM4->CCR1 = 1000-Led_P;
		vTaskDelay(1);
	}
}
//--------------------------------------------------------------
//--------------------------------------------------------------
//按键截图
FIL file;
#define USE_GUIBMP 0
void _WriteByte2File(U8 Data, void * p) 
{
	f_putc(Data,p);
}
void ScreenShot()
{	
	char buf[20] = "0:/Screenshot.bmp";
	uint8_t temp;
	temp = f_open(&file,buf, FA_WRITE|FA_CREATE_ALWAYS);
	if(FR_OK == temp)
	{

		GUI_BMP_Serialize(_WriteByte2File,&file);		

		f_close(&file);
	}
}

//截图任务
//PI8  按键上拉输入   
static void vTaskScreenshot(void *pvParameters)
{
	static uint8_t temp = 1;
	uint32_t Use_Time;
	vTaskDelay(10000);//10秒后
	while(1)
	{	
		if((HAL_GPIO_ReadPin(GPIOI,GPIO_PIN_8) == 0)&& temp)
		{
		vTaskDelay(20);
			if(HAL_GPIO_ReadPin(GPIOI,GPIO_PIN_8) == 0)
			{
				Use_Time = HAL_GetTick();
				
#if 	USE_GUIBMP==0
				Create_BMP();
#elif USE_GUIBMP==1
				ScreenShot();
#endif				
				Use_Time = HAL_GetTick() - Use_Time;
				printf("截图成功! 用时%dms\r\n",Use_Time);
				temp = 0;
			}
		}
		else if(HAL_GPIO_ReadPin(GPIOI,GPIO_PIN_8))
				temp = 1;
		vTaskDelay(20);
	}
}
//--------------------------------------------------------------
//--------------------------------------------------------------
//网络任务
extern	void httpd_ssi_init();
extern	void 	httpd_cgi_init();
static void vTaskMsgPro(void *pvParameters)
{
#if NET	
	MY_ADC_Init();

	HAL_GPIO_WritePin(GPIOE,GPIO_PIN_3,0);
	HAL_Delay(1000);
	HAL_GPIO_WritePin(GPIOE,GPIO_PIN_3,1);
	HAL_Delay(1000);
	MX_LWIP_Init();
	
	#ifdef USE_DHCP
	xTaskCreate(DHCP_thread,
							"DHCP_Thread",
							512,
							NULL,
							3,
							NULL);
	#endif
	httpd_ssi_init();
	httpd_cgi_init();
	httpd_init();
#if VNC

#else
	tcpecho_init();
	udpecho_init();
	//ntp_client_init();
	TFTP_Start();	
#endif	
	netbiosns_set_name("gx.lwip");//
	netbiosns_init();	
	//----------------------------------------------
	sntp_setservername(0,"s1a.time.edu.cn");
	sntp_setservername(1,"s2c.time.edu.cn");
	sntp_setservername(2,"s2f.time.edu.cn");
	sntp_setservername(3,"s2j.time.edu.cn");
	sntp_init();
#endif
	while(1)
	{
			vTaskDelay(500);
	}
}
//--------------------------------------------------------------
//--------------------------------------------------------------
//触摸任务
static void vTimerCallback( xTimerHandle pxTimer )
{
   GUI_TouchScan();
}
static void vTaskTouch(void *pvParameters)
{
	#if LCD7
	#else
	Touch43_Init();
	#endif
	while(1)
	{
		GUI_TouchScan();
		#if TOUCH_DEBUG
		vTaskDelay(500);
		#else
		vTaskDelay(50);
		#endif
	}
}
//--------------------------------------------------------------
//--------------------------------------------------------------

FATFS fs[2];
FIL fil;
uint8_t look = 0;
uint32_t testsram[250] __attribute__((at(0XD0000000)));//测试用数组
uint32_t testsram2[2500] __attribute__((at(0XD0020000)));
	
extern void MainTask_U(void);
extern void MainTask_ETI(void);
extern void MainTask(void) ;


static void vSD_Task(void *pvParameters)
{
	uint8_t ReadBuff[100] = {0};
	uint8_t res;
	UINT brw;
//	BYTE work[4096];
	
	vTaskDelay(1000);
	//SD卡测试
		printf("SD fatfs test\r\n");
	look = f_mount(&fs[0],"0:/",0);
//	scan_files("0:/");
	look =f_open (&fil,"0:/123.txt",FA_OPEN_ALWAYS|FA_WRITE);
	f_puts("fatfs test \r\n文件系统测试\r\n屏幕截图 Screenshot.bmp",&fil);
	look = f_close(&fil);

 	memset(ReadBuff,0,50);
	look = f_open (&fil,"0:/123.txt",FA_OPEN_ALWAYS|FA_WRITE|FA_READ);

	while(1)
	 {
	 res = f_read(&fil,ReadBuff,sizeof(ReadBuff),&brw);
	 if(res||brw==0) break;
	 }
		look = f_close(&fil);

	 printf("SD:%s\r\n",ReadBuff);	
 //----------------------------------------------------------
	printf("spi flash fatfs test\r\n");
	 
	look = f_mount(&fs[1],"1:/",0);
//	f_mkfs("1:", FM_FAT32, 0, work, sizeof work);//	 
//	f_open (&fil,"1:/1.txt",FA_OPEN_ALWAYS|FA_WRITE);
//	f_puts("spiflash fatfs test!\r\n文件系统测试",&fil);
//	f_close(&fil);
	 //---------------
	 if (FR_OK == look)
	 {
		f_open (&fil,"1:/1.txt",FA_OPEN_ALWAYS|FA_WRITE|FA_READ);
		memset(ReadBuff,0,100);
		while(1)
		 {
		 res = f_read(&fil,ReadBuff,sizeof(ReadBuff),&brw);
		 if(res||brw==0) break;
		 }
		}
	 f_close(&fil);
	 printf("文件内容:\r\n%s\r\n",ReadBuff);
	 printf("----------------\r\n");
	 //----------------------------------------

	 printf("SDRAM测试\r\n");
	uint32_t i=0;  	  
	 for(i=0;i<2500;i++)
	{
		*(__IO uint16_t*)( 0XD0030000+i*2) = 15 ;
	} 	
	printf ("存入250个数\r\n");
	 for(i=0;i<250;i++)
	{
		testsram[i]=i;//*(3|3<<16)
	} 	
	printf ("第一次读取\r\n");
	for(i=0;i<250;i++)
	{
		printf ("%4d",testsram[i]);
	} 
	printf ("\r\n第二次读取\r\n");
	for(i=0;i<250;i++)
	{
		printf ("%4d",testsram[i]);
	} 
	for(i=0;i<2500;i++)
	{
		testsram2[i]=0x7e0|0x7e0<<16;
	} 	
	HAL_Delay(1000);
	Display_Init();
	Display_Test();

	//vTaskDelete(xHandleTaskSD);
	vTaskDelete(NULL);
	while(1)
	{
		vTaskDelay(1000);
	}
}
static void vGUI_Task(void *pvParameters)
{
	vTaskDelay(10000);
	WM_SetCreateFlags(WM_CF_MEMDEV);
	 GUI_Init();
#if VNC
	GUI_VNC_X_StartServer(0, 0);
	GUI_VNC_SetPassword("123456");
	GUI_VNC_SetProgName("Designed by GX");
	GUI_VNC_RingBell();
#endif	

	WM_MULTIBUF_Enable(1);
	WM_SetCreateFlags(WM_CF_MEMDEV);
		MainTask();
//	GUIDEMO_Main();
//	MainTask_ETI();
//	Font_Demo();
	vTaskDelay(1000);	
	printf("picture test\r\n");
	PicDisplaly_Test();
	while(1)
	{
		vTaskDelay(1000);
	}
}

char pWriteBuffer[2048];
static void vGUIRAM_Task(void *pvParameters)
{
	uint32_t Free;
	while(1)
	{
		Free = GUI_ALLOC_GetNumFreeBytes();
		printf("Free RAM = %d byte\r\n",Free);
		vTaskDelay(5000);
		    vTaskList((char *)&pWriteBuffer);
    printf("[task information]\r\n");
    printf("task name\tstate\tprior\tstack\tnumber\r\n");
    printf("------------------------------------------------\r\n");
    printf("%s\r\n", pWriteBuffer);
	}
	
}



void AppTaskCreate (void)
{
#if TOUCH
//-----------------------------------------------------------------
	#if LCD7
	#else
	Touch43_Init();
	#endif
	TouchScreenTimer = xTimerCreate ("Timer", 20, pdTRUE, ( void * ) 1, vTimerCallback );
	if( TouchScreenTimer != NULL )
  {
    if( xTimerStart( TouchScreenTimer, 0 ) != pdPASS )
    {
      /* The timer could not be set into the Active state. */
    }
  }
	//-----------------------------------------------------------------
//	xTaskCreate(vTaskTouch,
//							"vTaskTouch",
//							2048,
//							NULL,
//							4,
//							&xHandleTaskTouch);
#endif
	
	xTaskCreate(vTaskLed,
							"vTaskLed",
							512,
							NULL,
							2,
							&xHandleTaskLed);
	
	xTaskCreate(vTaskMsgPro,
							"vTaskMsgPro",
							512,
							NULL,
							3,
							&xHandleTaskMsgPro);
#if TEST
	xTaskCreate(vSD_Task,
							"SD_Task",
							512,
							NULL,
							1,
							&xHandleTaskSD
							);
#endif
							
#if GUI

	xTaskCreate(vGUI_Task,
							"GUI_Task",
							512,
							NULL,
							2,
							&xHandleTaskGUI
							);
		xTaskCreate(vGUIRAM_Task,
							"GUIRAM_Task",
							512,
							NULL,
							2,
							&xHandleTaskGUIRAM
							);
	#if SCREENSHOT
	xTaskCreate(vTaskScreenshot,
							"vTaskScreenshot",
							512,
							NULL,
							2,
							&xHandleTaskScreen);
							
	#endif
#endif							
}
