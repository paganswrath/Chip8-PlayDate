#include "Cpu.h"

static int ScreenScale = 6;

void ClearScreen(unsigned char Color){
    for (int x = 0 ; x <= 64; x ++){
        for (int y = 0 ; y <= 32; y ++){
            ScreenMemory[x][y] = Color;
        }
    }
}