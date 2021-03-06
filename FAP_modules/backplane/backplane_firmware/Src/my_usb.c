#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "my_usb.h"
#include "usbd_cdc_if.h"

char usb_line_buf[LB_SIZE];
linear_buf usb_recv_buf;
char* usb_data;

void my_usb_init(void)
{
  linear_buf_init(&usb_recv_buf);
}

void my_usb_putchar(uint8_t ch)
{
  uint8_t result = USBD_FAIL;
  while(1)
  {
    result = CDC_Transmit_FS(&ch, 1);
    if(result == USBD_OK || result == USBD_FAIL)
      return;
    if(result == USBD_BUSY)
      continue;
  }
}

char* my_usb_readline(void)
{
  char* ret = NULL;
  if(usb_recv_buf.curr_index > 0)
  {
    if(usb_recv_buf.buf[usb_recv_buf.curr_index - 1] == '\n')
    {
      for (int i = 0; i < usb_recv_buf.curr_index; ++i)
        if(usb_recv_buf.buf[i] == '\n' || usb_recv_buf.buf[i] == '\r')
          usb_recv_buf.buf[i] = 0;
      strcpy(usb_line_buf, usb_recv_buf.buf);
      ret = usb_line_buf;
      linear_buf_reset(&usb_recv_buf);
    }
    else if(HAL_GetTick() - usb_recv_buf.last_recv > 250)
      linear_buf_reset(&usb_recv_buf);
  }
  return ret;
}
