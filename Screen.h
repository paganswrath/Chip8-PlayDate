#include "Cpu.h"


#define ScreenWidth 64
#define ScreenHeight 32

static int ScreenScale = 6;

void ClearScreen(unsigned char Color){
    for (int x = 0 ; x <= ScreenWidth; x ++){
        for (int y = 0 ; y <= ScreenHeight; y ++){
            ScreenMemory[x][y] = Color;
        }
    }
}