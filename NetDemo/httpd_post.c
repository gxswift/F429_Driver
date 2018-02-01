
err_t httpd_post_begin(void *connection, const char *uri, const char *
http_request,u16_t http_request_len, int content_len, char *
response_uri,u16_t response_uri_len, u8_t *post_auto_wnd)
{
#if LWIP_HTTPD_CGI
  int i = 0;
#endif
struct http_state *hs = (struct http_state *)connection;
 if(!uri || (uri[0] == '\0')) {
    return ERR_ARG;
 }
#if LWIP_HTTPD_CGI
  if (g_iNumCGIs && g_pCGIs) {
    for (i = 0; i < g_iNumCGIs; i++) {
      if (strcmp(uri, g_pCGIs[i].pcCGIName) == 0) {
	  	response_uri = g_pCGIs[i].pfnCGIHandler(i, http_cgi_paramcount, hs->params,
                                          hs->param_vals);
		response_uri_len = strlen(response_uri);
         break;
       }
    }
  }
  if(i == g_iNumCGIs) {
    return ERR_ARG; // 未找到CGI handler 
  }
#endif
  return ERR_OK;
}




#define LWIP_HTTPD_POST_MAX_PAYLOAD_LEN     512
static char http_post_payload[LWIP_HTTPD_POST_MAX_PAYLOAD_LEN];
static u16_t http_post_payload_len = 0;

err_t httpd_post_receive_data(void *connection, struct pbuf *p)
{
    struct http_state *hs = (struct http_state *)connection;
    struct pbuf *q = p;
    int count;
    u32_t http_post_payload_full_flag = 0;
    while(q != NULL)  // 缓存接收的数据至http_post_payload
    {
      if(http_post_payload_len + q->len <= LWIP_HTTPD_POST_MAX_PAYLOAD_LEN) {
          MEMCPY(http_post_payload+http_post_payload_len, q->payload, q->len);
          http_post_payload_len += q->len;
      }
      else {  // 缓存溢出 置溢出标志位
        http_post_payload_full_flag = 1;
        break;
      }
      q = q->next;
    }
    pbuf_free(p); // 释放pbuf
    if(http_post_payload_full_flag) // 缓存溢出 则丢弃数据
    {
        http_post_payload_full_flag = 0;
        http_post_payload_len = 0;
    }
    else if(hs->post_content_len_left == 0) {  // POST数据已经接收完毕 则处理
            count = extract_uri_parameters(hs, http_post_payload);  // 解析
            http_post_payload_len = 0;
        }
    }
    return ERR_OK;
}



void httpd_post_finished(void *connection, char *response_uri, u16_t response_uri_len)
{
    struct http_state *hs = (struct http_state *)connection;
 	strncpy(response_uri, "/index.html",response_uri_len);
}

//--------------------------------------------------------------
struct fs_file {
  char *data;
  int len;
};
#ifdef USE_IAP_HTTP

static __IO uint32_t DataFlag=0;
static __IO uint32_t size =0;
static __IO uint32_t FlashWriteAddress;
static uint32_t TotalReceived=0;
static char LeftBytesTab[4];
static uint8_t LeftBytes=0;
static __IO uint8_t resetpage=0;
static uint32_t ContentLengthOffset =0,BrowserFlag=0;
static __IO uint32_t TotalData=0, checklogin=0; 
struct http_state
{
  char *file;
  u32_t left;
};

typedef enum 
{
  LoginPage = 0,
  FileUploadPage,
  UploadDonePage,
  ResetDonePage
}htmlpageState;
  
htmlpageState htmlpage;
 
static const char http_crnl_2[4] = 
/* "\r\n--" */
{0xd, 0xa,0x2d,0x2d};
static const char octet_stream[14] = 
/* "octet-stream" */
{0x6f, 0x63, 0x74, 0x65, 0x74, 0x2d, 0x73, 0x74, 0x72, 0x65, 0x61, 0x6d,0x0d, };
static const char Content_Length[17] = 
/* Content Length */
{0x43, 0x6f, 0x6e, 0x74, 0x65, 0x6e, 0x74, 0x2d, 0x4c, 0x65, 0x6e, 0x67,0x74, 0x68, 0x3a, 0x20, };


static uint32_t Parse_Content_Length(char *data, uint32_t len);
static void IAP_HTTP_writedata(char* data, uint32_t len);

/* file must be allocated by caller and will be filled in
   by the function. */
static int fs_open(char *name, struct fs_file *file);



/**
  * @brief callback function for handling TCP HTTP traffic
  * @param arg: pointer to an argument structure to be passed to callback function
  * @param pcb: pointer to a tcp_pcb structure
  * @param p: pointer to a packet buffer
  * @param err: LwIP error code
  * @retval err
  */
static err_t http_recv(void *arg, struct tcp_pcb *pcb,  struct pbuf *p, err_t err)
{
  int32_t i,len=0;
  uint32_t DataOffset, FilenameOffset;
  char *data, *ptr, filename[40], login[LOGIN_SIZE];
  struct fs_file file = {0, 0};
  struct http_state *hs;

#ifdef USE_LCD
  char message[46];
#endif
  
  hs = arg;

  if (err == ERR_OK && p != NULL)
  {
    /* Inform TCP that we have taken the data */
    tcp_recved(pcb, p->tot_len);
    
    if (hs->file == NULL)
    {
      data = p->payload;
      len = p->tot_len;
      if (((strncmp(data, "POST /upload.cgi",16)==0)||(DataFlag >=1))&&(htmlpage == FileUploadPage))
      { 
        DataOffset =0;
        
        /* POST Packet received */
        if (DataFlag ==0)
        { 
          BrowserFlag=0;
          TotalReceived =0;
          
          /* parse packet for Content-length field */
          size = Parse_Content_Length(data, (uint32_t)(p->tot_len));//解析出文件长度
           
          /* parse packet for the octet-stream field */
          for (i=0;i<len;i++)
          {
             if (strncmp ((char*)(data+i), octet_stream, 13)==0)
             {
               DataOffset = i+16;
               break;
             }
          }  
          /* case of MSIE8 : we do not receive data in the POST packet */ 
          if (DataOffset==0)
          {
             DataFlag++;
             BrowserFlag = 1;
             pbuf_free(p);
             return ERR_OK;
             
          }
          /* case of Mozilla Firefox : we receive data in the POST packet*/
          else
          {
            TotalReceived = len - (ContentLengthOffset + 4);
          }
        }
        
        if (((DataFlag ==1)&&(BrowserFlag==1)) || ((DataFlag ==0)&&(BrowserFlag==0)))
        { 
           if ((DataFlag ==0)&&(BrowserFlag==0)) 
           {
             DataFlag++;
           }
           else if ((DataFlag ==1)&&(BrowserFlag==1))
           {
             /* parse packet for the octet-stream field */
             for (i=0;i<len;i++)
             {
               if (strncmp ((char*)(data+i), octet_stream, 13)==0)
               {
                 DataOffset = i+16;
                 break;
               }
             }
             TotalReceived+=len;
             DataFlag++;
           }  
                
           /* parse packet for the filename field */
           FilenameOffset = 0;
           for (i=0;i<len;i++)
           {
             if (strncmp ((char*)(data+i), "filename=", 9)==0)
             {
                FilenameOffset = i+10;
                break;
             }
           }           
           i =0;
           if (FilenameOffset)
           {
             while((*(data+FilenameOffset + i)!=0x22 )&&(i < 40))
             {
               filename[i] = *(data+FilenameOffset + i);//解析出文件名
               i++;
             }
             filename[i] = 0x0;
           }
           
           if (i==0)//没有文件名
           {
             htmlpage = FileUploadPage;
             /* no filename, in this case reload upload page */
             fs_open("/upload.html", &file);
             hs->file = file.data;
             hs->left = file.len;
             pbuf_free(p);
             
             /* send index.html page */ 
             send_data(pcb, hs);
          
             /* Tell TCP that we wish be to informed of data that has been
             successfully sent by a call to the http_sent() function. */
             tcp_sent(pcb, http_sent); 
             DataFlag=0;
             return ERR_OK;
             
           }
          
           TotalData =0;
           /* init flash */
           FLASH_If_Init();
           /* erase user flash area */
           FLASH_If_Erase(USER_FLASH_FIRST_PAGE_ADDRESS);
          
           FlashWriteAddress = USER_FLASH_FIRST_PAGE_ADDRESS;
         }
         /* DataFlag >1 => the packet is data only  */
         else 
         {
           TotalReceived +=len;
         }
        
         ptr = (char*)(data + DataOffset);
         len-= DataOffset;
        
         /* update Total data received counter */
         TotalData +=len;
        
         /* check if last data packet */
         if (TotalReceived == size)
         {
           /* if last packet need to remove the http boundary tag */
           /* parse packet for "\r\n--" starting from end of data */
           i=4; 
           while (strncmp ((char*)(data+ p->tot_len -i),http_crnl_2 , 4) && (p->tot_len -i > 0))
           {
             i++;
           }
           len-=i;
           TotalData-=i;
          
           /* write data in Flash */
           if (len)
           IAP_HTTP_writedata(ptr,len);
          
           DataFlag=0;
          htmlpage = UploadDonePage;
          /* send uploaddone.html page */
          fs_open("/uploaddone.html", &file);
          hs->file = file.data;
          hs->left = file.len;
          send_data(pcb, hs);
          
          /* Tell TCP that we wish be to informed of data that has been
           successfully sent by a call to the http_sent() function. */
          tcp_sent(pcb, http_sent);  
        }
        /* not last data packet */
        else
        {
          /* write data in flash */
          if(len)
          IAP_HTTP_writedata(ptr,len);
        }
        pbuf_free(p);
      }
      else
      {

       /* Bad HTTP requests */
        close_conn(pcb, hs);
      }
    }
    else
    {
      pbuf_free(p);
      close_conn(pcb,hs);
    }
    
  }
  if (err == ERR_OK && p == NULL)
  {
    /* received empty frame */
    close_conn(pcb, hs);
  }

  return ERR_OK;
}

/**
  * @brief  callback function on TCP connection setup ( on port 80)
  * @param  arg: pointer to an argument structure to be passed to callback function
  * @param  pcb: pointer to a tcp_pcb structure
  * &param  err: Lwip stack error code
  * @retval err
  */
static err_t http_accept(void *arg, struct tcp_pcb *pcb, err_t err)
{
  struct http_state *hs;

  /* Allocate memory for the structure that holds the state of the connection */
  hs = mem_malloc(sizeof(struct http_state));

  if (hs == NULL)
  {
    return ERR_MEM;
  }

  /* Initialize the structure. */
  hs->file = NULL;
  hs->left = 0;

  /* Tell TCP that this is the structure we wish to be passed for our
     callbacks. */
  tcp_arg(pcb, hs);

  /* Tell TCP that we wish to be informed of incoming data by a call
     to the http_recv() function. */
  tcp_recv(pcb, http_recv);

  tcp_err(pcb, conn_err);

  tcp_poll(pcb, http_poll, 10);
  return ERR_OK;
}

/**
  * @brief  intialize HTTP webserver  
  * @param  none
  * @retval None
  */
void IAP_httpd_init(void)
{
  struct tcp_pcb *pcb;
  /*create new pcb*/
  pcb = tcp_new();
  /* bind HTTP traffic to pcb */
  tcp_bind(pcb, IP_ADDR_ANY, 80);
  /* start listening on port 80 */
  pcb = tcp_listen(pcb);
  /* define callback function for TCP connection setup */
  tcp_accept(pcb, http_accept);
}

/**
  * @brief  Opens a file defined in fsdata.c ROM filesystem 
  * @param  name : pointer to a file name
  * @param  file : pointer to a fs_file structure  
  * @retval  1 if success, 0 if fail
  */
static int fs_open(char *name, struct fs_file *file)
{
  struct fsdata_file_noconst *f;

  for (f = (struct fsdata_file_noconst *)FS_ROOT; f != NULL; f = (struct fsdata_file_noconst *)f->next)
  {
    if (!strcmp(name, f->name))
    {
      file->data = f->data;
      file->len = f->len;
      return 1;
    }
  }
  return 0;
}

/**
  * @brief  Extract the Content_Length data from HTML data  
  * @param  data : pointer on receive packet buffer 
  * @param  len  : buffer length  
  * @retval size : Content_length in numeric format
  */
static uint32_t Parse_Content_Length(char *data, uint32_t len)
{
  uint32_t i=0,size=0, S=1;
  int32_t j=0;
  char sizestring[6], *ptr;
   
  ContentLengthOffset =0;
  
  /* find Content-Length data in packet buffer */
  for (i=0;i<len;i++)
  {
    if (strncmp ((char*)(data+i), Content_Length, 16)==0)
    {
      ContentLengthOffset = i+16;
      break;
    }
  }
  /* read Content-Length value */
  if (ContentLengthOffset)
  {
    i=0;
    ptr = (char*)(data + ContentLengthOffset);
    while(*(ptr+i)!=0x0d)
    {
      sizestring[i] = *(ptr+i);
      i++;
      ContentLengthOffset++; 
    }
    if (i>0)
    {
      /* transform string data into numeric format */
      for(j=i-1;j>=0;j--)
      {
        size += (sizestring[j]-'0')*S;
        S=S*10;
      }
    }
  }
  return size;
}

/**
  * @brief  writes received data in flash    
  * @param  ptr: data pointer
  * @param  len: data length
  * @retval None 
  */
static void IAP_HTTP_writedata(char * ptr, uint32_t len)            
{
  uint32_t count, i=0, j=0;
  
  /* check if any left bytes from previous packet transfer*/
  /* if it is the case do a concat with new data to create a 32-bit word */
  if (LeftBytes)
  {
    while(LeftBytes<=3)
    {
      if(len>(j+1))
      {
        LeftBytesTab[LeftBytes++] = *(ptr+j);
      }
      else
      {
        LeftBytesTab[LeftBytes++] = 0xFF;
      }
      j++;
    }
    FLASH_If_Write(&FlashWriteAddress, (uint32_t*)(LeftBytesTab),1);
    LeftBytes =0;
    
    /* update data pointer */
    ptr = (char*)(ptr+j);
    len = len -j;
  }
  
  /* write received bytes into flash */
  count = len/4;
  
  /* check if remaining bytes < 4 */
  i= len%4;
  if (i>0)
  {
    if (TotalReceived != size)
    {
      /* store bytes in LeftBytesTab */
      LeftBytes=0;
      for(;i>0;i--)
      LeftBytesTab[LeftBytes++] = *(char*)(ptr+ len-i);  
    }
    else count++;
  }
  FLASH_If_Write(&FlashWriteAddress, (uint32_t*)ptr ,count);
}
#endif


