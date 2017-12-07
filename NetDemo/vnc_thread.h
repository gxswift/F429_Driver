#ifndef _VNC_THREAD
#define _VNC_THREAD
#include "stm32f4xx_hal.h"


#define VNC_IDLE                   (uint8_t) 0   
#define VNC_INIT                   (uint8_t) 9     
#define VNC_LINK_UP                (uint8_t) 1
#define VNC_WAIT_FOR_ADDRESS       (uint8_t) 2
#define VNC_ADDRESS_ASSIGNED       (uint8_t) 3
#define VNC_ERROR                  (uint8_t) 4
#define VNC_LINK_DOWN              (uint8_t) 5
#define VNC_PROCESS                (uint8_t) 6   
#define VNC_START                  (uint8_t) 7  
#define VNC_CONN_ESTABLISHED       (uint8_t) 8     
   
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void     VNC_SERVER_Start(void);
void     VNC_SERVER_Stop (void);
uint8_t  VNC_GetState(void);
void     VNC_SetState(uint8_t state);
void     VNC_SetLockState(uint8_t LockState);
uint8_t  VNC_GetLockState(void);

int GUI_VNC_X_StartServer(int LayerIndex, int ServerIndex);

#endif
