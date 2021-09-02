#include "Includes.h"
#include "keyboard.h"
#include "system.h"
#define TIM4            4

volatile uint32_t Tim4Tick;
volatile uint8_t dummy;


static void TIM_Cmd(uint8_t timer,uint8_t enable)
{
    if(enable)
    {
        SysTick->CTRL = (1 << SYSTICK_CLKSOURCE) | (1<<SYSTICK_ENABLE) | (1<<SYSTICK_TICKINT); /* Enable SysTick IRQ and SysTick Timer */
    }
    else
    {
        SysTick->CTRL = 0;
    }
}

extern void SysTick_Handler(void)
{
    uint8_t i;

    Tim4Tick++;

    for(i=0; i<sizeof(Tim4Tick)*8; i++)     // 32 bit
    {
        if(Tim4Tick & (1L << i))
        {
            break;
        }
    }
    switch(i)
    {
    case 0:     // 2ms
        if( iTimeOut > 0 ) iTimeOut--;
        break;
    case 1:     // 4ms
        break;
    case 2:     // 8ms
        break;
    case 3:     // 16ms
        tSys_flag.bKeyboardScan = TRUE;  		
        break;
    case 4:     // 32ms
        break;
    case 5:     // 64ms
        break;
    case 6:     // 128ms
        break;
    case 7:     // 256ms
        break;
    case 8:     // 512ms
        break;
    case 9:     // 1024ms   
				tSys_flag.bDisplayTick = TRUE;	
        break;
    default:    // ...
        break;
    }
}

extern uint32_t SysGetTimerTick(void)
{
    return Tim4Tick;
}

extern void OSTimeDly(uint32_t tick)
{
        iTimeOut = tick/2;      //*1000;
        while(iTimeOut != 0);   //{ClrWdt();};
}

extern void Tim4Init(void)
{
    SysTick_Config(SystemFrequency/1000);

    TIM_Cmd(TIM4, 1);
}


extern void set_timeout(uint16_t time)
{
    iTimeOut = time / 2;

    return;
}

extern uint8_t timeout_expire(void)
{
    if( iTimeOut > 0 )
        return FALSE;

    return TRUE;
}

