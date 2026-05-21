#ifndef __MID_CIRCULAR_BUFFER_H__
#define __MID_CIRCULAR_BUFFER_H__

#include <stdint.h>

#define CIRCULAR_BUFFER_SIZE 100
typedef uint8_t data_type_t;
typedef struct {
    data_type_t     data[CIRCULAR_BUFFER_SIZE];
    uint16_t        head;
    uint16_t        tail;
} circular_buffer_t;

circular_buffer_t * createEmptyBuffer (void);
uint8_t buffer_is_empty (circular_buffer_t * p_buffer);
uint8_t buffer_is_full (circular_buffer_t * p_buffer);
uint8_t buffer_get_count (circular_buffer_t * p_buffer);
uint8_t insert_data (circular_buffer_t * p_buffer ,data_type_t data);
uint8_t get_data (circular_buffer_t * p_buffer ,data_type_t * data);

#endif /* __MID_CIRCULAR_BUFFER_H__ */
