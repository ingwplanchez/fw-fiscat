#ifndef _EVENT_H_
#define _EVENT_H_
//===================================================
enum
{
	EVT_KEYDOWN,
	EVT_KEYUP,
	EVT_KEYHOLD,
	EVT_UART_RX,
	EVT_MAX
};
//===================================================
extern void EventInit(void);
extern void EventPost(EVENT_T evt);
extern EVENT_T EventPend(void);
//====================================================
#endif
