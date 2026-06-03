#include "bsp_led_drive.h"

led_status_t led_drive_init(bsp_led_drive_t    *const self)
{
    if( self == NULL )
    {   
#if DEBUG
        DEBUGOUT("LED init params error\n");
#endif             
        return LED_ERRORRESOURCE;
    }       
#if DEBUG
        DEBUGOUT("LED init start\n");
#endif
// 2.初始化LED驱动      
    self->p_led_opes_inst->pf_led_off();

}
/**
 * @brief LED驱动初始化函数
 * 
 * 该函数用于初始化LED驱动模块，配置LED操作接口、时间基准以及可选的操作系统延时接口。
 * 初始化过程中会对所有必要参数进行空指针检查，确保资源有效性。
 *
 * @param self         LED驱动实例指针，指向待初始化的LED驱动对象
 * @param led_ops      LED操作接口指针，包含LED的底层操作函数（如点亮、熄灭等）
 * @param time_base    时间基准接口指针，提供毫秒级时间戳获取能力
 * @param os_delay     操作系统延时接口指针（仅在OS_SUPPORTING启用时有效）
 *
 * @return led_status_t 初始化状态
 * @retval LED_ERRORRESOURCE  参数为空，资源无效
 * @retval LED_OK             初始化成功（函数体待补充完整实现）
 */
led_status_t led_drive_inst( bsp_led_drive_t    *const self,
                             led_operations_t   *const led_ops,
                             time_base_ms_t     *const time_base,
#if  OS_SUPPORTING                           
                             os_delay_t         *const os_delay
#endif                                                         
                             )
{
    // 0.参数有效性检查
    if( self== NULL || led_ops == NULL || time_base == NULL )
    {
#if DEBUG
        DEBUGOUT("LED init params error\n");
#endif
        return LED_ERRORRESOURCE;
    }
#if  OS_SUPPORTING      
    if( os_delay == NULL )  
    {
#if DEBUG
        DEBUGOUT("LED os_delay error\n");
#endif
        return LED_ERRORRESOURCE;
    }   
#endif 
    // 1.检查初始化状态是否已初始化
    if( self->init_status == INITED )
    {
#if DEBUG
        DEBUGOUT("LED init_status \n");
#endif
        return LED_ERRORRESOURCE;
    }  
    // 2.初始化LED驱动      
#if DEBUG
        DEBUGOUT("LED inst start\n");
#endif   
    self->p_led_opes_inst   = led_ops;
    self->p_time_base_ms    = time_base;
    self->p_os_time_delay   = os_delay;
    // 3.初始化LED控制接口
    self->cycle_time        = 0;
    self->blink_time        = 0;
    self->proportion_on_off = PROPORTOON_X_X;


}

