/******************************************************************************
 * Copyright (C) 2024 EternalChip, Inc.(Gmbh) or its affiliates.
 * 
 * All Rights Reserved.
 * 
 * @file bsp_led_driver.c
 * 
 * @par dependencies 
 * - bsp_led_driver.h
 * 
 * 
 * @author Jack | R&D Dept. | EternalChip ??????????
 * 
 * @brief Provide the HAL APIs of LEDs and corresponding operations.
 * 
 * Processing flow:
 * 
 * call directly.
 * 
 * @version V1.0 2024-10-18
 *
 * @note 1 tab == 4 spaces!
 * 
 *****************************************************************************/

//******************************** Includes *********************************//
#include "bsp_led_driver.h"

//******************************** Includes *********************************//


//******************************** Defines **********************************//
/**
 * @brief init the target of bsp_led_driver_t.
 * 
 * Steps:
 *  1, make the targe at a specific statues;
 *  
 * 
 * @param[in] self      : Pointer to the input data.
 * 
 * @return led_status_t : The status of running
 * 
 * */
led_status_t led_bliank (bsp_led_driver_t *self)
{
    //1check the parameter
    if( NULL == self || NOT_INITED == self->is_inited  )
    {
#ifdef DEBUG
        DEBUG_OUT("LED_ERRORPARAMETER\r\n");
#endif // DEBUG
        return LED_ERRORPARAMETER;
    } 
    //
    uint32_t        cycle_time_local;
    uint32_t        blink_times_local;
    proportion_t    proportion_local;
    uint32_t        led_toggle_time;
    //
    cycle_time_local = self->cycle_time_ms;
    blink_times_local = self->blink_times;
    proportion_local = self->proportion_on_off;
    //
    if(PROPORTIONN_1_3 == proportion_local)
    {
        led_toggle_time = cycle_time_local / 4;
    }
    else if(PROPORTIONN_1_2 == proportion_local)
    {
        led_toggle_time = cycle_time_local / 3;
    }
    else if(PROPORTIONN_1_1 == proportion_local)
    {
        led_toggle_time = cycle_time_local/2 ;
    }
    else
    {
        return LED_ERRORPARAMETER;
    }
    for(int i = 0; i < blink_times_local; i ++)
    {
        for(int j = 0; j < cycle_time_local; j ++)
        {
            if(j < led_toggle_time)
            {
                DEBUG_OUT("led on\r\n");
                //self->p_led_opes_inst->pf_led_on();
            }
            else
            {
                DEBUG_OUT("led off\r\n");
                //self->p_led_opes_inst->pf_led_off();
            }
        }

    }

    return LED_OK;
}

 

/**
 * @brief init the target of bsp_led_driver_t.
 * 
 * Steps:
 *  1, make the targe at a specific statues;
 *  
 * 
 * @param[in] self      : Pointer to the input data.
 * 
 * @return led_status_t : The status of running
 * 
 * */
static led_status_t led_control(
                                bsp_led_driver_t * const      self, 
                                uint32_t Cycle_time,     // Cycle_time[ms]
                                uint32_t blink_times,    // blink_times[times]
                                proportion_t proportion  //  proportion_on_off
                                                  )
{
    // 1check the parameter
    if( NULL == self || NOT_INITED == self->is_inited  )
    {
#ifdef DEBUG
        DEBUG_OUT("LED_ERRORPARAMETER\r\n");
#endif // DEBUG
        return LED_ERRORPARAMETER;
    }   
    // 2check the parameter
    if(
        !(
            (Cycle_time < 010000)                   &&
            (blink_times < 01000)                   &&
            (PROPORTIONN_1_3 <= proportion)  &&
            (PROPORTIONN_1_1 >= proportion)
        )
    )
    {
        return LED_ERRORPARAMETER;
    }
    // 3set the parameter
    self->cycle_time_ms     = Cycle_time;
    self->blink_times       = blink_times;
    self->proportion_on_off = proportion;

    //
    led_status_t ret = led_bliank(self);
    
    return ret;

}                                                 




/**
 * @brief init the target of bsp_led_driver_t.
 * 
 * Steps:
 *  1, make the targe at a specific statues;
 *  
 * 
 * @param[in] self      : Pointer to the input data.
 * 
 * @return led_status_t : The status of running
 * 
 * */
led_status_t led_driver_init( bsp_led_driver_t * const self)
{
    led_status_t ret = LED_OK;
    
    DEBUG_OUT("led init start \r\n");
    if( NULL == self )
    {
        #ifdef DEBUG
        DEBUG_OUT("LED_ERRORPARAMETER\r\n");
        return LED_ERRORPARAMETER;
        #endif // DEBUG
    }
    
    self->p_led_opes_inst->pf_led_off();
    self->p_os_time_delay->pf_os_delay_ms(600);
    uint32_t time_stamp = 0;
    self->p_time_base_ms->pf_get_time_ms(&time_stamp);

    return ret;
    
}

/**
 * @brief instantiate the target of bsp_led_driver_t.
 * 
 * Steps:
 *  1, Adding the Core interfaces into target of bsp_led_driver instance.
 *  2, Adding the OS interfaces into target of bsp_led_driver instance.
 *  3, Adding the timebase interfaces into target of bsp_led_driver instance.
 *  
 * 
 * @param[in] self      : Pointer to the input data.
 * @param[in] led_ops   : Length of the input data.
 * @param[in] os_delay  : Pointer to the input data.
 * @param[in] time_base : Pointer to the input data.
 * 
 * @return led_status_t : The status of running
 * 
 * */
led_status_t led_driver_inst (
                                      bsp_led_driver_t * const      self, 
                                      led_operations_t * const   led_ops,
#ifdef OS_SUPPORTING
                                      os_delay_t       * const  os_delay,
#endif //OS_SUPPORTING
                                      time_base_ms_t   * const time_base    )
{
    led_status_t ret = LED_OK;
    DEBUG_OUT("led inst Kick-off \r\n");

/*************1.Checing the input parameters**************/
    if( NULL == self      ||
        NULL == led_ops   ||
        NULL == os_delay  ||
        NULL == time_base
                              )
    {
        #ifdef DEBUG
        DEBUG_OUT("LED_ERRORPARAMETER\r\n");
        return LED_ERRORPARAMETER;
        #endif // DEBUG
    }

/*************2.Checing the Resources*********************/
    if( INITED == self->is_inited )
    {
#ifdef DEBUG
        DEBUG_OUT("LED_ERRORRESOURCE\r\n");
        return LED_ERRORRESOURCE;
#endif // DEBUG
    }

#ifdef DEBUG
    DEBUG_OUT("led inst start\r\n");
#endif  // DEBUG

/*************3.Adding the interface**********************/

    self->p_led_opes_inst =   led_ops;
    self->p_os_time_delay =  os_delay;
    self->p_time_base_ms  = time_base;
    self->pf_led_countroler = led_control;    

/*************4.Init the target     **********************/
    self->blink_times   =                   0;
    self->cycle_time_ms =                   0;
    self->proportion_on_off = PROPORTIONN_x_x;
    
    ret = led_driver_init(self);
    if( LED_OK != ret )
    {
#ifdef DEBUG
        DEBUG_OUT("LED init failed\r\n");
#endif  // DEBUG
        self->p_led_opes_inst =  NULL;
        self->p_os_time_delay =  NULL;
        self->p_time_base_ms  =  NULL;
        return ret;
    }
    
    self->is_inited = INITED ;
    DEBUG_OUT("LED init finished\r\n");
    
    return ret;

    
    



}
                        

//******************************** Defines **********************************//

//******************************** Declaring ********************************//
//******************************** Declaring ********************************//

