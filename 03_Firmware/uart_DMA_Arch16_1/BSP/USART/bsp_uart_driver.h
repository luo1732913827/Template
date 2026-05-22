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

void dma_half_irq_callback(uint32_t number_of_data);
void dma_comp_irq_callback(uint32_t number_of_data);
void uart_idle_irq_callback(uint32_t number_of_data);

#endif /* __BSP_USART_H__ */
