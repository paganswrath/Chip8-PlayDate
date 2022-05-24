#include "Cpu.h"


#define ScreenWidth 64
#define ScreenHeight 32

static int ScreenScale = 6;
static int XOff = 7;
static int YOff = 24;
static bool Interlaced = false;

void ClearScreen(unsigned char Color){
    for (int x = 0 ; x <= ScreenWidth; x ++){
        for (int y = 0 ; y <= ScreenHeight; y ++){
            ScreenMemory[x][y] = Color;
        }
    }
}

void DrawScreen(){
    for (int x = 0 ; x < ScreenWidth; x ++){
        for (int y = 0 ; y < ScreenHeight; y ++){
            if (Interlaced){
                if (y % 2 == 0){
                    if (ScreenMemory[x][y] == 1){
                        PD->graphics->fillRect((x * ScreenScale ) + XOff,(y * ScreenScale ) + YOff, 1* ScreenScale, 1* ScreenScale, kColorWhite);
                    }
                    else {
                        PD->graphics->fillRect((x * ScreenScale ) + XOff,(y * ScreenScale ) + YOff, 1* ScreenScale, 1* ScreenScale, kColorBlack);
                    }
                }
            }
            else {
                if (ScreenMemory[x][y] == 1){
                    PD->graphics->fillRect((x * ScreenScale ) + XOff,(y * ScreenScale ) + YOff, 1* ScreenScale, 1* ScreenScale, kColorWhite);
                }
                else {
                    PD->graphics->fillRect((x * ScreenScale ) + XOff,(y * ScreenScale ) + YOff, 1* ScreenScale, 1* ScreenScale, kColorBlack);
                }
            }
        }
    }

}