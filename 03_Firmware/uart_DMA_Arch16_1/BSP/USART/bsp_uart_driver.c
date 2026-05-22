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
    ret = HAL_UARTEx_ReceiveToIdle_DMA(&huart1, g_circular_buffer_irq_thread->data, CIRCULAR_BUFFER_SIZE);
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

void dma_half_irq_callback(uint32_t number_of_data)
{
    //1.获取当前位置
    uint32_t head_pos = 0;
    uint8_t ret = 0;
    ret = get_head_pos(g_circular_buffer_irq_thread, &head_pos);  
    if(ret != 0x00)
    {
        log_e("get_head_pos failed");
        return;
    }
    //2.获取进入半满中断时,数据已经到达的位置
    uint32_t current_data_pos = (CIRCULAR_BUFFER_SIZE/2)-1;
    //3.对 heap进行取余数
    uint32_t pos_in_buffer = head_pos % (CIRCULAR_BUFFER_SIZE/2);
    //4.计算当前应该偏移的数量
    uint32_t move_pos = current_data_pos - pos_in_buffer;
    //5.更新 head位置
    head_pos_increment(g_circular_buffer_irq_thread, move_pos);
    //test head_pos
    uint32_t test_head_pos = 0;
    ret = get_head_pos(g_circular_buffer_irq_thread, &test_head_pos);
    if(ret != 0x00)
    {
        log_e("get_head_pos failed");
        return;
    }
    log_i("[half]test_head_pos = [%d]", test_head_pos);


}
void dma_comp_irq_callback(uint32_t number_of_data)
{
    //1.获取当前位置
    uint32_t head_pos = 0;
    uint8_t ret = 0;
    ret = get_head_pos(g_circular_buffer_irq_thread, &head_pos);  
    if(ret != 0x00)
    {
        log_e("get_head_pos failed");
        return;
    }
    //2.获取进入全满中断时,数据已经到达的位置
    uint32_t current_data_pos = CIRCULAR_BUFFER_SIZE - 1;
    //3.对 heap进行取余数
    uint32_t pos_in_buffer = head_pos % (CIRCULAR_BUFFER_SIZE);
    //4.计算当前应该偏移的数量
    uint32_t move_pos = current_data_pos - pos_in_buffer;
    //5.更新 head位置
    head_pos_increment(g_circular_buffer_irq_thread, move_pos);
    //test head_pos
    uint32_t test_head_pos = 0;
    ret = get_head_pos(g_circular_buffer_irq_thread, &test_head_pos);
    if(ret != 0x00)
    {
        log_e("get_head_pos failed");
        return;
    }
    log_i("[comp]test_head_pos = [%d]", test_head_pos);

}  
void uart_idle_irq_callback(uint32_t number_of_data)
{
    //1.获取当前位置
    uint32_t head_pos = 0;
    uint8_t ret = 0;
    ret = get_head_pos(g_circular_buffer_irq_thread, &head_pos);  
    if(ret != 0x00)
    {
        log_e("get_head_pos failed");
        return;
    }
    //2.获取进入全满中断时,数据已经到达的位置
    uint32_t current_data_pos = number_of_data - 1;
    //3.对 heap进行取余数
    uint32_t pos_in_buffer = head_pos % (CIRCULAR_BUFFER_SIZE);
    //4.计算当前应该偏移的数量
    uint32_t move_pos = current_data_pos - pos_in_buffer;
    //5.更新 head位置
    head_pos_increment(g_circular_buffer_irq_thread, move_pos);
    //test head_pos
    uint32_t test_head_pos = 0;
    ret = get_head_pos(g_circular_buffer_irq_thread, &test_head_pos);
    if(ret != 0x00)
    {
        log_e("get_head_pos failed");
        return;
    }
    log_i("[idle]test_head_pos = [%d]", test_head_pos);    
}
