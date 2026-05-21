#ifndef __UART_PARSE_TASK_H__
#define __UART_PARSE_TASK_H__

#include <stdint.h>


#define FRAME_NOT_DETECTED  (0x01)  //未检测到帧
#define FRAME_HEAD          (0x02)  //帧头
#define FRAME_END           (0x03)  //帧尾
#define FRAME_HEAD_FLAG     (0XFE)  //帧头标志位
#define FRAME_END_FLAG      (0XFF)  //帧尾标志位

void uart_rec_A_function(void *argument);

uint8_t buffer1[1] = {0};
uint8_t buffer2[2] = {0};

#endif /* __UART_PARSE_TASK_H__ */
