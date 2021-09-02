#include "includes.h"
#include <stdio.h>
#include <rt_misc.h>
#pragma import(__use_no_semihosting_swi)

struct __FILE
{
    int handle;
};

FILE __stdin, __stdout, __stderr;

int fputc(int ch, FILE *f)
{
    //Uart1_Putchar(ch);
    return ch;
}

int ferror(FILE *f)
{
    return 0;
}

void _ttywrch(int ch)
{
    //sendchar(ch);
}


void _sys_exit(int return_code)
{
label:
    goto label;  /* endless loop */
}

