#include "includes.h"
#include "keyboard.h"


/* Local-scope ---------------------------------------------------------------*/
#define KEYBOARD_QUEUE_LENGTH   8
#define KEYBOARD_COL            8
#define KEYBOARD_ROW            5

typedef struct {
    struct {
        KeyBoardKey event[KEYBOARD_QUEUE_LENGTH];
        size_t volatile head;
        size_t tail;
    } queue;
    uint8_t keyStatus[KEYBOARD_ROW];
    uint8_t prevKey;
    uint8_t keyHoldTimer;
} Me;

static void postKey(KeyBoardKey event);
static void scanDelay(void);
static void scanRowOutput(uint32_t row);
static uint32_t scanColInput(void);

static Me me;


/*............................................................................*/
void Keyboard_init(void) {
    /* Configura el pin 10 del puerto 1 como salida */
    GPIOSetDir(PORT1, P1_KEYO0, 1);
    GPIOSetValue(PORT1, P1_KEYO0, 0);
    LPC_IOCON->PIO1_10 = IOCON_PIO;
    /* Configura el pin 11 del puerto 0 como salida */
    GPIOSetDir(PORT0, P0_KEYO1, 1);
    GPIOSetValue(PORT0, P0_KEYO1, 0);
    LPC_IOCON->R_PIO0_11 = IOCON_R_PIO;
    /* Configura el pin 0 del puerto 1 como salida */
    GPIOSetDir(PORT1, P1_KEYO2, 1);
    GPIOSetValue(PORT1, P1_KEYO2, 0);
    LPC_IOCON->R_PIO1_0 = IOCON_R_PIO;
    /* Configura el pin 1 del puerto 1 como salida */
    GPIOSetDir(PORT1, P1_KEYO3, 1);
    GPIOSetValue(PORT1, P1_KEYO3, 0);
    LPC_IOCON->R_PIO1_1 = IOCON_R_PIO;
    /* Configura el pin 2 del puerto 1 como salida */
    GPIOSetDir(PORT1, P1_KEYO4, 1);
    GPIOSetValue(PORT1, P1_KEYO4, 0);
    LPC_IOCON->R_PIO1_2 = IOCON_R_PIO;
    /* Variables locales a sus valores por defecto */
    memset(me.keyStatus, 0xFF, sizeof(me.keyStatus));
    me.queue.head = 0;
    me.queue.tail = 0;
    me.prevKey = 0;
    me.keyHoldTimer = 0;
}
/*............................................................................*/
void Keyboard_scan(void) {
    size_t col;
    size_t row;
    uint8_t keys;
    uint32_t temp;
	
    /* Escaneo de teclas */
    for(row = 0, keys = 0; row < KEYBOARD_ROW; ++row) {
        scanRowOutput(1 << row);            /* Activa la fila correspondiente */
        scanDelay();
        temp = scanColInput();
        if(me.keyStatus[row] != temp) {
            me.keyStatus[row] = temp;
            ++keys;
        }
    }
    scanRowOutput(0);                                  /* Desactiva las filas */
    if(keys) {
        return;                                       /* Hubo rebote de tecla */
    }

    /* Comprueba tecla pulsada */
    for(row = 0, keys = 0; row < KEYBOARD_ROW; ++row) {
        temp = me.keyStatus[row];
        for(col = 0; col < KEYBOARD_COL; ++col) {
            if((temp & (1 << col)) == 0) {                   /* Tecla pulsada */
                if(keys) {
                    return;                      /*  Mas de una tecla pulsada */
                }
                keys = (row * KEYBOARD_COL) + col + 1;
            }
        }
    }

    /* Comprueba el estado de la tecla */
    if(me.prevKey ^ keys) {
        if(me.prevKey) {
            postKey(me.prevKey);
        }
        me.prevKey = keys;
    }
}
/*............................................................................*/
KeyBoardKey KeyBoard_getKey(void) {
    KeyBoardKey event;

    if (me.queue.head != me.queue.tail) {
        event = me.queue.event[me.queue.tail];
        me.queue.tail = (me.queue.tail + 1) % (ARRAY_SIZE(me.queue.event) - 1);
    }
    else {
        event = 0;
    }
    return event;
}
/*............................................................................*/
static void postKey(KeyBoardKey event) {
    me.queue.event[me.queue.head] = event;
    me.queue.head = (me.queue.head + 1) % (ARRAY_SIZE(me.queue.event) - 1);
}
/*............................................................................*/
static void scanDelay(void) {
    volatile uint32_t t;

    for(t = 30; t; --t) {
    }
}
/*............................................................................*/
static void scanRowOutput(uint32_t row) {
    GPIOSetValue(PORT1, P1_KEYO0, ((row & (1 << 0))? 1: 0));
    GPIOSetValue(PORT0, P0_KEYO1, ((row & (1 << 1))? 1: 0));
    GPIOSetValue(PORT1, P1_KEYO2, ((row & (1 << 2))? 1: 0));
    GPIOSetValue(PORT1, P1_KEYO3, ((row & (1 << 3))? 1: 0));
    GPIOSetValue(PORT1, P1_KEYO4, ((row & (1 << 4))? 1: 0));
}
/*............................................................................*/
static uint32_t scanColInput(void) {
    uint32_t col;

    col = 0;

    col |= (LPC_GPIO0->DATA & (1 << P0_KEYI0))? (1 << 0): 0;
    col |= (LPC_GPIO0->DATA & (1 << P0_KEYI1))? (1 << 1): 0;
    col |= (LPC_GPIO0->DATA & (1 << P0_KEYI2))? (1 << 2): 0;
    col |= (LPC_GPIO1->DATA & (1 << P1_KEYI3))? (1 << 3): 0;
    col |= (LPC_GPIO3->DATA & (1 << P3_KEYI4))? (1 << 4): 0;
    col |= (LPC_GPIO3->DATA & (1 << P3_KEYI5))? (1 << 5): 0;
    col |= (LPC_GPIO0->DATA & (1 << P0_KEYI6))? (1 << 6): 0;
    col |= (LPC_GPIO0->DATA & (1 << P0_KEYI7))? (1 << 7): 0;

    return col;
}

