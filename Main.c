#include <stdio.h>
#include <stdlib.h>
#include "Memory.h"

static int update(void* userdata);

static LCDFont* Font = NULL;

#ifdef _WINDLL
__declspec(dllexport)
#endif

void setPDPtr(PlaydateAPI* p) {
	pd = p;
}

int eventHandler(PlaydateAPI* playdate, PDSystemEvent event, uint32_t arg)
{
	(void)arg;

	if ( event == kEventInit )
	{
		setPDPtr(playdate);
		playdate->display->setRefreshRate(50);
		pd->system->setUpdateCallback(update, pd);

		Font = pd->graphics->loadFont("/System/Fonts/Asheville-Sans-14-Bold.pft", NULL);

	}
	
	return 0;
}

bool FirstLoad = true;

static int XOff = 7;
static int YOff = 24;


static int update(void* userdata)
{

	if (FirstLoad){
		srand(time(NULL));
		InitMemory();
		ClearScreen(0x00);
		FirstLoad = false;
	}

	pd->graphics->clear(kColorWhite);

	pd->graphics->setFont(Font);

	if (RomLoaded){
		pd->graphics->drawText("CHIP8 PlayDate", strlen("CHIP8 PlayDate"), kASCIIEncoding, 400 / 2 - 99 / 2 , 4);


		PDButtons Current;
		pd->system->getButtonState(&Current, NULL, NULL);



		RunCycle(); 
		
		for (int x = 0 ; x < 64; x ++){
			for (int y = 0 ; y < 32; y ++){
				if (ScreenMemory[x][y] == 1){
					pd->graphics->fillRect((x * ScreenScale ) + XOff,(y * ScreenScale ) + YOff, 1* ScreenScale, 1* ScreenScale, kColorWhite);
				}
				else {
					pd->graphics->fillRect((x * ScreenScale ) + XOff,(y * ScreenScale ) + YOff, 1* ScreenScale, 1* ScreenScale, kColorBlack);
				}
			}
		}
	}
	else {
		pd->graphics->drawText("Rom Not Found :(", strlen("Rom Not Found :("), kASCIIEncoding, 400 / 2 - 99 / 2 , 100);
		pd->graphics->drawText("Please Put Rom.bin in CHIP8PD's Data Folder", strlen("Please Put Rom.bin in CHIP8PD's Data Folder"), kASCIIEncoding, 400 / 2 - 290 / 2 , 140);
	}

	return 1;
}
