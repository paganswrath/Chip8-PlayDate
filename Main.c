#include <stdio.h>
#include <stdlib.h>
#include "Memory.h"

static int Update(void* userdata);

static LCDFont* Font = NULL;

#ifdef _WINDLL
__declspec(dllexport)
#endif

LCDBitmap *ScreenMask;

bool LoadFlag = true;

static int Cursor = 0;
static int Delay = 0;
static int GameID = 0;
static bool SetInputs = false;
static int InputCursor = 0;
static bool InputCursorSelected = false;
static bool Skip = true;

int GetKepMapping(int ID){
	switch (ID)
	{
		case 0:
			return Key1;
			break;
		case 1:
			return Key2;
			break;
		case 2:
			return Key3;
			break;
		case 3:
			return Key4;
			break;
		case 4:
			return KeyQ;
			break;
		case 5:
			return KeyW;
			break;
		case 6:
			return KeyE;
			break;
		case 7:
			return KeyR;
			break;
		case 8:
			return KeyA;
			break;
		case 9:
			return KeyS;
			break;
		case 10:
			return KeyD;
			break;
		case 11:
			return KeyF;
			break;
		case 12:
			return KeyZ;
			break;
		case 13:
			return KeyX;
			break;
		case 14:
			return KeyC;
			break;
		case 15:
			return KeyV;
			break;
		default:
			break;
	}
}

static void MapInputsCallBack(){
	SetInputs = true;
}

static void MaxCallBack(){
	if (HighPreformance)HighPreformance = false;
	else {
		HighPreformance = true;
	}
}
static void RebootCallBack(){
	RomLoaded = false;
	LoadFlag = true;

	SetInputs = false;
}

static void InterlacedCallBack(){
	if(Interlaced)Interlaced = false;
	else {
		Interlaced = true;
	}
}

static void UpdateInputs(){
	PDButtons Current;
	PDButtons Released;
	PD->system->getButtonState(&Current, NULL, &Released);
	switch (GameID)
	{
		case 0:
			if (Current & kButtonUp){SystemKey = UpMapping;}
			if (Released & kButtonUp){SystemKey = KeyNull;}

			if (Current & kButtonDown){SystemKey = DownMapping;}
			if (Released & kButtonDown){SystemKey = KeyNull;}

			if (Current & kButtonLeft){SystemKey = LeftMapping;}
			if (Released & kButtonLeft){SystemKey = KeyNull;}

			if (Current & kButtonRight){SystemKey = RightMapping;}
			if (Released & kButtonRight){SystemKey = KeyNull;}

			if (Current & kButtonA){SystemKey = AMapping;}
			if (Released & kButtonA){SystemKey = KeyNull;}

			if (Current & kButtonB){SystemKey = SMapping;}
			if (Released & kButtonB){SystemKey = KeyNull;}

			break;
		case 1:
			if (Current & kButtonUp){
				SystemKey = Key1;
			}
			if (Released & kButtonUp){
				SystemKey = KeyNull;
			}

			if (Current & kButtonDown){
				SystemKey = KeyQ;
			}
			if (Released & kButtonDown){
				SystemKey = KeyNull;
			}
			break;
		case 2:
			if (Current & kButtonUp){
				SystemKey = KeyS;
			}
			if (Released & kButtonUp){
				SystemKey = KeyNull;
			}
			break;
		case 3:
			if (Current & kButtonA){
				SystemKey = KeyW;
			}
			if (Released & kButtonA){
				SystemKey = KeyNull;
			}

			if (Current & kButtonLeft){
				SystemKey = KeyQ;
			}
			if (Released & kButtonLeft){
				SystemKey = KeyNull;
			}

			if (Current & kButtonRight){
				SystemKey = KeyE;
			}
			if (Released & kButtonRight){
				SystemKey = KeyNull;
			}
			break;
		case 4:
			if (Current & kButtonRight){
				SystemKey = KeyE;
			}
			if (Released & kButtonRight){
				SystemKey = KeyNull;
			}

			if (Current & kButtonLeft){
				SystemKey = KeyQ;
			}
			if (Released & kButtonLeft){
				SystemKey = KeyNull;
			}
			break;
		
		default:
			break;
	}
}

void SetPlayDateApi(PlaydateAPI* p) {
	PD = p;
}

const char* FontPath = "/System/Fonts/Roobert-10-Bold.pft";

int eventHandler(PlaydateAPI* PlayDate, PDSystemEvent Event, uint32_t Arg)
{
	(void)Arg;

	if ( Event == kEventInit )
	{
		const char* Error;

		SetPlayDateApi(PlayDate);
		PlayDate->display->setRefreshRate(50);
		Font = PD->graphics->loadFont(FontPath , &Error);
		ScreenMask = LoadPng("Mask"); 
		PDMenuItem *MenuItem2 = PD->system->addMenuItem("Reboot", RebootCallBack, NULL);
		PDMenuItem *MenuItem4 = PD->system->addMenuItem("Fast CPU()", MaxCallBack, NULL);
		PDMenuItem *MenuItem3 = PD->system->addMenuItem("Interlaced", InterlacedCallBack, NULL);
		PDMenuItem *MenuItem1 = PD->system->addMenuItem("Map Inputs", MapInputsCallBack, NULL);
		PD->system->setUpdateCallback(Update, NULL);

	}
	
	return 0;
}

static int Update(void* userdata)
{

	if (FrameTimer != 50){
		FrameTimer ++;
	}
	else {
		FrameTimer = 0;
	}

	if (LoadFlag){
		InitMemory();
		ClearScreen(0x00);
		LoadFlag = false;
	}
	
	PD->graphics->clear(kColorWhite);

	PD->graphics->setFont(Font);

	if (!SetInputs){


		if (RomLoaded){
			PD->graphics->drawText("CHIP8", strlen("CHIP8"), kASCIIEncoding, 400 / 2 - (5 * 6.6) / 2 , 3);
			UpdateInputs();

			if (HighPreformance){
				RunCycleFast(); 
			}
			else {
				RunCycle(); 
			}

			DrawScreen();
			
		}
		else {
			PD->graphics->drawText("Select Game:", strlen("Select Game:"), kASCIIEncoding, 400 / 2 - 86 / 2 , 10);
			PD->graphics->fillRect(5,100,390, 125, kColorBlack);
			PD->graphics->fillRect(2,97,396, 131, kColorWhite);

			for (int i = 0 ; i < 5 ; i ++){
				PD->graphics->drawRect(5,100 + i * 25,390, 25, kColorBlack);

				PD->graphics->fillRect(10,100 + (Cursor * 25) + 5,15, 15, kColorBlack);

				switch (i)
				{
					case 0:
						PD->graphics->drawText("Custom Rom", strlen("Custom Rom"), kASCIIEncoding, 400 / 2 - (11*6.6) / 2 , (100 + i * 25) + 2);
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
			PD->graphics->drawBitmap(ScreenMask, 0, 0, 0);
			

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
					Delay = 15 + abs(PD->system->getCrankChange());
				}
				if (PD->system->getCrankChange() > 0 && Delay == 0 && Cursor < 4){
					Cursor ++;
					Delay = 15 + abs(PD->system->getCrankChange());
				}
				else {
					if (Cursor == 4 && PD->system->getCrankChange() > 0){
						Cursor = 0;
					}
				}
				
			}
			if (Current & kButtonA && Delay == 0){
				switch (Cursor)
				{
					case 0:
						LoadCustomRom("Rom.bin");
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
	}
	else {
		//
		PD->graphics->drawText("Map Inputs", strlen("Map Inputs"), kASCIIEncoding, 400 / 2 - (11*6.6) / 2 , 2);

		for (int i = 0 ; i <= 15 ; i ++){
			if (i < 8){
				PD->graphics->drawRect(5,30,50, 20 + i * 20, kColorBlack);
			}
			else {
				PD->graphics->drawRect(200,30,50, 20 + (i - 8) * 20, kColorBlack);
			}

			if (InputCursor < 8){
				if (!InputCursorSelected)PD->graphics->drawRect(57 , 32 + InputCursor * 20 , 15, 15, kColorBlack);
				else{
					PD->graphics->fillRect(57 , 32 + InputCursor * 20 , 15, 15, kColorBlack);
					PD->graphics->drawText("Press Key", strlen("Press Key"), kASCIIEncoding, 400 / 2 - (10*6.6) / 2 , 220);
				}
			}
			else {
				if (!InputCursorSelected)PD->graphics->drawRect(57 + 195 , 32 + (InputCursor - 8)* 20 , 15, 15, kColorBlack);
				else{
					PD->graphics->fillRect(57 + 195 , 32 + (InputCursor - 8)* 20 , 15, 15, kColorBlack);
					PD->graphics->drawText("Press Key", strlen("Press Key"), kASCIIEncoding, 400 / 2 - (10*6.6) / 2 , 220);
				}
			}

			if (i < 8){
				switch (i)
				{
					case 0:
						PD->graphics->drawText("1", strlen("1"), kASCIIEncoding, 15,30 + i * 20);
						break;
					case 1:
						PD->graphics->drawText("2", strlen("2"), kASCIIEncoding, 15,30 + i * 20);
						break;
					case 2:
						PD->graphics->drawText("3", strlen("3"), kASCIIEncoding, 15,30 + i * 20);
						break;
					case 3:
						PD->graphics->drawText("4", strlen("4"), kASCIIEncoding, 15,30 + i * 20);
						break;
					case 4:
						PD->graphics->drawText("Q", strlen("Q"), kASCIIEncoding, 15,30 + i * 20);
						break;
					case 5:
						PD->graphics->drawText("W", strlen("W"), kASCIIEncoding, 15,30 + i * 20);
						break;
					case 6:
						PD->graphics->drawText("E", strlen("E"), kASCIIEncoding, 15,30 + i * 20);
						break;
					case 7:
						PD->graphics->drawText("R", strlen("R"), kASCIIEncoding, 15,30 + i * 20);
						break;
					
					default:
						break;
				}
			}
			else {
				switch (i)
				{
					case 8:
						PD->graphics->drawText("A", strlen("A"), kASCIIEncoding, 15 + 195,30 + (i - 8) * 20);
						break;
					case 9:
						PD->graphics->drawText("S", strlen("A"), kASCIIEncoding, 15 + 195,30 + (i - 8) * 20);
						break;
					case 10:
						PD->graphics->drawText("D", strlen("A"), kASCIIEncoding, 15 + 195,30 + (i - 8) * 20);
						break;
					case 11:
						PD->graphics->drawText("F", strlen("A"), kASCIIEncoding, 15 + 195,30 + (i - 8) * 20);
						break;
					case 12:
						PD->graphics->drawText("Z", strlen("A"), kASCIIEncoding, 15 + 195,30 + (i - 8) * 20);
						break;
					case 13:
						PD->graphics->drawText("X", strlen("A"), kASCIIEncoding, 15 + 195,30 + (i - 8) * 20);
						break;
					case 14:
						PD->graphics->drawText("C", strlen("A"), kASCIIEncoding, 15 + 195,30 + (i - 8) * 20);
						break;
					case 15:
						PD->graphics->drawText("V", strlen("A"), kASCIIEncoding, 15 + 195,30 + (i - 8) * 20);
						break;
					
					default:
						break;
				}
			}

			if (i < 8){
				if (UpMapping == GetKepMapping(i)){
					PD->graphics->drawText("- Mapped To Up", strlen("- Mapped To Up"), kASCIIEncoding, 15 + 50 ,30 + i * 20);
				}
				if (DownMapping == GetKepMapping(i)){
					PD->graphics->drawText("- Mapped To Down", strlen("- Mapped To Down"), kASCIIEncoding, 15 + 50 ,30 + i * 20);
				}
				if (LeftMapping == GetKepMapping(i)){
					PD->graphics->drawText("- Mapped To Left", strlen("- Mapped To Left"), kASCIIEncoding, 15 + 50 ,30 + i * 20);
				}
				if (RightMapping == GetKepMapping(i)){
					PD->graphics->drawText("- Mapped To Right", strlen("- Mapped To Right"), kASCIIEncoding, 15 + 50 ,30 + i * 20);
				}
				if (AMapping == GetKepMapping(i)){
					PD->graphics->drawText("- Mapped To A", strlen("- Mapped To A"), kASCIIEncoding, 15 + 50  ,30 + i  * 20);
				}
				if (SMapping == GetKepMapping(i)){
					PD->graphics->drawText("- Mapped To B", strlen("- Mapped To B"), kASCIIEncoding, 15 + 50  ,30 + i  * 20);
				}
			}
			else {
				if (UpMapping == GetKepMapping(i)){
					PD->graphics->drawText("- Mapped To Up", strlen("- Mapped To Up"), kASCIIEncoding, 15 + 50 + 195 ,30 + (i - 8) * 20);
				}
				if (DownMapping == GetKepMapping(i)){
					PD->graphics->drawText("- Mapped To Down", strlen("- Mapped To Down"), kASCIIEncoding, 15 + 50 + 195 ,30 + (i - 8) * 20);
				}
				if (LeftMapping == GetKepMapping(i)){
					PD->graphics->drawText("- Mapped To Left", strlen("- Mapped To Left"), kASCIIEncoding, 15 + 50 + 195 ,30 + (i - 8) * 20);
				}
				if (RightMapping == GetKepMapping(i)){
					PD->graphics->drawText("- Mapped To Right", strlen("- Mapped To Right"), kASCIIEncoding, 15 + 50 + 195 ,30 + (i - 8) * 20);
				}
				if (AMapping == GetKepMapping(i)){
					PD->graphics->drawText("- Mapped To A", strlen("- Mapped To A"), kASCIIEncoding, 15 + 50 + 195 ,30 + (i - 8) * 20);
				}
				if (SMapping == GetKepMapping(i)){
					PD->graphics->drawText("- Mapped To B", strlen("- Mapped To B"), kASCIIEncoding, 15 + 50 + 195 ,30 + (i - 8) * 20);
				}
			}

			PDButtons Current;
			PD->system->getButtonState(&Current, NULL, NULL);

			if (Current & kButtonDown && Delay == 0 && InputCursor < 15){
				InputCursor ++;
				if(UpMapping == GetKepMapping(InputCursor))InputCursor ++;
				if(DownMapping == GetKepMapping(InputCursor))InputCursor ++;
				if(LeftMapping == GetKepMapping(InputCursor))InputCursor ++;
				if(RightMapping == GetKepMapping(InputCursor))InputCursor ++;

				Delay = 50;
			}
			if (Current & kButtonUp && Delay == 0 && InputCursor > 0){
				InputCursor --;
				if(UpMapping == GetKepMapping(InputCursor))InputCursor --;
				if(DownMapping == GetKepMapping(InputCursor))InputCursor --;
				if(LeftMapping == GetKepMapping(InputCursor))InputCursor --;
				if(RightMapping == GetKepMapping(InputCursor))InputCursor --;
				Delay = 50;
			}

			if (Current & kButtonA){
				InputCursorSelected = true;
				Skip = true;
			}

			if (InputCursorSelected && !Skip){
				if (Current & kButtonUp){
					if (InputCursor == 0 || InputCursor == 1)UpMapping = GetKepMapping(InputCursor);
					else {
						UpMapping = GetKepMapping(InputCursor + 1);
					}
					InputCursor = 0;
					InputCursorSelected = false;
				}
				if (Current & kButtonDown){
					if (InputCursor == 0 || InputCursor == 1)DownMapping = GetKepMapping(InputCursor);
					else {
						DownMapping = GetKepMapping(InputCursor + 1);
					}
					InputCursor = 0;
					InputCursorSelected = false;
				}
				if (Current & kButtonLeft){
					if (InputCursor == 0 || InputCursor == 1)LeftMapping = GetKepMapping(InputCursor);
					else {
						LeftMapping = GetKepMapping(InputCursor + 1);
					}
					InputCursor = 0;
					InputCursorSelected = false;
				}
				if (Current & kButtonRight){
					if (InputCursor == 0 || InputCursor == 1)RightMapping = GetKepMapping(InputCursor);
					else {
						RightMapping = GetKepMapping(InputCursor + 1);
					}
					InputCursor = 0;
					InputCursorSelected = false;
				}
				if (Current & kButtonA){
					if (InputCursor == 0 || InputCursor == 1)AMapping = GetKepMapping(InputCursor);
					else {
						AMapping = GetKepMapping(InputCursor + 1);
					}
					InputCursor = 0;
					InputCursorSelected = false;
				}
				if (Current & kButtonB){
					if (InputCursor == 0 || InputCursor == 1)SMapping = GetKepMapping(InputCursor);
					else {
						SMapping = GetKepMapping(InputCursor + 1);
					}
					InputCursor = 0;
					InputCursorSelected = false;
				}
			}

			if (Skip){
				Skip = false;
			}

			if (Delay != 0){
				Delay --;
			}
		}

	}
	return 1;
}
