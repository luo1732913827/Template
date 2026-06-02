#ifndef __BSP_LED_DRIVE_H__
#define __BSP_LED_DRIVE_H__

#include <stdint.h>
#include <stdio.h>

#define OS_SUPPORTING   1
#define DEBUG           1
#define DEBUGOUT(X)    printf(X)

#define INITED          1
#define NO_INITED       0
//  LED状态枚举
typedef enum
{
    LED_OK              = 0,
    LED_ERROR           = 1,
    LED_ERRORTITIMEOUT  = 2, 
    LED_ERRORRESOURCE   = 3, 
    LED_ERRORPARAMETER  = 4, 
    LED_ERRORNOMEMORY   = 5,
    LED_ERRORISR        = 6,    
    LED_RESERVED        = 0XFF,
}led_status_t;
// LED闪烁频率枚举
typedef enum
{
    PROPORTION_1_3 = 0,
    PROPORTION_1_2 = 1,
    PROPORTION_1_1 = 2,
	PROPORTOON_X_X =0XFF,
}proportion_t;
// LED操作接口
typedef struct
{
    led_status_t (*pf_led_off)  (void);
    led_status_t (*pf_led_on)   (void);
    led_status_t (*pf_led_flip) (void);
}led_operations_t;
// 时间基准接口
typedef struct
{
    led_status_t (*pf_get_time_ms)(uint32_t * const);
}time_base_ms_t;
#if  OS_SUPPORTING                           
// OS延时接口
typedef struct
{
    led_status_t (*pf_os_delay_ms)( uint32_t );
}os_delay_t;
#endif   
typedef led_status_t (*pf_led_control)( uint32_t ,
                                        uint32_t ,
                                        proportion_t );

typedef struct
{
    /***********初始化状态**************/
    uint8_t             init_status;
    /***********目标功能**************/
    /*  闪烁周期 */ 
    uint32_t            cycle_time;
    /*  闪烁次数 */
    uint32_t            blink_time;  
    /*  闪烁状态 */ 
    proportion_t        proportion_on_off;
    /************目标IO需求*************/
    /*  LED操作接口 */
    led_operations_t    *const p_led_opes_inst;
    /*  时间基准接口 */
    time_base_ms_t      *p_time_base_ms;
    /************ OS需求*************/
    /*  OS延时接口 */
#if  OS_SUPPORTING                           
    os_delay_t          *p_os_time_delay;
#endif      
    /************   LED控制接口 *************/
    pf_led_control      pf_led_control;

   }bsp_led_drive_t;


led_status_t led_drive_init( bsp_led_drive_t    *const self,
                             led_operations_t   *const led_ops,
                             time_base_ms_t     *const time_base,
#if  OS_SUPPORTING                           
                             os_delay_t         *const os_delay
#endif                                                         
                             );








#endif // __BSP_LED_DRIVE_H__
