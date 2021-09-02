#include "includes.h"
#include "LETRA8X12.h"
#include "config.h"
#include "system.h"
#include "display.h"
#include "func.h"
#include "mem.h"

//******************************************************************************//
//         DEFINICION DE FUNCIONES     //
//******************************************************************************//
#define WORKING_BUFFER_LENGTH   128

typedef struct
{
    char workingBuffer[WORKING_BUFFER_LENGTH + 1];
    char displayBuffer[MAX_DISPLAY + 1];
    char *begin;
    char *end;
} Line;

typedef struct
{
    Line upperLine;
    Line lowerLine;
} Me;

static Me me;

static void displayRotate(Line *line)
{
    if (line->begin != NULL)
    {
        snprintf(line->displayBuffer, MAX_DISPLAY, "%s  %s", line->begin, line->workingBuffer);
        if (++line->begin >= line->end)
        {
            line->begin = line->workingBuffer;
        }
    }
    else
    {
        strncpy(line->displayBuffer, line->workingBuffer, MAX_DISPLAY);
    }
}

void Display_rotateWrite(char *source, uint8_t line)
{
    Line *currentLine;

    switch(line)
    {
        case LINEA_SUPERIOR:
            currentLine = &me.upperLine;
            break;
        case LINEA_INFERIOR:
            currentLine = &me.lowerLine;
            break;
        default:
            break;
    }

    if (strlen(source) > MAX_DISPLAY)
    {
        strncpy(currentLine->workingBuffer, source, WORKING_BUFFER_LENGTH);
        currentLine->begin = currentLine->workingBuffer;
        currentLine->end = (char *)(currentLine->workingBuffer + strlen(currentLine->workingBuffer));
    }
    else
    {
        strncpy(currentLine->workingBuffer, source, WORKING_BUFFER_LENGTH);
        currentLine->begin = currentLine->end = NULL;
    }
}

void Display_rotateTick(void)
{
    if (tSys_flag.bDisplayRotate)
    {
        if (strlen(me.lowerLine.workingBuffer) != 0)
        {
            displayRotate(&me.upperLine);
            displayRotate(&me.lowerLine);
            tSys_flag.bTerceralinea = 0;
//            display_erase_line(AMBAS_LINEAS, DISPLAY_AMBOS);
            tSys_flag.bErase = 1;
            display_print((uint8_t *)me.upperLine.displayBuffer, LINEA_SUPERIOR, FONDO_NEGRO, DISPLAY_AMBOS);
            tSys_flag.bErase = 1;
            display_print((uint8_t *)me.lowerLine.displayBuffer, LINEA_INFERIOR, FONDO_NEGRO, DISPLAY_AMBOS);
        }
        else
        {
            displayRotate(&me.upperLine);
            tSys_flag.bTerceralinea = 1;
            tSys_flag.bErase = 0;
//            display_erase_line(LINEA_SUPERIOR, DISPLAY_AMBOS);
            display_print((uint8_t *)me.upperLine.displayBuffer, LINEA_SUPERIOR, FONDO_NEGRO, DISPLAY_AMBOS);
        }
    }
}

void Display_rotateSet(void)
{
    tSys_flag.bDisplayRotate = TRUE;
}

void Display_rotateUnSet(void)
{
    tSys_flag.bDisplayRotate = FALSE;
}


/**
Funcion que permite convertir un caracter en mapa de bits con un fondo negro
@ch2 Caracter
@Posx Posicion X
@Posy Linea  el display
@Posy Linea  el display
@Font Tipo de Fuente
*/
void CharToDot(uint8_t ch, size_t posx, size_t posy, uint8_t cFondo)
{
    size_t i;
    uint8_t bitMapBuffer[20];

    memset(bitMapBuffer, 0, sizeof(bitMapBuffer));

    /* Se coloca el mapa de bits en el vector */
    for (i = 0; i < 12; ++i)
    {
        if(cFondo == FONDO_BLANCO)
        {
            bitMapBuffer[i]= ~Fnt8L12[(12 * ch) + i];
        }
        else
        {
            bitMapBuffer[i]= Fnt8L12[(12 * ch) + i];

        }
    }
    /* Coloca el patro de bits en pantalla */
    SPEC12832DisplayPattern(bitMapBuffer, posx, posy);
}

void CharToDot_3(uint8_t ch, size_t posx, uint8_t cFondo3)
{
    size_t i;
    uint8_t bitMapBuffer[8];
    uint8_t letras[] = { 'A', 'B', 'C', 'D', 'E', 'F', 'L', 'O', 'R', 'T', 'V', ' ' };

    memset (bitMapBuffer, 0x00, sizeof(bitMapBuffer));
    SPEC12832Select(LCD_SELECT_USER);
    for(i = 0; i < sizeof(letras); ++i)
    {
        if(letras[i] == ch)
        {
            ch = i;
            break;
        }
    }
    for (i = 0; i < 8; ++i)
    {
        if(cFondo3 == FONDO_BLANCO)
        {
            bitMapBuffer[i]= ~Arial_Narrow7x7[(8 * ch) + i];
        }
        else
        {
            bitMapBuffer[i]= Arial_Narrow7x7[(8 * ch) + i];
        }
    }
    shift_display(bitMapBuffer, posx);
}


void display_print2(uint8_t *pText, uint8_t cLine,uint16_t lcdini, uint16_t lcdfin,uint8_t cFondo2)
{
    uint8_t     cIndex;

    //Se define el display
    SPEC12832Select(LCD_SELECT_USER);

    //Se envia caracter a caracter la linea de texto
    for(cIndex=lcdini;cIndex<lcdfin;cIndex++)
    {
        CharToDot_3(pText[cIndex],cIndex,cFondo2);
    }
}

/**
Funcion que permite imprimir una linea corrida de texto en el display
@pText Puntero al texto que se desea imprimir
@cLine Linea que se desea imprimir (1-2)
@retval Resultado de la operacion
@note  Nota importante.
*/
void display_print(uint8_t *pText, uint8_t cLine, uint8_t cFondo, uint8_t nDisplay)
{
    uint16_t  iParamLinea;
    uint8_t     cIndex;
    uint8_t     cCantCarac;

    //Se define el display
    if(nDisplay==DISPLAY_OPERADOR)
    {
        SPEC12832Select(LCD_SELECT_USER);
    }
    else if(nDisplay==DISPLAY_CLIENTE)
    {
        SPEC12832Select(LCD_SELECT_CUSTOM);
    }
    else
    {
        SPEC12832Select(LCD_SELECT_USER | LCD_SELECT_CUSTOM);
    }

    if(cLine==LINEA_SUPERIOR)iParamLinea = 16;
    else iParamLinea = 0;

    cCantCarac = strlen((char*)pText);
    if(cCantCarac>MAX_DISPLAY)cCantCarac=MAX_DISPLAY;

    //Se envia caracter a caracter la linea de texto
    for(cIndex=0;cIndex<cCantCarac;cIndex++)
    {
        CharToDot(pText[cIndex],cIndex,iParamLinea,cFondo);
    }
}


/**
Funcion que permite imprimir una linea corrida de texto en el display
@pText Puntero al texto que se desea imprimir
@cLine Linea que se desea imprimir (1-2)
@retval Resultado de la operacion
@note  Nota importante.
*/
void display_erase_line(uint8_t cLine, uint8_t nDisplay)
{
    uint8_t cBuffer[20]={NULL};
    tSys_flag.bErase=1;
    memset(cBuffer, ' ', sizeof(cBuffer));
    cBuffer[MAX_DISPLAY]=0;


    if(cLine<=LINEA_INFERIOR)
    {
        display_print(cBuffer, cLine, FONDO_NEGRO, nDisplay);
    }
    else
    {
        display_print(cBuffer, LINEA_SUPERIOR, FONDO_NEGRO, nDisplay);
        display_print(cBuffer, LINEA_INFERIOR, FONDO_NEGRO, nDisplay);
    }
    tSys_flag.bErase=0;
}


/**
Mensaje Inicial
@param Descripcion de Parametro 1
@param Descripcion de Parametro 2
@retval Resultado de la operacion
@note  Nota importante.
*/
void display_message_ini(void)
{
    tMSG_COM *msgcom;
    uint8_t buffer[128];
    uint8_t cResp  = FALSE;

    msgcom = (tMSG_COM *)buffer;

    if (tConfig.cFlag.bMensajeComercial == TRUE)
    {
        cResp = mem_read_write(MEM_MSG_COMERCIAL, msgcom->cValor, 0, MODO_LECTURA);
    }

    tSys_flag.bErase = 1;
    display_erase_line(AMBAS_LINEAS, DISPLAY_AMBOS);
    if ((cResp == TRUE) && (msgcom->cMarca == MARCA))
    {
        Display_rotateWrite((char *)msgcom->cMensaje, LINEA_SUPERIOR);
        Display_rotateWrite((char *)"", LINEA_INFERIOR);
        Display_rotateSet();
    }
    else
    {
        Display_rotateUnSet();
        display_erase_line(AMBAS_LINEAS,DISPLAY_AMBOS);
        memset(buffer, 0 , sizeof(buffer));
        mensaje_leer(buffer, MENSAJE_MARCA);
        display_print(buffer, LINEA_SUPERIOR, FONDO_NEGRO, DISPLAY_AMBOS);
        tSys_flag.bTerceralinea = 1;
    }
}
/**
Imprime solo un caracter en la posición indicada del display
@caracter Es el caracter a mostrar en pantalla
@posx Posición del cursor en el display
@cLine Linea en donde se imprime el texto
@note  Nota importante.
*/
void display_print_un_caracter(uint8_t caracter,int posx, uint8_t cLine, uint8_t nDisplay)
{
    uint16_t  iParamLinea;

    //Se define el display
    if(nDisplay==DISPLAY_OPERADOR)
    {
        SPEC12832Select(LCD_SELECT_USER);
    }
    else if(nDisplay==DISPLAY_CLIENTE)
    {
        SPEC12832Select(LCD_SELECT_CUSTOM);
    }
    else
    {
        SPEC12832Select(LCD_SELECT_USER | LCD_SELECT_CUSTOM);
    }

    if(posx>=16)
    {
        cLine=LINEA_INFERIOR;
        posx-=16;
    }

    if(cLine==LINEA_SUPERIOR)iParamLinea = 16;
    else iParamLinea = 0;

    if(posx==0&&cLine==LINEA_SUPERIOR)
    {
        display_erase_line(iParamLinea,nDisplay);
    }

    CharToDot(caracter,posx,iParamLinea,FONDO_NEGRO);

}

/**
Mensaje estandar en el display
@nrodemensaje Nro del mensaje que se quiere mostrar
@nDisplay Display donde se mostrara el mensaje
@retval Resultado de la operacion
@note
*/
void mensaje_display(uint16_t nrodemensaje, uint8_t nDisplay) //
{
    uint8_t mensaje[20];

    Display_rotateUnSet();
    memset(mensaje,0x20,MAX_DISPLAY);
    mensaje[MAX_DISPLAY]=0;
    tSys_flag.bTerceralinea=0;
    display_erase_line(AMBAS_LINEAS,nDisplay);
    mensaje_leer(mensaje,nrodemensaje);
    mensaje[MAX_DISPLAY]=0;
    tSys_flag.bErase=1;
    display_print(mensaje,LINEA_SUPERIOR,FONDO_NEGRO,nDisplay);
    tSys_flag.bTerceralinea=1;
    tSys_flag.bErase=0;	
}

/**
Inicializacion del Display
*/
void display_ini(void)
{
    display_erase_line(AMBAS_LINEAS,DISPLAY_AMBOS);
}
