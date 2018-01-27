#include "lwip/debug.h"
#include "lwip/apps/httpd.h"
#include "lwip/tcp.h"
#include "fs.h"

#include <string.h>
#include <stdlib.h>

#include "stm32f4xx_hal.h"


#include "adc.h"

#define NUM_CONFIG_CGI_URIS	3 //CGI的URI数量
#define NUM_CONFIG_SSI_TAGS	4  //SSI的TAG数量

//控制LED和BEEP的CGI handler
const char* Login_CGI_Handler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[]);
const char* Set_CGI_Handler(int iIndex,int iNumParams,char *pcParam[],char *pcValue[]);
const char* Ctrl_CGI_Handler(int iIndex,int iNumParams,char *pcParam[],char *pcValue[]);


extern uint8_t Led_Flag;
extern uint16_t Led_Time;
extern uint16_t Led_P;



static const tCGI ppcURLs[]= //cgi程序
{
	{"/login.cgi",Login_CGI_Handler},
	{"/set.cgi",Set_CGI_Handler},
	{"/ctrl.cgi",Ctrl_CGI_Handler}
};
static const char *ppcTAGs[]=  //SSI的Tag
{
	"w", //温度值
	"t",
	"s",
	"c"
};


//SSIHandler中需要用到的处理ADC的函数
void Tim_Handler(char *pcInsert)
{ 
    uint32_t temp = 0;   
	temp = HAL_GetTick();
		sprintf(pcInsert,"%d,%d",temp/1000,temp%1000);
}
extern short Get_Temprate(void);
void Temp_Handler(char *pcInsert)
{ 
    uint32_t temp = 0; 
		temp = Get_Temprate(); 
		sprintf(pcInsert,"%d.%d℃",temp/100,temp%100);
}
void State_Handler(char *pcInsert)
{ 
	if(Led_Flag == 0)
		sprintf(pcInsert,"LED开");
	else if(Led_Flag == 1)
		sprintf(pcInsert,"LED闪：%dms",Led_Time/2);
	else
		sprintf(pcInsert,"LED灭");
}
extern uint32_t IPaddress;
extern uint32_t Maskaddress;
extern uint32_t GWaddress;
void Address_Handler(char *pcInsert)
{
			sprintf(pcInsert,"IP_Address:%d.%d.%d.%d\tMask_Address:%d.%d.%d.%d\tGW_Address:%d.%d.%d.%d\r\n",
			IPaddress&0xff,
			IPaddress>>8&0xff,
			IPaddress>>16&0xff,
			IPaddress>>24,
			Maskaddress&0xff,
			Maskaddress>>8&0xff,
			Maskaddress>>16&0xff,
			Maskaddress>>24,
			GWaddress&0xff,
			GWaddress>>8&0xff,
			GWaddress>>16&0xff,
			GWaddress>>24);
}
//SSI的Handler句柄
static u16_t SSIHandler(int iIndex,char *pcInsert,int iInsertLen)
{
	switch(iIndex)
	{
		case 0: 
				Temp_Handler(pcInsert);
				break;
		case 1:
				Tim_Handler(pcInsert);
			break;
		case 2:
			State_Handler(pcInsert);
			break;
		case 3:
			Address_Handler(pcInsert);
		break;
	}
	return strlen(pcInsert);
}


//SSI句柄初始化
void httpd_ssi_init(void)
{  
	//配置SSI句柄
	http_set_ssi_handler(SSIHandler,ppcTAGs,NUM_CONFIG_SSI_TAGS);
}

//当web客户端请求浏览器的时候,使用此函数被CGI handler调用
static int FindCGIParameter(const char *pcToFind,char *pcParam[],int iNumParams)
{
	int iLoop;
	for(iLoop = 0;iLoop < iNumParams;iLoop ++ )
	{
		if(strcmp(pcToFind,pcParam[iLoop]) == 0)
		{
			return (iLoop); //返回iLOOP
		}
	}
	return (-1);
}

const char *Login_CGI_Handler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[])
{

  uint8_t i=0; //???????????????i???
      uint8_t passchack=0;
      iIndex =FindCGIParameter("username",pcParam,iNumParams);  //??bktime????
      if(iIndex != -1) //??pagingvol???
      {
           for(i = 0;i < iNumParams;i++)
           {
                 if(strcmp(pcParam[i],"username")== 0)  //??CGI??
                 {
                      if(strcmp(pcValue[i],"admin")== 0)//?????
                      {
                            passchack++;
                      }
                 }
                 else if(strcmp(pcParam[i],"password") == 0)  //??CGI??
                 {
                      if(strcmp(pcValue[i],"admin")== 0)//????
                      {
                            passchack++;
                      }
                 }
           }
           if(passchack > 1)//??????????
           {
                 return "/menu.html";     //????????????????????
           }
      }
      return "/alert.html";      //????????????????????
}


const char* Ctrl_CGI_Handler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[])
{
	uint8_t i=0;  
	iIndex = FindCGIParameter("sex",pcParam,iNumParams); 
	if (iIndex != -1)
	{
		for (i=0; i<iNumParams; i++) //??CGI??
		{
		  if (strcmp(pcParam[i] , "sex")==0)  //????"led" ????LED1??
		  {
			if(strcmp(pcValue[i], "male") ==0)  //??LED1??
				Led_Flag=0; //??LED1
			else if(strcmp(pcValue[i],"female") == 0)
				Led_Flag=2; //??LED1
			else if(strcmp(pcValue[i],"flash") == 0)
				Led_Flag=1; //??LED1			
			
		  }	
		}
	 }
	return "/control.html";   							//LED1?,BEEP?					
}

const char* Set_CGI_Handler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[])
{
	uint8_t i=0;  
	iIndex = FindCGIParameter("rate",pcParam,iNumParams); 
	if (iIndex != -1)
	{
		for (i=0; i<iNumParams; i++)
		{
		  if (strcmp(pcParam[i] , "rate")==0)
		  {
				if(atoi(pcValue[i]))
				Led_Time = atoi(pcValue[i])*100;
		  }
		  if (strcmp(pcParam[i] , "time")==0)
		  {
				if(atoi(pcValue[i]))
				Led_P = atoi(pcValue[i])*10;
		  }
		}
	 }
	return "/sys.html";   							//LED1?,BEEP?					
}
//CGI句柄初始化
void httpd_cgi_init(void)
{ 
  //配置CGI句柄
  http_set_cgi_handlers(ppcURLs, NUM_CONFIG_CGI_URIS);
}
