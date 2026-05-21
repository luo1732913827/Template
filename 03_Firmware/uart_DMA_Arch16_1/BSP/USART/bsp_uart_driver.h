 #ifndef __BSP_USART_H__
#define __BSP_USART_H__

#include <stdint.h>
#include "FreeRTOS.h"
#include "task.h" 
#include "cmsis_os.h"
#include "queue.h"
#include "usart.h"

#include "elog.h" 
#include "mid_circular_buffer.h"


void bsp_uart_driver_init(void);
circular_buffer_t * get_circular_buffer(void);


#endif /* __BSP_USART_H__ */
