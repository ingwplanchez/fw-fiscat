#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdint.h>

typedef uint8_t KeyBoardKey;

void Keyboard_init(void);
void Keyboard_scan(void);
KeyBoardKey KeyBoard_getKey(void);

#endif

