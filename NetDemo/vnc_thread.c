#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "GUI.h"
#include "GUI_VNC.h"

#include "tcpecho.h"
#include "lwip/opt.h"
#include "lwip/sys.h"
#include "lwip/api.h"
#include "lwip/sockets.h"

#include "vnc_thread.h"

/*********************************************************************
*       Static data
**********************************************************************
*/
#define USE_SOCKET	1
#if USE_SOCKET


int _Sock;
static uint8_t iptxt[128];
static struct sockaddr_in _Addr;
static GUI_VNC_CONTEXT    _Context;
static int is_initialized = 0;
static int Connection_accepted = 0;


/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Called by the server to send data
  * @param  buf: buffer to be sent.
  * @param  len: length of buf.
  * @param  pConnectionInfo: Connection info  
  * @retval transmit status.
  */
static int _Send(const U8 * buf, int len, void * pConnectionInfo) {
  return ( send((long)pConnectionInfo, (const char *)buf, len, 0));
}

/**
  * @brief  Called by the server when data is received
  * @param  buf: buffer to get the received data.
  * @param  len: length of received data.
  * @param  pConnectionInfo: Connection info  
  * @retval receive status.
  */
static int _Recv(U8 * buf, int len, void * pConnectionInfo) {
  return recv((long)pConnectionInfo, (char *)buf, len, 0);
}

/**
  * @brief  Starts listening at a TCP port.
  * @param  Port: TCP port to listen at 
  * @retval listen status.
  */
static int _ListenAtTcpAddr(U16 Port) {
  struct sockaddr_in addr;
  int sock;
  
  sock = socket(AF_INET, SOCK_STREAM, 0);
  memset(&addr, 0, sizeof(addr));
  addr.sin_family      = AF_INET;
  addr.sin_port        = htons(Port);
  addr.sin_addr.s_addr = INADDR_ANY;
  bind(sock, (struct sockaddr *)&addr, sizeof(addr));
  listen(sock, 1);
  return sock;
}
/**
  * @brief  VNC server thread process
  * @param  argument: not used
  * @retval none
  */
static void VNC_Process( void)
{
  static int s;
  static uint8_t Is_SocketAssigned = 0;
  u32_t AddrLen;
  U16 Port;
  
  if(Is_SocketAssigned == 0)
  {
    Is_SocketAssigned = 1;
    /* Prepare socket (one time setup) 
    Default port for VNC is is 590x, where x is the 0-based layer index */
    Port = 5900 + _Context.ServerIndex; 
    
    /* Loop until we get a socket into listening state */
    do {
      s = _ListenAtTcpAddr(Port);
      if (s != -1) {
        break;
      }
      vTaskDelay(100); /* Try again */
    } while (1);
  }
  while (1) 
  {
    
    /* Wait for an incoming connection */
    AddrLen = sizeof(_Addr);
    Connection_accepted = 1;
    if ((_Sock = accept(s, (struct sockaddr*)&_Addr, &AddrLen)) < 1) {
      closesocket(_Sock);
      vTaskDelay(100);      
      continue; /* Error */
    }
    Connection_accepted = 0;
    GUI_VNC_Process(&_Context, _Send, _Recv, (void *)_Sock);

    /* Close the connection */
    closesocket(_Sock);
		memset(&_Addr, 0, sizeof(struct sockaddr_in));
  }
}

int GUI_VNC_X_StartServer(int LayerIndex, int ServerIndex) {

  GUI_VNC_AttachToLayer(&_Context, LayerIndex);
	_Context.ServerIndex = ServerIndex;
  xTaskCreate(VNC_Process,
  	"VNC_Task",
  	1024,
  	NULL,
  	3,
  	NULL
  	);
  return 0;
}
#else
static GUI_VNC_CONTEXT    _Context;

static int  VNC_Recv(void * buf, uint16_t len, void * pConnectionInfo) {

err_t r;
struct netbuf *Data;
		r = netconn_recv((struct netconn*)pConnectionInfo, &Data);
		netbuf_data(Data, &buf, &len);
	netbuf_delete(Data);
return r;
}

static int VNC_Send(const U8 * buf, int len, void * pConnectionInfo) {
  err_t r;
	r = netconn_write((struct netconn*)pConnectionInfo, buf, len, NETCONN_COPY);
  return r;
}
  
static void VNC_Task(void *pvParameters){
  struct netconn *conn, *newconn;
  err_t err;
  conn = netconn_new(NETCONN_TCP);
  netconn_bind(conn, IP_ADDR_ANY, 5900);
  netconn_listen(conn);
	while(1)
	{
    err = netconn_accept(conn, &newconn);

    if (err == ERR_OK) {
      struct netbuf *buf;
//      void *data;
//      u16_t len;
      GUI_VNC_Process(&_Context, VNC_Send, VNC_Recv, (void *)newconn);
        netbuf_delete(buf);
			
      netconn_close(newconn);
      netconn_delete(newconn);
    }
  }
}

int GUI_VNC_X_StartServer(int LayerIndex, int ServerIndex) {

  GUI_VNC_AttachToLayer(&_Context, LayerIndex);
	_Context.ServerIndex = ServerIndex;
  xTaskCreate(VNC_Task,
  	"VNC_Task",
  	1024,
  	NULL,
  	3,
  	NULL
  	);
  return 0;
}

#endif
/*************************** End of file ****************************/
