#include "mid_circular_buffer.h"

#include "FreeRTOS.h"
#include "task.h" 
#include "cmsis_os.h"

#include "elog.h"


circular_buffer_t * createEmptyBuffer   (void)
{
    circular_buffer_t * p_buffer_temp = (circular_buffer_t *)malloc(sizeof(circular_buffer_t));
    if(p_buffer_temp == NULL)
    {
        log_e("createEmptyBuffer is p_buffer malloc failed");
        return NULL;
    }
    memset(p_buffer_temp, 0, sizeof(circular_buffer_t));

    return p_buffer_temp;
}
//判断buffer是否为空,
//返回值:0xFF表示buffer指针为NULL,
//       0x00表示buffer为空,
//       0x01表示buffer不为空
uint8_t buffer_is_empty(circular_buffer_t * p_buffer)
{
    if(p_buffer == NULL)
    {
        log_e("buffer_is_empty is p_buffer is NULL");
        return 0xFF;
    }
    if( p_buffer->head == p_buffer->tail )
    {
        return 0x00;
    }
    return 0x01;

}


//判断buffer是否已满,
//返回值:0xFF表示buffer指针为NULL,
//       0x01表示buffer已满,
//       0x00表示buffer未满
uint8_t buffer_is_full (circular_buffer_t * p_buffer)
{
    if(p_buffer == NULL)
    {
        log_e("buffer_is_full is p_buffer is NULL");
        return 0xFF;
    }
    if( (p_buffer->head+1) % CIRCULAR_BUFFER_SIZE == (p_buffer->tail% CIRCULAR_BUFFER_SIZE) )
    {
        return 0x01;
    }
    return 0x00;    

}

//获取buffer中数据个数
//返回值:0xFF表示buffer指针为NULL,
//       0x00表示buffer为空,
//       其他值表示buffer中数据个数 (0-99)
uint8_t buffer_get_count (circular_buffer_t * p_buffer)
{
    if(p_buffer == NULL)
    {
        log_e("buffer_get_count is p_buffer is NULL");
        return 0xFF;
    }
    if( p_buffer->head == p_buffer->tail )
    {
        return 0x00;
    }
    return (p_buffer->tail - p_buffer->head) % CIRCULAR_BUFFER_SIZE;

}
//插入数据到buffer
//返回值:0xFF表示buffer指针为NULL,
//       0xFE表示buffer已满,
//       0x00表示数据插入成功
uint8_t insert_data (circular_buffer_t * p_buffer ,data_type_t data)
{
    if(p_buffer == NULL)
    {
        log_e("insert_data is p_buffer is NULL");
        return 0xFF;
    }
    if( buffer_is_full(p_buffer) == 0x01 )
    {
        log_e("insert_data is buffer is full");
        return 0xFE;
    }
    p_buffer->data[(p_buffer->tail) % CIRCULAR_BUFFER_SIZE] = data;
    p_buffer->tail = (p_buffer->tail + 1) % CIRCULAR_BUFFER_SIZE;
    return 0x00;
}
//从buffer中获取数据
//返回值:0xFF表示buffer指针为NULL,
//       0x01表示buffer为空,
//       0x00表示数据获取成功
uint8_t get_data (circular_buffer_t * p_buffer ,data_type_t * data)
{
    if(p_buffer == NULL)    
    {
        log_e("get_data is p_buffer is NULL");
        return 0xFF;
    }
    if( buffer_is_empty(p_buffer) == 0x00 )
    {
        log_e("get_data is buffer is empty");
        return 0x01;
    }
    *data = p_buffer->data[(p_buffer->head) % CIRCULAR_BUFFER_SIZE];
    p_buffer->head = (p_buffer->head + 1) % CIRCULAR_BUFFER_SIZE;
    return 0x00;
}

