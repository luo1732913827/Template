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
    //0.创建接收数据缓冲区
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
    //开启串口空闲中断接收+DMA半满全满中断
    HAL_StatusTypeDef ret = HAL_OK;
    ret = HAL_UARTEx_ReceiveToIdle_DMA(&huart1, g_circular_buffer_irq_thread->data, 10);
	if(ret != HAL_OK)
	{
	    log_i("HAL_UARTEx_ReceiveToIdle_DMA failed");
	}        
    log_i("HAL_UARTEx_ReceiveToIdle_DMA success");  
    /* Infinite loop */
    for(;;)
    {
        //接收中断里的触发条件
        xQueueReceive(queue_irq_thread, &receive_data, portMAX_DELAY);
        log_i(" front recive data  from irq= [%x]", receive_data);    

        if(IRQ_SEND_TO_THREAD == receive_data)
        {

            uint32_t send_to_end = FRONT_SEND_TO_THREAD;
            BaseType_t ret_queue = pdTRUE;
            //发送通知解包队列开始解包
            ret_queue = xQueueGenericSend(queue_irq_rec_A, &send_to_end,0, queueOVERWRITE);
            if(ret_queue != pdTRUE)
            {   
                log_e("send_to_end failed");
            }
            log_a("front to end send success");
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

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size) {
    log_d("HAL_UARTEx_RxEventCallback is running");
 

}


