#include "bsp_uart_driver.h" 

#include "FreeRTOS.h"
#include "task.h" 
#include "cmsis_os.h"
#include "queue.h"
#include "usart.h"

#include "elog.h" 
#include "mid_circular_buffer.h"


static circular_buffer_t * g_circular_buffer_irq_thread = NULL; //接收数据缓冲区
static QueueHandle_t queue_irq_thread = NULL; //接收队列
extern QueueHandle_t queue_irq_rec_A; //外部的接收队列

#if 1 // circular buffer
uint8_t g_data_buffer = 0;
#define IRQ_SEND_TO_THREAD   0XA1A2A3A4
#define FRONT_SEND_TO_THREAD 0XB1B2B3B4
#endif // end of circular buffer



#if 0   //双缓冲区
#define FLAG_A 0
#define FLAG_B 1
uint8_t flag_AB = FLAG_A; //接收数据缓冲区
uint8_t g_data_buffer_A[1] = {0x00}; //接收数据缓冲区
uint8_t g_data_buffer_B[1] = {0x00}; //接收数据缓冲区
#endif

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
    g_circular_buffer_irq_thread = createEmptyBuffer();
    if(g_circular_buffer_irq_thread == NULL)
    {
        log_e("createEmptyBuffer is g_circular_buffer_irq_thread malloc failed");
        return;
    }
    uint8_t ret1 = insert_data(g_circular_buffer_irq_thread, g_data_buffer);
    //创建接收队列
    queue_irq_thread = xQueueCreate(1, 4);
    if(queue_irq_thread == NULL)
    {
        log_e("xQueueCreate is queue_irq_thread malloc failed");
        return;
    }
		log_i("queue_irq_thread is running");
		HAL_StatusTypeDef ret = HAL_OK;
		ret = HAL_UART_Receive_IT(&huart1, &g_data_buffer, 1);
		if(ret != HAL_OK)
		{
		log_i("HAL_UART_Receive_IT failed");
		}        
#if 0    
    //判断buffer A是否为空
		uint8_t buffer_empty = buffer_is_empty(g_circular_buffer_irq_thread );
		log_i("buffer_empty = %d", buffer_empty);
	//判断buffer A是否已满
        uint8_t buffer_full = buffer_is_full(p_buffer_A);
        log_i("buffer_full = %d", buffer_full);
    //读取A数据个数
        uint8_t buffer_count = buffer_get_count(p_buffer_A);
        log_i("buffer_count = %d", buffer_count);
    //插入数据到buffer A
        uint8_t insert_ret = insert_data(p_buffer_A, 0x55);
        log_i("insert_ret = %d", p_buffer_A->tail);
    //从buffer A获取数据
        data_type_t data_get;           
        uint8_t get_ret = get_data(p_buffer_A, &data_get);  
        log_i("get_ret = %d", p_buffer_A->head);
        HAL_StatusTypeDef ret = HAL_OK;
        ret = HAL_UART_Receive_IT(&huart1, g_data_buffer_A, 1);
		if(ret != HAL_OK)
		{
			log_i("HAL_UART_Receive_IT failed");
		}    
#endif



    /* Infinite loop */
    for(;;)
    {
        xQueueReceive(queue_irq_thread, &receive_data, portMAX_DELAY);
        log_i(" receive_data = %x", receive_data);    
        if(IRQ_SEND_TO_THREAD == receive_data)
        {
            uint32_t send_to_end = FRONT_SEND_TO_THREAD;
            BaseType_t ret_queue = pdTRUE;
            ret_queue = xQueueSend(queue_irq_rec_A, &send_to_end, 0);
            if(ret_queue != pdTRUE)
            {   
                    log_e("HAL_UART_RxCpltCallback failed");
            }
            log_a("send_to_end success");
        }
        osDelay(1);
    }
    /* USER CODE END uart_rec_A_function */
}

	
/* USER CODE BEGIN Application */
/**
  * @brief  Rx Transfer completed callbacks.
  * @param  huart  Pointer to a UART_HandleTypeDef structure that contains
  *                the configuration information for the specified UART module.
  * @retval None
  */

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
#if 0
		log_d("HAL_UART_RxCpltCallback is running");
        BaseType_t        queue_ret = pdFALSE;
        HAL_StatusTypeDef uart_ret  = HAL_OK;
        //buffer A 接收数据切换到 buffer B 接收数据
        if(flag_AB == FLAG_A)
        {

                flag_AB = FLAG_B;    
                // queue_ret = xQueueSendFromISR(queue_irq_rec_A, &g_data_buffer_A[0], NULL);
                // if(queue_ret != pdPASS)
                // {
                //         log_e("HAL_UART_RxCpltCallback failed");
                // }
                //切换buffer B 接收数据        
                uart_ret = HAL_UART_Receive_IT(&huart1, g_data_buffer_B, 1);
                if(uart_ret != HAL_OK)
                {
                        log_i("HAL_UART_Receive_IT receive B failed");
                }else
                {
                        log_i("g_data_buffer_A = [%c]", g_data_buffer_A[0]);
                }
        }//buffer B 接收数据切换到 buffer A 接收数据
        else
        {
                flag_AB = FLAG_A;
                // queue_ret = xQueueSendFromISR(queue_irq_rec_A, &g_data_buffer_B[0], NULL);
                // if(queue_ret != pdPASS)
                // {
                //     log_e("HAL_UART_RxCpltCallback failed");
                // }
                //切换buffer A 接收数据          
                uart_ret = HAL_UART_Receive_IT(&huart1, g_data_buffer_A, 1);
                if(uart_ret != HAL_OK)
                {
                    log_i("HAL_UART_Receive_IT receive A failed");
                }else
                {
                    log_i("g_data_buffer_B = [%c]", g_data_buffer_B[0]);
                }

        }
#endif

#if 1
		log_d("HAL_UART_RxCpltCallback is running");
        uint8_t ret = insert_data(g_circular_buffer_irq_thread, g_data_buffer);
		BaseType_t        queue_ret = pdFALSE;
		uint32_t send_to_thread  = IRQ_SEND_TO_THREAD;
		//发送数据到队列
		queue_ret = xQueueSendFromISR(queue_irq_thread, &send_to_thread, NULL);
		if(queue_ret != pdPASS)
		{   
				log_e("HAL_UART_RxCpltCallback failed");
		}
        //开启下一次接收中断
        HAL_UART_Receive_IT(&huart1, &g_data_buffer, 1);    

#endif        
		
		




}
