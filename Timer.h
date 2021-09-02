#ifndef _TIMER_H_
#define _TIMER_H_

extern void SysTick_Handler(void);
extern uint32_t SysGetTimerTick(void);
extern void OSTimeDly(uint32_t tick);
extern void Tim4Init(void);
extern void set_timeout(uint16_t time);
extern uint8_t timeout_expire(void);

#endif
