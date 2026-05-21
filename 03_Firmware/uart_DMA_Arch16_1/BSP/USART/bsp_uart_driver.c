#include "bsp_uart_driver.h" 



static circular_buffer_t * g_circular_buffer_irq_thread = NULL; //接收数据缓冲区
static QueueHandle_t queue_irq_thread = NULL; //接收队列
extern QueueHandle_t queue_irq_rec_A; //外部的接收队列

#if 1 // circular buffer
uint8_t g_data_buffer = 0;
#define IRQ_SEND_TO_THREAD   0XA1A2A3A4
#define FRONT_SEND_TO_THREAD 0XB1B2B3B4
#endif // end of circular buffer




/* USER CODE BEGIN Header_uart_rec_A_function */
/**
* @brief Function implementing the Task_uart_rec_A thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_uart_rec_A_function */
void uart_driver_func(void *argument)
{
    /* USER CODE BEGIN uart_rec_A_function */
    log_i("Task_uart_driver is running");
    uint32_t receive_data = 0;
    //创建接收数据缓冲区
    g_circular_buffer_irq_thread = create_empty_circular_buffer();
    if(g_circular_buffer_irq_thread == NULL)
    {
        log_e("create g_circular_buffer_irq_thread malloc failed");
        return;
    }
    uint8_t ret1 = insert_data(g_circular_buffer_irq_thread, g_data_buffer);
    //1.创建接收队列
    queue_irq_thread = xQueueCreate(1, 4);
    if(queue_irq_thread == NULL)
    {
        log_e("xQueueCreate is queue_irq_thread malloc failed");
        return;
    }
		
		HAL_StatusTypeDef ret = HAL_OK;
		ret = HAL_UART_Receive_IT(&huart1, &g_data_buffer, 1);
		if(ret != HAL_OK)
		{
		log_i("HAL_UART_Receive_IT failed");
		}        


    /* Infinite loop */
    for(;;)
    {
        xQueueReceive(queue_irq_thread, &receive_data, portMAX_DELAY);
        log_i(" receive_data = %x", receive_data);    
        if(IRQ_SEND_TO_THREAD == receive_data)
        {
            uint32_t send_to_end = FRONT_SEND_TO_THREAD;
            BaseType_t ret_queue = pdTRUE;
            ret_queue = xQueueGenericSend(queue_irq_rec_A, &send_to_end,0, queueOVERWRITE);
            if(ret_queue != pdTRUE)
            {   
                log_e("send_to_end failed");
            }
            log_a("send_to_end success");
        }
        osDelay(1);
    }
    /* USER CODE END uart_rec_A_function */
}

circular_buffer_t * get_circular_buffer(void)
{
    if( g_circular_buffer_irq_thread == NULL)
    {
        log_e("get_circular_buffer is g_circular_buffer_irq_thread is NULL");
        return NULL;
    }
    return g_circular_buffer_irq_thread;
}




	
/* USER CODE BEGIN Application */
/**
  * @brief  Rx Transfer completed callbacks.
  * @param  huart  Pointer to a UART_HandleTypeDef structure that contains
  *                the configuration information for the specified UART module.
  * @retval None
  */

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {

#if 1
		log_d("HAL_UART_RxCpltCallback is running");
        uint8_t ret = insert_data(g_circular_buffer_irq_thread, g_data_buffer);
		BaseType_t        queue_ret = pdFALSE;
		uint32_t send_to_thread  = IRQ_SEND_TO_THREAD;
		//发送数据到队列
		queue_ret = xQueueGenericSendFromISR(queue_irq_rec_A, &send_to_thread,0, queueOVERWRITE);
		if(queue_ret != pdTRUE)
		{   
				log_e("HAL_UART_RxCpltCallback failed");
		}
        //开启下一次接收中断
        HAL_UART_Receive_IT(&huart1, &g_data_buffer, 1);    

#endif        
		
		




}


