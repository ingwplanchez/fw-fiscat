#ifndef __INCLUDES_H__
#define __INCLUDES_H__
//=============================================================================
//(STD Library)
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
//--------------------------------------------
//(Device STD Library)
#include "lpc11xx.h"

#include "clkconfig.h"
//#include "timer16.h"
// #include "timer32.h"
#include "gpio.h"
//--------------------------------------------
//(Base type)
#include "type.h"

//--------------------------------------------
//(Hardware Interface)
#include "hardware.h"

#include "timer.h"
#include "spi1.h"
//--------------------------------------------
//(BSP interface)
#include "uart.h"
#include "keyscan.h"
#include "F25L016A.h"
#include "spec12832.h"
//--------------------------------------------
// …œ≤„”¶”√(User Layer)

#include "system.h"
/*
#include "adc.h"


#include "lcd.h" */
//=============================================================================
#endif
