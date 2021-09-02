#include "Includes.h"

typedef struct
{
    volatile uint8_t head;
    uint8_t tail;
    EVENT_T evt[32];
} MAINQ_T;

static MAINQ_T  EventQ;
//===================================================
extern void EventInit(void)
{
    EventQ.head = EventQ.tail = 0;
}
//===================================================
extern void EventPost(EVENT_T evt)
{
    __disable_irq();
    EventQ.evt[EventQ.head] = evt;
    EventQ.head = (EventQ.head + 1) % (ARRAY_SIZE(EventQ.evt) - 1);
    __enable_irq();
}
//====================================================
extern EVENT_T EventPend(void)
{
    EVENT_T evt;

    while(EventQ.head == EventQ.tail);
    evt = EventQ.evt[EventQ.tail];
    EventQ.tail = (EventQ.tail + 1) % (ARRAY_SIZE(EventQ.evt) - 1);
    return evt;
}
//====================================================

