#include <stdio.h>
#include <stdlib.h>
#include "Memory.h"

static int Update(void* userdata);

static LCDFont* Font = NULL;

#ifdef _WINDLL
__declspec(dllexport)
#endif

void SetPlayDateApi(PlaydateAPI* p) {
	PD = p;
}

const char* FontPath = "/System/Fonts/Asheville-Sans-14-Bold.pft";

int eventHandler(PlaydateAPI* PlayDate, PDSystemEvent Event, uint32_t Arg)
{
	(void)Arg;

	if ( Event == kEventInit )
	{
		const char* Error;

		SetPlayDateApi(PlayDate);
		PlayDate->display->setRefreshRate(50);
		Font = PD->graphics->loadFont(FontPath , &Error);
		PD->system->setUpdateCallback(Update, NULL);

	}
	
	return 0;
}

bool LoadFlag = true;

static int XOff = 7;
static int YOff = 24;
static int Cursor = 0;
static int Delay = 0;
static int GameID;


static int Update(void* userdata)
{

	

	if (LoadFlag){
		InitMemory();
		ClearScreen(0x00);
		LoadFlag = false;
	}
	
	PD->graphics->clear(kColorWhite);

	PD->graphics->setFont(Font);


	if (RomLoaded){
		PD->graphics->drawText("CHIP8", strlen("CHIP8"), kASCIIEncoding, 400 / 2 - (5 * 6.6) / 2 , 3);
		PD->graphics->drawText("Crank To Go Home", strlen("Crank To Go Home"), kASCIIEncoding, 400 / 2 - (17 * 6.6) / 2 , 240 - 20);
		PDButtons Current;
		PDButtons Released;
		PD->system->getButtonState(&Current, NULL, &Released);
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
		
		for (int x = 0 ; x < ScreenWidth; x ++){
			for (int y = 0 ; y < ScreenHeight; y ++){
				if (ScreenMemory[x][y] == 1){
					PD->graphics->fillRect((x * ScreenScale ) + XOff,(y * ScreenScale ) + YOff, 1* ScreenScale, 1* ScreenScale, kColorWhite);
				}
				else {
					PD->graphics->fillRect((x * ScreenScale ) + XOff,(y * ScreenScale ) + YOff, 1* ScreenScale, 1* ScreenScale, kColorBlack);
				}
			}
		}

		
		if (!PD->system->isCrankDocked()){
			if (PD->system->getCrankChange() != 0){
				RomLoaded = false;
				LoadFlag = true;
			}
		}
	}
	else {
		PD->graphics->drawText("Select Game:", strlen("Select Game:"), kASCIIEncoding, 400 / 2 - 86 / 2 , 30);
		PD->graphics->drawRect(5,100,390, 125, kColorBlack);
		PD->graphics->drawRect(2,97,396, 131, kColorBlack);

		for (int i = 0 ; i < 5 ; i ++){
			PD->graphics->drawRect(5,100 + i * 25,390, 25, kColorBlack);

			PD->graphics->fillRect(10,100 + (Cursor * 25) + 5,15, 15, kColorBlack);

			switch (i)
			{
				case 0:
					PD->graphics->drawText("Chip8 Test Rom", strlen("Chip8 Test Rom"), kASCIIEncoding, 400 / 2 - 99 / 2 , (100 + i * 25) + 2);
					break;
				case 1:
					PD->graphics->drawText("PONG", strlen("PONG"), kASCIIEncoding, 400 / 2 - ( 4 * 6.6) / 2 , (100 + i * 25) + 2);
					break;
				case 2:
					PD->graphics->drawText("MISSILE", strlen("MISSILE"), kASCIIEncoding, 400 / 2 - ( 8 * 6.6) / 2 , (100 + i * 25) + 2);
					break;
				case 3:
					PD->graphics->drawText("SPACE INVADERS", strlen("SPACE INVADERS"), kASCIIEncoding, 400 / 2 - ( 15 * 6.6) / 2 , (100 + i * 25) + 2);
					break;
				case 4:
					PD->graphics->drawText("BREAKOUT", strlen("BREAKOUT"), kASCIIEncoding, 400 / 2 - ( 9 * 6.6) / 2 , (100 + i * 25) + 2);
					break;
				default:
					break;
			}
		}
		

		PDButtons Current;
		PD->system->getButtonState(&Current, NULL, NULL);

		if (PD->system->isCrankDocked()){
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
			if (PD->system->getCrankChange() < 0 && Delay == 0 && Cursor > 0){
				Cursor --;
				Delay = 5;
			}
			if (PD->system->getCrankChange() > 0 && Delay == 0 && Cursor < 4){
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
