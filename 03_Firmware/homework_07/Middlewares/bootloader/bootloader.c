#include "bootloader.h"

#define ApplicationAddress 0x08008000

typedef  void (*pFunction)(void);
pFunction Jump_To_Application;
uint32_t JumpAddress;

void bootloader_clear_peripheral(void)
{
    HAL_DeInit();
    HAL_RCC_DeInit();
}

void bootloader_colse_irq(void)
{
    __set_PRIMASK(1);
    SysTick->CTRL = 0;
    SysTick->LOAD = 0;
    SysTick->VAL  = 0;    
    for(int i = 0; i < 8; i++)
    {
        NVIC->ICER[i] = 0xFFFFFFFF;
        NVIC->ICPR[i] = 0xFFFFFFFF;
    }   
}

int8_t jump_app(void)   
{
    printf(" start Jump app\n");
    if( ((*(__IO uint32_t*)ApplicationAddress)&0x2FFE0000) == 0x20000000)
    {
        printf("Run to app\n");
        // 清除外设
        bootloader_clear_peripheral();
        // 关闭中断
        bootloader_colse_irq();
        // 设置跳转地址
        JumpAddress = *(__IO uint32_t*) (ApplicationAddress + 4);
        // 设置跳转地址
        Jump_To_Application = (pFunction) JumpAddress;
        // 设置主堆栈指针
        __set_MSP(*(__IO uint32_t*) ApplicationAddress);
        // 跳转到应用
        Jump_To_Application();

        return 0;   
    }
    else
    {
        printf("Run to app error\n");
        return -1;   
    }


}


