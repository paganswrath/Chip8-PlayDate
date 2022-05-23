#include <stdbool.h>
#include <sys/time.h>

#include "pd_api.h"

PlaydateAPI* PD = NULL;

int _gettimeofday( struct timeval *tv, void *tzvp ) // Needs This To Compile ?
{
    uint64_t t = time(NULL);  
    tv->tv_sec = t / 1000000000; 
    tv->tv_usec = ( t % 1000000000 ) / 1000; 
    return 0; 
}

#define F 15

static unsigned char ScreenMemory[64][32];

static unsigned char SystemMemory[4096];
static unsigned char SystemRegisters[16];
static unsigned short SystemStack[16];
static unsigned short SystemIRegister = 0;
static unsigned short SystemProgramCounter = 0x200;
static unsigned short SystemStackPointer = 0;

static unsigned char SystemDelayTimer = 0 ;
static unsigned char SystemSoundTimer = 0;
static unsigned char SystemKey = 0x0006;

bool RomLoaded = false;

void RunCycle(){
	SystemDelayTimer = 0;

	if (SystemSoundTimer > 0) {
		--(SystemSoundTimer);
		if (SystemSoundTimer == 0) {
            // Play Noise Here
        }
	}

    bool JumpFlag = false;
    int i;
	int j;
	unsigned short Opcode;
    unsigned short JumpAddress;
	unsigned char X, Y, NN;
	Opcode = SystemMemory[SystemProgramCounter] << 8 | SystemMemory[SystemProgramCounter + 1];

	JumpAddress = Opcode & 0x0FFF;
	X = (Opcode & 0x0F00) >> 8;
	Y = (Opcode & 0x00F0) >> 4;
	NN = Opcode & 0x00FF;

	switch (Opcode & 0xF000) {
		case 0x0000:
			if (Opcode == 0x00E0) {
				ClearScreen(0);
			}
			else if (Opcode == 0x00EE) {

				SystemProgramCounter = SystemStack[SystemStackPointer];
				SystemStack[SystemStackPointer] = 0;
				if (SystemStackPointer > 0) {
					--(SystemStackPointer);
				}
			}
			break;
		case 0x1000:
			SystemProgramCounter = JumpAddress;
			JumpFlag = true;
			break;
		case 0x2000:
			if (SystemStack[SystemStackPointer] != 0) {
				++(SystemStackPointer);
			}
			SystemStack[SystemStackPointer] = SystemProgramCounter;
			SystemProgramCounter = JumpAddress;
			JumpFlag = true;
			break;
		case 0x3000:
			if (SystemRegisters[X] == NN) { SystemProgramCounter += 2; }
			break;
		case 0x4000:
			if (SystemRegisters[X] != NN) { SystemProgramCounter += 2; }
			break;
		case 0x5000:
			if (SystemRegisters[X] == SystemRegisters[Y]) { SystemProgramCounter += 2; }
			break;
		case 0x6000:
			SystemRegisters[X] = NN;
			break;
		case 0x7000:
			SystemRegisters[X] += NN;
			break;
		case 0x8000:
			switch (Opcode & 0x000F) {
				case 0x0000:
					SystemRegisters[X] = SystemRegisters[Y];
					break;
				case 0x0001:
					SystemRegisters[X] = SystemRegisters[X] | SystemRegisters[Y];
					break;
				case 0x0002:
					SystemRegisters[X] = SystemRegisters[X] & SystemRegisters[Y];
					break;
				case 0x0003:
					SystemRegisters[X] = SystemRegisters[X] ^ SystemRegisters[Y];
					break;
				case 0x0004:
					if (SystemRegisters[X] + SystemRegisters[Y] > 255) {
						SystemRegisters[F] = 1;
					} else {
						SystemRegisters[F] = 0;
					}
					SystemRegisters[X] += SystemRegisters[Y];
					break;
				case 0x0005:
					if (SystemRegisters[X] < SystemRegisters[Y]) {
						SystemRegisters[F] = 0;
					} else {
						SystemRegisters[F] = 1;
					}
					SystemRegisters[X] -= SystemRegisters[Y];
					break;
				case 0x0006:
					if (SystemRegisters[X] % 2 == 0) {
						SystemRegisters[F] = 0;
					} else {
						SystemRegisters[F] = 1;
					}
					SystemRegisters[X] = SystemRegisters[X] >> 1;
					break;
				case 0x0007:
					if (SystemRegisters[Y] < SystemRegisters[X]) {
						SystemRegisters[F] = 0;
					}
					else {
						SystemRegisters[F] = 1;
					}
					SystemRegisters[X] = SystemRegisters[Y] - SystemRegisters[X];
					break;
				case 0x000E:
					if (SystemRegisters[X] < 128) {
						SystemRegisters[F] = 0;
					} else {
						SystemRegisters[F] = 1;
					}
					SystemRegisters[X] = SystemRegisters[X] << 1;
					break;
			}
			break;
		case 0x9000:
			if (SystemRegisters[X] != SystemRegisters[Y]) { SystemProgramCounter += 2; }
			break;
		case 0xA000:
			SystemIRegister = Opcode & 0x0FFF;
			break;
		case 0xB000:
			SystemProgramCounter = JumpAddress + SystemRegisters[0];
			JumpFlag = true;
			break;
		case 0xC000:
			SystemRegisters[X] = (rand() % 255) & (Opcode & 0x00FF);
			break;
		case 0xD000: {

			int Height = Opcode & 0x000F;
			int XCoord = SystemRegisters[X];
			int YCoord = SystemRegisters[Y];
			
			int Ands[8] = { 128, 64, 32, 16, 8, 4, 2, 1 };

			SystemRegisters[F] = 0;
			for (i = 0; i < Height; i++) {
				for (j = 0; j < 8; j++) {
					if (XCoord + j == 64) {
						XCoord = -j;
					}
					if (YCoord + i == 32) {
						YCoord = -i;
					}
					if (ScreenMemory[XCoord + j][YCoord + i] == 1 &&
						((SystemMemory[SystemIRegister + i] & Ands[j]) >> (8 - j - 1)) == 1) {
						SystemRegisters[F] = 1;
					}
					ScreenMemory[XCoord + j][YCoord + i] = ScreenMemory[XCoord + j][YCoord + i] ^ 
						((SystemMemory[SystemIRegister + i] & Ands[j]) >> (8 - j - 1));
				}
				XCoord = SystemRegisters[X];
				YCoord = SystemRegisters[Y];
			}
			break;
		}
		case 0xE000:
			if (NN == 0x009E) {
				if (SystemKey == SystemRegisters[X]) {
					SystemProgramCounter += 2;
				}
			} else if (NN == 0x00A1) {
				if (SystemKey != SystemRegisters[X]) {
					SystemProgramCounter += 2;
				}
			}
			break;
		case 0xF000:
			switch (Opcode & 0x00FF) {
				case 0x0007:
					SystemRegisters[X] = SystemDelayTimer;
					break;
				case 0x000A:
				if (SystemKey == 0) {
						return;
					} else {
						SystemRegisters[X] = SystemKey;
					}
					break;
				case 0x0015:
					SystemDelayTimer = SystemRegisters[X];
					break;
				case 0x0018:
					SystemSoundTimer = SystemRegisters[X];
					break;
				case 0x001E:
					SystemIRegister += SystemRegisters[X];
					break;
				case 0x0029:
					SystemIRegister = SystemRegisters[X] * 5;
					break;
				case 0x0033:
					SystemMemory[SystemIRegister] = SystemRegisters[X] / 100;
					SystemMemory[SystemIRegister + 1] = (SystemRegisters[X] / 10) % 10;
					SystemMemory[SystemIRegister + 2] = SystemRegisters[X] % 10;
					break;
				case 0x0055:
					for (i = 0; i < X + 1; i++) {
						SystemMemory[SystemIRegister + i] = SystemRegisters[i];
					}
					SystemIRegister = SystemIRegister + X + 1;
					break;
				case 0x0065:
					for (i = 0; i < X + 1; i++) {
						SystemRegisters[i] = SystemMemory[SystemIRegister + i];
					}
					SystemIRegister = SystemIRegister + X + 1;
					break;
			}
		break;
	}


	if (!JumpFlag) { SystemProgramCounter += 2; }
}