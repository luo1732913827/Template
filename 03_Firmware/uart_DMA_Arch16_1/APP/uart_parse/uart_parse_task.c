#include "uart_parse_task.h"   

#include "FreeRTOS.h"
#include "task.h" 
#include "cmsis_os.h"
#include "queue.h"

#include "elog.h"
#include "mid_circular_buffer.h"

#include "bsp_uart_driver.h"


static circular_buffer_t * g_circular_buffer_from_driver = NULL; //接收数据缓冲区
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

		g_circular_buffer_from_driver = get_circular_buffer();	
    /* Infinite loop */
    for(;;)
    {
				xQueueReceive(queue_irq_rec_A,&receive_data,0xffff);
				log_i("receive_data = [%x]",receive_data);
			
        if(g_circular_buffer_from_driver == NULL)
        {
            log_e("get_circular_buffer failed");    
            return;
        }
        while(0x00 != buffer_is_empty(g_circular_buffer_from_driver))
        {
            uint8_t  temp_data      = 0;
            static  uint8_t  data_counter   = 0;
            static  uint8_t  temp_data_array[20] = {0}; 
            static  uint32_t stateus = FRAME_NOT_DETECTED;
            //获取打印数据
            if(0x00 == get_data(g_circular_buffer_from_driver, &temp_data))
            {
                //log_i("temp_data = %x", temp_data);
            }
						
            //TODO: 解析数据帧
            switch (stateus)
            {
            case FRAME_NOT_DETECTED:
                if(temp_data == FRAME_HEAD_FLAG)
                {
                    stateus = FRAME_HEAD;
                    log_i("Data packet start");
                }
                break;
            case FRAME_HEAD:
                if(temp_data == FRAME_END_FLAG)
                {
                    stateus = FRAME_NOT_DETECTED;
									
                    uint32_t data_sum_temp = 0;
                    uint32_t data_sum = temp_data_array[data_counter-1];
                    
										log_i("data_sum = %d", data_sum);  
									
                    // 计算校验和
                    for(int i = 0; i < (data_counter-1); i++)
										{
                        data_sum_temp += temp_data_array[i];
                    }
										
										log_i("calculated data_sum = [%d]",data_sum_temp);
                    //
										if(data_sum_temp == data_sum)
                    {
                        for(uint8_t i = 0; i < (data_counter-1); i++)
                        {
                            log_i("calculated receive data = [%d]", temp_data_array[i]);  
                        }                        
                    }
                    //       
										for(int i = 0; i < (data_counter-1); i++)
										{
												temp_data_array[data_counter]=0x00;
										}
										data_counter = 0;
                }
                else 
                {
                    log_i("Data packet data:[%d]", temp_data);
                    //数据解析
                    temp_data_array[data_counter] = temp_data;
                    data_counter++;
                }
                break;                          
            default:
                break;
            }

            osDelay(1);
        }

        osDelay(1);
    }
    /* USER CODE END uart_rec_A_function */
}


