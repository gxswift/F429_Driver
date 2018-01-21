/*
 * Copyright (c) 2001-2003 Swedish Institute of Computer Science.
 * All rights reserved. 
 * 
 * Redistribution and use in source and binary forms, with or without modification, 
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission. 
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED 
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT 
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT 
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING 
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 * 
 * Author: Adam Dunkels <adam@sics.se>
 *
 */
#include "tcpecho.h"

#include "lwip/opt.h"

#if LWIP_NETCONN

#include "lwip/sys.h"
#include "lwip/api.h"
/*-----------------------------------------------------------------------------------*/
extern uint8_t Led_Flag;
extern uint16_t Led_Time;
extern uint16_t Led_P;
static void 
tcpecho_thread(void *arg)
{
  struct netconn *conn, *newconn;
  err_t err;
  LWIP_UNUSED_ARG(arg);

  /* Create a new connection identifier. */
  /* Bind connection to well known port number 7. */
#if LWIP_IPV6
  conn = netconn_new(NETCONN_TCP_IPV6);
  netconn_bind(conn, IP6_ADDR_ANY, 7);
#else /* LWIP_IPV6 */
  conn = netconn_new(NETCONN_TCP);
  netconn_bind(conn, IP_ADDR_ANY, 7);
#endif /* LWIP_IPV6 */
  LWIP_ERROR("tcpecho: invalid conn", (conn != NULL), return;);

  /* Tell connection to go into listening mode. */
  netconn_listen(conn);

  while (1) {

    /* Grab new connection. */
    err = netconn_accept(conn, &newconn);
    /*printf("accepted new connection %p\n", newconn);*/
    /* Process the new connection. */
    if (err == ERR_OK) {
      struct netbuf *buf;
      void *data;
      u16_t len;
      char buff1[]="¿ª\r\n";
			char buff2[]="¹Ø\r\n";
			char buff3[20]="ÉÁ\r\n";
			char buff5[20]="\r\n";
			char buff4[]="\r\non:¿ª\r\noff:¹Ø\r\nflash:ÉÁ\r\nflash=**:ÉÁË¸ÆµÂÊ(100ms)\r\nhelp:°ïÖú\r\n";
      while ((err = netconn_recv(newconn, &buf)) == ERR_OK) {
        /*printf("Recved\n");*/
        do {
					netbuf_data(buf, &data, &len);
					netconn_write(newconn, data, len, NETCONN_COPY);
					*((char*)data+len)=0;
					if(strcmp(data , "on")==0) 
					{
						Led_Flag = 0;
						err = netconn_write(newconn, buff1, sizeof(buff1), NETCONN_COPY);
					}
					else if(strcmp(data , "off")==0)
					{
						Led_Flag = 2;
						err = netconn_write(newconn, buff2, sizeof(buff2), NETCONN_COPY);
					}
					else if(strncmp(data , "flash",5)==0)
					{
						Led_Flag = 1;

						if(strncmp(data,"flash=",6)==0)
							if(atoi((char*)data+6))
							{
							Led_Time = (uint16_t)atoi((char*)data+6)*100;
							}
							sprintf(buff3,"ÉÁË¸ÆµÂÊ:%dms\r\n",Led_Time);
						err = netconn_write(newconn, buff3, sizeof(buff3), NETCONN_COPY);
					}
					else if(strncmp(data , "pwm=",4)==0)
					{
						if((char*)data+4)=="0")
						{
							Led_P = 0;
						}
						else if(atoi((char*)data+4))
							{
							Led_P = atoi((char*)data+4);
							}
							sprintf(buff5,"ÁÁ¶È:%d\r\n",Led_P);
							Led_P*=10;
						err = netconn_write(newconn, buff5, sizeof(buff5), NETCONN_COPY);
					}
					else if(strcmp(data , "help")==0)
					{						
             err = netconn_write(newconn, buff4, sizeof(buff4), NETCONN_COPY);
					}
#if 0
            if (err != ERR_OK) {
              printf("tcpecho: netconn_write: error \"%s\"\n", lwip_strerr(err));
            }
#endif
        } while (netbuf_next(buf) >= 0);
        netbuf_delete(buf);
      }
      /*printf("Got EOF, looping\n");*/ 
      /* Close connection and discard connection identifier. */
      netconn_close(newconn);
      netconn_delete(newconn);
    }
  }
}
/*-----------------------------------------------------------------------------------*/
void
tcpecho_init(void)
{
  sys_thread_new("tcpecho_thread", tcpecho_thread, NULL, DEFAULT_THREAD_STACKSIZE, DEFAULT_THREAD_PRIO);
}
/*-----------------------------------------------------------------------------------*/

#endif /* LWIP_NETCONN */
