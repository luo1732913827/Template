#include "uart_parse_task.h"    
#include "FreeRTOS.h"
#include "task.h" 
#include "cmsis_os.h"
#include "queue.h"
#include "elog.h"



QueueHandle_t queue_irq_rec_A = NULL; //

/* USER CODE BEGIN Header_uart_rec_A_function */
/**
* @brief Function implementing the Task_uart_rec_A thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_uart_rec_A_function */
void uart_rec_A_function(void *argument)
{
    /* USER CODE BEGIN uart_rec_A_function */
    static uint32_t receive_data = 0;
    log_i("Task_uart_rec_A is running");
    //创建队列
    queue_irq_rec_A = NULL;
    queue_irq_rec_A = xQueueCreate(1, 4); 
    if(queue_irq_rec_A == NULL)
    {
        log_e("queue_irq_rec_A create failed");
    }else
    {
        log_i("queue_irq_rec_A create success");
    }

		
    /* Infinite loop */
    for(;;)
    {

        xQueueReceive(queue_irq_rec_A, &receive_data, portMAX_DELAY);
        log_i("queue_irq_rec_A receive_data = %x", receive_data);
        osDelay(1);
    }
    /* USER CODE END uart_rec_A_function */
}


