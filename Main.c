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

const char* FontPath = "/System/Fonts/Asheville-Sans-14-Bold.pft";

int eventHandler(PlaydateAPI* playdate, PDSystemEvent event, uint32_t arg)
{
	(void)arg;

	if ( event == kEventInit )
	{
		const char* err;

		setPDPtr(playdate);
		playdate->display->setRefreshRate(50);
		Font = pd->graphics->loadFont(FontPath , &err);
		pd->system->setUpdateCallback(update, NULL);

	}
	
	return 0;
}

bool FirstLoad = true;

static int XOff = 7;
static int YOff = 24;
static int Cursor = 0;
static int Delay = 0;
static int GameID;


static int update(void* userdata)
{

	

	if (FirstLoad){
		InitMemory();
		ClearScreen(0x00);
		FirstLoad = false;
	}
	

	pd->graphics->clear(kColorWhite);

	pd->graphics->setFont(Font);


	if (RomLoaded){
		pd->graphics->drawText("CHIP8", strlen("CHIP8"), kASCIIEncoding, 400 / 2 - (5 * 6.6) / 2 , 3);
		pd->graphics->drawText("Crank To Go Home", strlen("Crank To Go Home"), kASCIIEncoding, 400 / 2 - (17 * 6.6) / 2 , 240 - 20);
		PDButtons Current;
		PDButtons Released;
		pd->system->getButtonState(&Current, NULL, &Released);
		switch (GameID)
		{
			case 1:
				if (Current & kButtonUp){
					SystemKey = 0x0001;
				}
				if (Released & kButtonUp){
					SystemKey = 0x00FF;
				}

				if (Current & kButtonDown){
					SystemKey = 0x0004;
				}
				if (Released & kButtonDown){
					SystemKey = 0x00FF;
				}
				break;
			case 2:
				if (Current & kButtonUp){
					SystemKey = 0x0008;
				}
				if (Released & kButtonUp){
					SystemKey = 0x00FF;
				}
				break;
			case 3:
				if (Current & kButtonA){
					SystemKey = 0x0005;
				}
				if (Released & kButtonA){
					SystemKey = 0x00FF;
				}

				if (Current & kButtonLeft){
					SystemKey = 0x0004;
				}
				if (Released & kButtonLeft){
					SystemKey = 0x00FF;
				}

				if (Current & kButtonRight){
					SystemKey = 0x006;
				}
				if (Released & kButtonRight){
					SystemKey = 0x00FF;
				}
				break;
			case 4:
				if (Current & kButtonRight){
					SystemKey = 0x0006;
				}
				if (Released & kButtonRight){
					SystemKey = 0x00FF;
				}

				if (Current & kButtonLeft){
					SystemKey = 0x0004;
				}
				if (Released & kButtonLeft){
					SystemKey = 0x00FF;
				}
				break;
			
			default:
				break;
		}

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

		
		if (!pd->system->isCrankDocked()){
			if (pd->system->getCrankChange() != 0){
				RomLoaded = false;
				FirstLoad = true;
			}
		}
	}
	else {
		pd->graphics->drawText("Select Game:", strlen("Select Game:"), kASCIIEncoding, 400 / 2 - 86 / 2 , 30);
		pd->graphics->drawRect(5,100,390, 125, kColorBlack);
		pd->graphics->drawRect(2,97,396, 131, kColorBlack);

		for (int i = 0 ; i < 5 ; i ++){
			pd->graphics->drawRect(5,100 + i * 25,390, 25, kColorBlack);

			pd->graphics->fillRect(10,100 + (Cursor * 25) + 5,15, 15, kColorBlack);

			switch (i)
			{
				case 0:
					pd->graphics->drawText("Chip8 Test Rom", strlen("Chip8 Test Rom"), kASCIIEncoding, 400 / 2 - 99 / 2 , (100 + i * 25) + 2);
					break;
				case 1:
					pd->graphics->drawText("PONG", strlen("PONG"), kASCIIEncoding, 400 / 2 - ( 4 * 6.6) / 2 , (100 + i * 25) + 2);
					break;
				case 2:
					pd->graphics->drawText("MISSILE", strlen("MISSILE"), kASCIIEncoding, 400 / 2 - ( 8 * 6.6) / 2 , (100 + i * 25) + 2);
					break;
				case 3:
					pd->graphics->drawText("SPACE INVADERS", strlen("SPACE INVADERS"), kASCIIEncoding, 400 / 2 - ( 15 * 6.6) / 2 , (100 + i * 25) + 2);
					break;
				case 4:
					pd->graphics->drawText("BREAKOUT", strlen("BREAKOUT"), kASCIIEncoding, 400 / 2 - ( 9 * 6.6) / 2 , (100 + i * 25) + 2);
					break;
				default:
					break;
			}
		}
		

		PDButtons Current;
		pd->system->getButtonState(&Current, NULL, NULL);

		if (pd->system->isCrankDocked()){
			if (Current & kButtonDown && Delay == 0 && Cursor < 4){
				Cursor ++;
				Delay = 8;
			}
			if (Current & kButtonUp && Delay == 0 && Cursor > 0){
				Cursor --;
				Delay = 8;
			}
		}
		else {
			if (pd->system->getCrankChange() < 0 && Delay == 0 && Cursor > 0){
				Cursor --;
				Delay = 5;
			}
			if (pd->system->getCrankChange() > 0 && Delay == 0 && Cursor < 4){
				Cursor ++;
				Delay = 5;
			}
			
		}
		if (Current & kButtonA && Delay == 0){
			switch (Cursor)
			{
				case 0:
					LoadRom("TestRom.bin");
					break;
				case 1:
					LoadRom("PONG.bin");
					break;
				case 2:
					LoadRom("MISSILE.bin");
					break;
				case 3:
					LoadRom("SPACE.bin");
					break;
				case 4:
					LoadRom("BREAK.bin");
					break;
				
				default:
					break;
			}
			GameID = Cursor;
		}
		if (Delay != 0){
			Delay --;
		}
		
	}
	
	return 1;
}
