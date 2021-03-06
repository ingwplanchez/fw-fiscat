/****************************************************************************
 *   $Id:: gpio.c 4068 2010-07-30 02:25:13Z usb00423                        $
 *   Project: NXP LPC11xx GPIO example
 *
 *   Description:
 *     This file contains GPIO code example which include GPIO
 *     initialization, GPIO interrupt handler, and related APIs for
 *     GPIO access.
 *
 ****************************************************************************
 * Software that is described herein is for illustrative purposes only
 * which provides customers with programming information regarding the
 * products. This software is supplied "AS IS" without any warranties.
 * NXP Semiconductors assumes no responsibility or liability for the
 * use of the software, conveys no license or title under any patent,
 * copyright, or mask work right to the product. NXP Semiconductors
 * reserves the right to make changes in the software without
 * notification. NXP Semiconductors also make no representation or
 * warranty that such application will be suitable for the specified
 * use without further testing or modification.
****************************************************************************/
#include "LPC11xx.h"            /* LPC11xx Peripheral Registers */
#include "gpio.h"
#include "includes.h"
#include "keyboard.h"

/*****************************************************************************
** Function name:       PIOINT0_IRQHandler
**
** Descriptions:        Use one GPIO pin(port0 pin1) as interrupt source
**
** parameters:          None
** Returned value:      None
**
*****************************************************************************/
#if 0
void PIOINT0_IRQHandler(void)
{
    if (GPIOIntStatus(PORT0, P0_KEYI0))
    {
        GPIOIntClear(PORT0, P0_KEYI0);
        Keyboard_postKey(7);
    }
    if (GPIOIntStatus(PORT0, P0_KEYI1))
    {
        GPIOIntClear(PORT0, P0_KEYI1);
        Keyboard_postKey(6);
    }
    if (GPIOIntStatus(PORT0, P0_KEYI2))
    {
        GPIOIntClear(PORT0, P0_KEYI2);
        Keyboard_postKey(5);
    }
    if (GPIOIntStatus(PORT0, P0_KEYI6))
    {
        GPIOIntClear(PORT0, P0_KEYI6);
        Keyboard_postKey(1);
    }
    if (GPIOIntStatus(PORT0, P0_KEYI7))
    {        
        GPIOIntClear(PORT0, P0_KEYI7);
        Keyboard_postKey(0);
    }
    return;
}
#endif

/*****************************************************************************
** Function name:       PIOINT1_IRQHandler
**
** Descriptions:        Use one GPIO pin(port1 pin1) as interrupt source
**
** parameters:          None
** Returned value:      None
**
*****************************************************************************/
#if 0
void PIOINT1_IRQHandler(void)
{
    static uint32_t volatile flags;

    if (GPIOIntStatus(PORT1, P1_KEYI3))
    {
        GPIOIntClear(PORT1, P1_KEYI3);
        Keyboard_postKey(4);
    }
    return;
}
#endif

/*****************************************************************************
** Function name:       PIOINT2_IRQHandler
**
** Descriptions:        Use one GPIO pin(port2 pin1) as interrupt source
**
** parameters:          None
** Returned value:      None
**
*****************************************************************************/
#if 0
void PIOINT2_IRQHandler(void)
{
    return;
}
#endif

/*****************************************************************************
** Function name:       PIOINT3_IRQHandler
**
** Descriptions:        Use one GPIO pin(port3 pin1) as interrupt source
**
** parameters:          None
** Returned value:      None
**
*****************************************************************************/
#if 0
void PIOINT3_IRQHandler(void)
{
    static uint32_t volatile flags;
    
    if (GPIOIntStatus(PORT3, P3_KEYI4))
    {
        GPIOIntClear(PORT3, P3_KEYI4);
        Keyboard_postKey(3);
    }
    if (GPIOIntStatus(PORT3, P3_KEYI5))
    {
        GPIOIntClear(PORT3, P3_KEYI5);
        Keyboard_postKey(2);
    }
    return;
}
#endif

/*****************************************************************************
** Function name:       GPIOInit
**
** Descriptions:        Initialize GPIO, install the
**                      GPIO interrupt handler
**
** parameters:          None
** Returned value:      true or false, return false if the VIC table
**                      is full and GPIO interrupt handler can be
**                      installed.
**
*****************************************************************************/
void GPIOInit( void )
{
    /* Enable AHB clock to the GPIO domain. */
    LPC_SYSCON->SYSAHBCLKCTRL |= (1<<6);
#if 1 /* __JTAG_DISABLED */
    LPC_IOCON->R_PIO1_1  &= ~0x07;
    LPC_IOCON->R_PIO1_1  |= 0x01;
#endif
#if 0
    /* Set up NVIC when I/O pins are configured as external interrupts. */
    NVIC_EnableIRQ(EINT0_IRQn);
    NVIC_EnableIRQ(EINT1_IRQn);
    NVIC_EnableIRQ(EINT2_IRQn);
    NVIC_EnableIRQ(EINT3_IRQn);
#endif
    return;
}

/*****************************************************************************
** Function name:       GPIOSetInterrupt
**
** Descriptions:        Set interrupt sense, event, etc.
**                      edge or level, 0 is edge, 1 is level
**                      single or double edge, 0 is single, 1 is double
**                      active high or low, etc.
**
** parameters:          port num, bit position, sense, single/double, polarity
** Returned value:      None
**
*****************************************************************************/
void GPIOSetInterrupt( uint32_t portNum, uint32_t bitPosi, uint32_t sense,
                       uint32_t single, uint32_t event )
{
    switch ( portNum )
    {
    case PORT0:
        if ( sense == 0 )
        {
            LPC_GPIO0->IS &= ~(0x1<<bitPosi);
            /* single or double only applies when sense is 0(edge trigger). */
            if ( single == 0 )
                LPC_GPIO0->IBE &= ~(0x1<<bitPosi);
            else
                LPC_GPIO0->IBE |= (0x1<<bitPosi);
        }
        else
            LPC_GPIO0->IS |= (0x1<<bitPosi);
        if ( event == 0 )
            LPC_GPIO0->IEV &= ~(0x1<<bitPosi);
        else
            LPC_GPIO0->IEV |= (0x1<<bitPosi);
        break;
    case PORT1:
        if ( sense == 0 )
        {
            LPC_GPIO1->IS &= ~(0x1<<bitPosi);
            /* single or double only applies when sense is 0(edge trigger). */
            if ( single == 0 )
                LPC_GPIO1->IBE &= ~(0x1<<bitPosi);
            else
                LPC_GPIO1->IBE |= (0x1<<bitPosi);
        }
        else
            LPC_GPIO1->IS |= (0x1<<bitPosi);
        if ( event == 0 )
            LPC_GPIO1->IEV &= ~(0x1<<bitPosi);
        else
            LPC_GPIO1->IEV |= (0x1<<bitPosi);
        break;
    case PORT2:
        if ( sense == 0 )
        {
            LPC_GPIO2->IS &= ~(0x1<<bitPosi);
            /* single or double only applies when sense is 0(edge trigger). */
            if ( single == 0 )
                LPC_GPIO2->IBE &= ~(0x1<<bitPosi);
            else
                LPC_GPIO2->IBE |= (0x1<<bitPosi);
        }
        else
            LPC_GPIO2->IS |= (0x1<<bitPosi);
        if ( event == 0 )
            LPC_GPIO2->IEV &= ~(0x1<<bitPosi);
        else
            LPC_GPIO2->IEV |= (0x1<<bitPosi);
        break;
    case PORT3:
        if ( sense == 0 )
        {
            LPC_GPIO3->IS &= ~(0x1<<bitPosi);
            /* single or double only applies when sense is 0(edge trigger). */
            if ( single == 0 )
                LPC_GPIO3->IBE &= ~(0x1<<bitPosi);
            else
                LPC_GPIO3->IBE |= (0x1<<bitPosi);
        }
        else
            LPC_GPIO3->IS |= (0x1<<bitPosi);
        if ( event == 0 )
            LPC_GPIO3->IEV &= ~(0x1<<bitPosi);
        else
            LPC_GPIO3->IEV |= (0x1<<bitPosi);
        break;
    default:
        break;
    }
    return;
}

/*****************************************************************************
** Function name:       GPIOIntEnable
**
** Descriptions:        Enable Interrupt Mask for a port pin.
**
** parameters:          port num, bit position
** Returned value:      None
**
*****************************************************************************/
void GPIOIntEnable( uint32_t portNum, uint32_t bitPosi )
{
    switch ( portNum )
    {
    case PORT0:
        LPC_GPIO0->IE |= (0x1<<bitPosi);
        break;
    case PORT1:
        LPC_GPIO1->IE |= (0x1<<bitPosi);
        break;
    case PORT2:
        LPC_GPIO2->IE |= (0x1<<bitPosi);
        break;
    case PORT3:
        LPC_GPIO3->IE |= (0x1<<bitPosi);
        break;
    default:
        break;
    }
    return;
}

/*****************************************************************************
** Function name:       GPIOIntDisable
**
** Descriptions:        Disable Interrupt Mask for a port pin.
**
** parameters:          port num, bit position
** Returned value:      None
**
*****************************************************************************/
void GPIOIntDisable( uint32_t portNum, uint32_t bitPosi )
{
    switch ( portNum )
    {
    case PORT0:
        LPC_GPIO0->IE &= ~(0x1<<bitPosi);
        break;
    case PORT1:
        LPC_GPIO1->IE &= ~(0x1<<bitPosi);
        break;
    case PORT2:
        LPC_GPIO2->IE &= ~(0x1<<bitPosi);
        break;
    case PORT3:
        LPC_GPIO3->IE &= ~(0x1<<bitPosi);
        break;
    default:
        break;
    }
    return;
}

/*****************************************************************************
** Function name:       GPIOIntStatus
**
** Descriptions:        Get Interrupt status for a port pin.
**
** parameters:          port num, bit position
** Returned value:      None
**
*****************************************************************************/
uint32_t GPIOIntStatus( uint32_t portNum, uint32_t bitPosi )
{
    uint32_t regVal = 0;

    switch ( portNum )
    {
    case PORT0:
        if ( LPC_GPIO0->MIS & (0x1<<bitPosi) )
            regVal = 1;
        break;
    case PORT1:
        if ( LPC_GPIO1->MIS & (0x1<<bitPosi) )
            regVal = 1;
        break;
    case PORT2:
        if ( LPC_GPIO2->MIS & (0x1<<bitPosi) )
            regVal = 1;
        break;
    case PORT3:
        if ( LPC_GPIO3->MIS & (0x1<<bitPosi) )
            regVal = 1;
        break;
    default:
        break;
    }
    return ( regVal );
}

/*****************************************************************************
** Function name:       GPIOIntClear
**
** Descriptions:        Clear Interrupt for a port pin.
**
** parameters:          port num, bit position
** Returned value:      None
**
*****************************************************************************/
void GPIOIntClear( uint32_t portNum, uint32_t bitPosi )
{
    switch ( portNum )
    {
    case PORT0:
        LPC_GPIO0->IC |= (0x1<<bitPosi);
        break;
    case PORT1:
        LPC_GPIO1->IC |= (0x1<<bitPosi);
        break;
    case PORT2:
        LPC_GPIO2->IC |= (0x1<<bitPosi);
        break;
    case PORT3:
        LPC_GPIO3->IC |= (0x1<<bitPosi);
        break;
    default:
        break;
    }
    return;
}

/*****************************************************************************
** Function name:       GPIOSetValue
**
** Descriptions:        Set/clear a bitvalue in a specific bit position
**                      in GPIO portX(X is the port number.)
**
** parameters:          port num, bit position, bit value
** Returned value:      None
**
*****************************************************************************/
void GPIOSetValue( uint32_t portNum, uint32_t bitPosi, uint32_t bitVal )
{
    LPC_GPIO[portNum]->MASKED_ACCESS[(1<<bitPosi)] = (bitVal<<bitPosi);
}

uint32_t GPIOGetValue(uint32_t porNum, uint32_t bitPosi)
{
    return (LPC_GPIO[porNum]->MASKED_ACCESS[1 << bitPosi])? 1: 0;
}

/*****************************************************************************
** Function name:       GPIOSetDir
**
** Descriptions:        Set the direction in GPIO port
**
** parameters:          port num, bit position, direction (1 out, 0 input)
** Returned value:      None
**
*****************************************************************************/
void GPIOSetDir( uint32_t portNum, uint32_t bitPosi, uint32_t dir )
{
    if(dir)
        LPC_GPIO[portNum]->DIR |= 1<<bitPosi;
    else
        LPC_GPIO[portNum]->DIR &= ~(1<<bitPosi);
}

/******************************************************************************
**                            End Of File
******************************************************************************/
